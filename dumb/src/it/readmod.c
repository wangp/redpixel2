/*  _______         ____    __         ___    ___
 * \    _  \       \    /  \  /       \   \  /   /     '   '  '
 *  |  | \  \       |  |    ||         |   \/   |       .      .
 *  |  |  |  |      |  |    ||         ||\  /|  |
 *  |  |  |  |      |  |    ||         || \/ |  |       '  '  '
 *  |  |  |  |      |  |    ||         ||    |  |       .      .
 *  |  |_/  /        \  \__//          ||    |  |
 * /_______/edicated  \____/niversal  /__\  /____\usic /|  .  . astardisation
 *                                                    /  \
 *                                                   / .  \
 * readmod.c - Code to read a good old-fashioned    / / \  \
 *             Amiga module from an open file.     | <  /   \_
 *                                                 |  \/ /\   /
 * By Ben Davis.                                    \_  /  > /
 *                                                    | \ / /
 *                                                    |  ' /
 *                                                     \__/
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "dumb.h"
#include "internal/it.h"



static int it_mod_read_pattern(IT_PATTERN *pattern, DUMBFILE *f, int n_channels)
{
	static unsigned char buffer[8192]; /* 32 channels * 64 rows * 4 bytes */
	int pos;
	int channel;
	int row;
	IT_ENTRY *entry;

	pattern->n_rows = 64;

	if (n_channels == 0) {
		/* Read the first four channels, leaving gaps for the rest. */
		for (pos = 0; pos < 64*8*4; pos += 8*4)
			dumbfile_getnc(buffer + pos, 4*4, f);
		/* Read the other channels into the gaps we left. */
		for (pos = 4*4; pos < 64*8*4; pos += 8*4)
			dumbfile_getnc(buffer + pos, 4*4, f);

		n_channels = 8;
	} else
		dumbfile_getnc(buffer, 64 * n_channels * 4, f);

	if (dumbfile_error(f))
		return -1;

	/* compute number of entries */
	pattern->n_entries = 64; /* Account for the row end markers */
	pos = 0;
	for (row = 0; row < 64; row++) {
		for (channel = 0; channel < n_channels; channel++) {
			if (buffer[pos+0] | buffer[pos+1] | buffer[pos+2] | buffer[pos+3])
				pattern->n_entries++;
			pos += 4;
		}
	}

	pattern->entry = malloc(pattern->n_entries * sizeof(*pattern->entry));
	if (!pattern->entry)
		return -1;

	entry = pattern->entry;
	pos = 0;
	for (row = 0; row < 64; row++) {
		for (channel = 0; channel < n_channels; channel++) {
			if (buffer[pos+0] | buffer[pos+1] | buffer[pos+2] | buffer[pos+3]) {
				unsigned char sample = (buffer[pos+0] & 0xF0) | (buffer[pos+2] >> 4);
				int period = ((int)(buffer[pos+0] & 0x0F) << 8) | buffer[pos+1];

				entry->channel = channel;
				entry->mask = 0;

				if (period) {
					int note;
					entry->mask |= IT_ENTRY_NOTE;

					/* frequency = (AMIGA_DIVISOR / 8) / (period * 2)
					 * C-1: period = 214 -> frequency = 16726
					 * so, set C5_speed to 16726
					 * and period = 214 should translate to C5 aka 60
					 * halve the period, go up an octive
					 *
					 * period = 214 / pow(DUMB_SEMITONE_BASE, note - 60)
					 * pow(DUMB_SEMITONE_BASE, note - 60) = 214 / period
					 * note - 60 = log(214/period) / log(DUMB_SEMITONE_BASE)
					 */
					note = floor(log(214.0/period) / log(DUMB_SEMITONE_BASE) + 60.5);
					entry->note = MID(0, note, 119);
					// or should we preserve the period?
					//entry->note = buffer[pos+0] & 0x0F; /* High nibble */
					//entry->volpan = buffer[pos+1]; /* Low byte */
					// and what about finetune?
				}

				if (sample) {
					entry->mask |= IT_ENTRY_INSTRUMENT;
					entry->instrument = sample;
				}

				_dumb_it_xm_convert_effect(buffer[pos+2] & 0x0F, buffer[pos+3], entry);

				entry++;
			}
			pos += 4;
		}
		IT_SET_END_ROW(entry);
		entry++;
	}

	return 0;
}



/* This function does not skip the name (22 bytes); it is assumed the caller
 * has already done that.
 */
static int it_mod_read_sample_header(IT_SAMPLE *sample, DUMBFILE *f)
{
	int finetune;

/**
     21       22   Chars     Sample 1 name.  If the name is not a full
                             22 chars in length, it will be null
                             terminated.

If
the sample name begins with a '#' character (ASCII $23 (35)) then this is
assumed not to be an instrument name, and is probably a message.
*/
	sample->length = dumbfile_mgetw(f) << 1;
	finetune = (signed char)(dumbfile_getc(f) << 4) >> 4; /* signed nibble */
/** Each  finetune step changes  the note 1/8th  of  a  semitone. */
	sample->global_volume = 64;
	sample->default_volume = dumbfile_getc(f); // Should we be setting global_volume to this instead?
	sample->loop_start = dumbfile_mgetw(f) << 1;
	sample->loop_end = sample->loop_start + (dumbfile_mgetw(f) << 1);
/**
Once this sample has been played completely from beginning
to end, if the  repeat length (next field)  is greater than two  bytes it
will loop back to this position in the sample and continue playing.  Once
it has played for  the repeat length,  it continues to  loop back to  the
repeat start offset.  This means the sample continues playing until it is
told to stop.
*/

	if (sample->length <= 0) {
		sample->flags = 0;
		return 0;
	}

	sample->flags = IT_SAMPLE_EXISTS;

	sample->default_pan = 0;
	sample->C5_speed = (long)(16726.0*pow(DUMB_PITCH_BASE, finetune*32));
	// the above line might be wrong

	if (sample->loop_end > sample->length)
		sample->loop_end = sample->length;

	if (sample->loop_end - sample->loop_start > 2)
		sample->flags |= IT_SAMPLE_LOOP;

	sample->vibrato_speed = 0;
	sample->vibrato_depth = 0;
	sample->vibrato_rate = 0;
	sample->vibrato_waveform = 0; // do we have to set _all_ these?

	return dumbfile_error(f);
}



static int it_mod_read_sample_data(IT_SAMPLE *sample, DUMBFILE *f)
{
	long i, n;
	long size;
	long truncated_size;

	/* let's get rid of the sample data coming after the end of the loop */
	if ((sample->flags & IT_SAMPLE_LOOP) && sample->loop_end < sample->length) {
		truncated_size = sample->length - sample->loop_end;
		sample->length = sample->loop_end;
	} else {
		truncated_size = 0;
	}

	sample->left = malloc((sample->length+DUMB_EXTRA_SAMPLES) * sizeof(*sample->left));

	if (!sample->left)
		return -1;

	/* Sample data are stored in "8-bit two's compliment format" (sic). */
	for (i = 0; i < sample->length; i++) {
		sample->left[i] = (int)(signed char)dumbfile_getc(f) << 8;
	}

	/* skip truncated data */
	dumbfile_skip(f, truncated_size);
	// Should we be truncating it?

	if (dumbfile_error(f))
		return -1;

	// NOTE: the following code is virtually a duplicate of some code in
	// sample.c. It may be possible to make it a helper function -
	// especially since sigtype designers might find it useful.
	// Uhm, actually the following code is probably unsuitable for this
	// case anyway...

	// Given that it's the second time it's duplicated, I think yes, it
	// should definitely become a helper function ;). Modified to produce
	// correct extra samples for ping pong loops. Might be good for IT/S3M
	// too...

	// ... third time?! I need a vacation. A Brit using the word 'vacation'
	// definitely needs a vacation. Ping pong loops and stereo stuff are not
	// supported in this copy, since .mod files don't have them.

	size = sample->length;
	n = 0;

	if (sample->flags & IT_SAMPLE_LOOP) {

		long m = sample->loop_start;

		for (; n < DUMB_EXTRA_SAMPLES &&
		       size + n < sample->loop_end + DUMB_EXTRA_SAMPLES; n++)
		{
			sample->left[size + n] = sample->left[m];
			m++;
			if (m >= sample->loop_end)
				m = sample->loop_start;
		}
	}

	for (; n < DUMB_EXTRA_SAMPLES; n++)
		sample->left[size + n] = 0;

	return 0;
}



static DUMB_IT_SIGDATA *it_mod_load_sigdata(DUMBFILE *f)
{
	DUMB_IT_SIGDATA *sigdata;
	unsigned char tmpbuf[22];
	int n_channels;
	int i;

	/**
      1       20   Chars     Title of the song.  If the title is not a
                             full 20 chars in length, it will be null-
                             terminated.
	*/
	if (dumbfile_skip(f, 20))
		return NULL;

	sigdata = malloc(sizeof(*sigdata));
	if (!sigdata) return NULL;

	/* Assume 31 samples at first; we might only need to load 15 though. */
	sigdata->sample = malloc(31 * sizeof(*sigdata->sample));
	if (!sigdata->sample) {
		free(sigdata);
		return NULL;
	}

	sigdata->n_samples = 31;

	sigdata->order = NULL;
	sigdata->instrument = NULL;
	sigdata->pattern = NULL;
	sigdata->midi = NULL;
	sigdata->checkpoint = NULL;

	for (i = 0; i < 31; i++)
		sigdata->sample[i].right = sigdata->sample[i].left = NULL;

	for (i = 0; i < 15; i++) {
		if (dumbfile_skip(f, 22) ||
		    it_mod_read_sample_header(&sigdata->sample[i], f))
		{
			_dumb_it_unload_sigdata(sigdata);
			return NULL;
		}
	}

	/* MOD files are evil. We have to check the next 22 bytes to see if they
	 * contain text. If they don't, it's a 15-sample MOD. Now imagine a
	 * 15-channel MOD with 64 orders, a restart position of 32 and patterns
	 * 32 onwards playing first. Oopsie! Maybe I should make one of those.
	 *
	 * Whoever's responsible for this little complication should be shot.
	 */

	if (dumbfile_getnc(tmpbuf, 22, f) < 22) {
		_dumb_it_unload_sigdata(sigdata);
		return NULL;
	}

	for (i = 0; i < 22; i++) {
		static unsigned char nottext[256] = {
			/*   0 */ 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1,
			/*  16 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			/*  32 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			/*  48 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			/*  64 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			/*  80 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			/*  96 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			/* 112 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			/* 128 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			/* 144 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			/* 160 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			/* 176 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			/* 192 */ 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1,
			/* 208 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			/* 224 */ 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			/* 240 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0
		};
		if (nottext[tmpbuf[i]]) {
			/* We have encountered a non-text character, so 15 samples. */
			IT_SAMPLE *sample;
			sigdata->n_samples = 15;
			/* May as well try to save some memory. */
			sample = realloc(sigdata->sample, 15 * sizeof(*sample));
			if (sample) sigdata->sample = sample;
			goto samples_loaded_and_we_overshot_by_22_bytes;
		}
	}

	/* It was all text, so we'll assume 31 samples. */
	if (it_mod_read_sample_header(&sigdata->sample[15], f)) {
		_dumb_it_unload_sigdata(sigdata);
		return NULL;
	}
	for (i = 16; i < 31; i++) {
		if (dumbfile_skip(f, 22) ||
		    it_mod_read_sample_header(&sigdata->sample[i], f))
		{
			_dumb_it_unload_sigdata(sigdata);
			return NULL;
		}
	}

	if (dumbfile_getnc(tmpbuf, 22, f) < 22) {
		_dumb_it_unload_sigdata(sigdata);
		return NULL;
	}

	samples_loaded_and_we_overshot_by_22_bytes:

	sigdata->n_orders = tmpbuf[0];
	sigdata->restart_position = tmpbuf[1];
	// what if this is >= 127? what about with Fast Tracker II?

	if (sigdata->n_orders <= 0 || sigdata->n_orders > 128) { // is this right?
		_dumb_it_unload_sigdata(sigdata);
		return NULL;
	}

	//if (sigdata->restart_position >= sigdata->n_orders)
		//sigdata->restart_position = 0;

	sigdata->order = malloc(128); /* We may need to scan the extra ones! */
	if (!sigdata->order) {
		_dumb_it_unload_sigdata(sigdata);
		return NULL;
	}
	memcpy(sigdata->order, &tmpbuf[2], 20);
	if (dumbfile_getnc(sigdata->order + 20, 108, f) < 108) {
		_dumb_it_unload_sigdata(sigdata);
		return NULL;
	}

	/* "The old NST format contains only 15 samples (instead of 31). Further
	 * it doesn't contain a file format tag (id). So Pattern data offset is
	 * at 20+15*30+1+1+128."
	 * - Then I shall assume the File Format Tag never exists if there are
	 * only 15 samples. I hope this isn't a faulty assumption...
	 */

	if (sigdata->n_samples == 15)
		n_channels = 4;
	else {
		unsigned long fft = dumbfile_mgetl(f);

		if (dumbfile_error(f)) {
			_dumb_it_unload_sigdata(sigdata);
			return NULL;
		}

		if ((fft & 0x0000FFFFL) == DUMB_ID(0,0,'C','H')) {
			n_channels = ((fft & 0x00FF0000L) >> 16) - '0';
			if ((unsigned int)n_channels >= 10)
				/* Rightmost character wasn't a digit - assume four channels. */
				n_channels = 4;
			else {
				n_channels += (((fft & 0xFF000000L) >> 24) - '0') * 10;
				if ((unsigned int)(n_channels - 1) >= 32 - 1)
					// Might be possible to allow up to DUMB_IT_N_CHANNELS later... (when the static buffer in it_mod_read_pattern changes to a malloc)
					/* No channels or too many? Can't be right... assume 4. */
					n_channels = 4;
			}
		} else if ((fft & 0x00FFFFFFL) == DUMB_ID(0,'C','H','N')) {
			n_channels = (fft >> 24) - '0';
			if ((unsigned int)(n_channels - 1) >= 9) {
				/* Character was '0' or it wasn't a digit */
				n_channels = 4;
			}
		} else if ((fft & 0xFFFFFF00L) == DUMB_ID('T','D','Z',0)) {
			n_channels = (fft & 0x000000FFL) - '0';
			if ((unsigned int)(n_channels - 1) >= 9)
				/* We've been very lenient, given that it should have been 1, 2
				 * or 3, but this MOD has been very naughty and must be punished.
				 */
				n_channels = 4;
		} else switch (fft) {
			case DUMB_ID('F','L','T','8'):
				n_channels = 0;
				/* 0 indicates a special case; two four-channel patterns must
				 * be combined into one eight-channel pattern. Pattern
				 * indexes must be halved. Why oh why do they obfuscate so?
				 */
				for (i = 0; i < 128; i++)
					sigdata->order[i] >>= 1;
				break;
			case DUMB_ID('O','C','T','A'):
			case DUMB_ID('C','D','8','1'):
				n_channels = 8;
				break;
			/* case DUMB_ID('M','.','K','.'): */
			/* case DUMB_ID('M','!','K','!'): */
			/* case DUMB_ID('F','L','T','4'): */
			default:
				n_channels = 4;
		}
	}

	/* Work out how many patterns there are. */
	sigdata->n_patterns = -1;
	for (i = 0; i < 128; i++)
		if (sigdata->n_patterns < sigdata->order[i])
			sigdata->n_patterns = sigdata->order[i];
	sigdata->n_patterns++;

	/* May as well try to save a tiny bit of memory. */
	if (sigdata->n_orders < 128) {
		unsigned char *order = realloc(sigdata->order, sigdata->n_orders);
		if (order) sigdata->order = order;
	}

	sigdata->pattern = malloc(sigdata->n_patterns * sizeof(*sigdata->pattern));
	if (!sigdata->pattern) {
		_dumb_it_unload_sigdata(sigdata);
		return NULL;
	}
	for (i = 0; i < sigdata->n_patterns; i++)
		sigdata->pattern[i].entry = NULL;

	/* Read in the patterns */

	for (i = 0; i < sigdata->n_patterns; i++) {
		if (it_mod_read_pattern(&sigdata->pattern[i], f, n_channels) != 0) {
			_dumb_it_unload_sigdata(sigdata);
			return NULL;
		}
	}

	/* And finally, the sample data */
	for (i = 0; i < sigdata->n_samples; i++) {
		if (it_mod_read_sample_data(&sigdata->sample[i], f)) {
			_dumb_it_unload_sigdata(sigdata);
			return NULL;
		}
	}

	/* Now let's initialise the remaining variables, and we're done! */
	sigdata->flags = IT_WAS_AN_XM | IT_WAS_A_MOD | IT_OLD_EFFECTS | IT_COMPATIBLE_GXX | IT_STEREO;

	sigdata->global_volume = 128;
	sigdata->mixing_volume = 48;
	/* We want 50 ticks per second; 50/6 row advances per second;
	 * 50*10=500 row advances per minute; 500/4=125 beats per minute.
	 */
	sigdata->speed = 6;
	sigdata->tempo = 125;
	sigdata->pan_separation = 128;

	memset(sigdata->channel_volume, 64, DUMB_IT_N_CHANNELS);

	for (i = 0; i < DUMB_IT_N_CHANNELS; i += 4) {
		sigdata->channel_pan[i+0] = 16;
		sigdata->channel_pan[i+1] = 48;
		sigdata->channel_pan[i+2] = 48;
		sigdata->channel_pan[i+3] = 16;
	}

	_dumb_it_fix_invalid_orders(sigdata);

	return sigdata;
}



/** WARNING: this is duplicated in itread.c, reads3m.c and readxm.c */
static DUH_SIGTYPE_DESC sigtype_it = {
	SIGTYPE_IT,
	NULL,
	&_dumb_it_start_sigrenderer,
	NULL,
	&_dumb_it_render_signal,
	&_dumb_it_end_sigrenderer,
	&_dumb_it_unload_sigdata
};



DUH *dumb_read_mod(DUMBFILE *f)
{
	sigdata_t *sigdata;
	long length;

	DUH_SIGTYPE_DESC *descptr = &sigtype_it;

	sigdata = it_mod_load_sigdata(f);

	if (!sigdata)
		return NULL;

	length = _dumb_it_build_checkpoints(sigdata);

	return make_duh(length, 1, &descptr, &sigdata);
}
