/*  loadaud, a .AUD sample format extension for Allegro.
 *  Copyright (C) 1998 Peter Wang.
 *  See readme.txt for more information.
 */

#include <allegro.h>
#include "loadaud.h"

static int index_table[16] = { 
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8 
};

static int stepsize_table[89] = {
    7,     8,     9,     10,    11,    12,     13,    14,    16,
    17,    19,    21,    23,    25,    28,     31,    34,    37,
    41,    45,    50,    55,    60,    66,     73,    80,    88,
    97,    107,   118,   130,   143,   157,    173,   190,   209,
    230,   253,   279,   307,   337,   371,    408,   449,   494,
    544,   598,   658,   724,   796,   876,    963,   1060,  1166,
    1282,  1411,  1552,  1707,  1878,  2066,   2272,  2499,  2749,
    3024,  3327,  3660,  4026,  4428,  4871,   5358,  5894,  6484,
    7132,  7845,  8630,  9493,  10442, 11487,  12635, 13899, 15289,
    16818, 18500, 20350, 22385, 24623, 27086,  29794, 32767 
};


/*  load_aud: loads an .aud file into memory.
 *  returns SAMPLE* on success, NULL on error.
 */ 

SAMPLE *load_aud_packfile(PACKFILE *f)
{
    SAMPLE *spl = NULL;
    unsigned short freq; 
    unsigned long size, outsize, chunksize; 
    unsigned char flags, type;
    unsigned long id;
    int stereo, ch;
    int index[2], step[2], valpred[2], vpdiff;
    int delta, store=0, low_nibble;
    signed short *o;

    freq    = pack_igetw(f);    /* frequency */
    size    = pack_igetl(f);    /* size of file (without header) */
    outsize = pack_igetl(f);    /* size of output data */
    flags   = pack_getc(f);     /* bit 0=stereo, 1=16bit */
    type    = pack_getc(f);     /* 1=Westwood compressed, 99=IMA ADPCM */

    if (type != 99 || !(flags & 2))
	goto getlost;

    if (flags & 1) stereo = 1;
    else stereo = 0;

    spl = create_sample(16, stereo, freq, outsize / 2 / (stereo ? 2 : 1));

    if (spl) {
	index[0] = valpred[0] = 0;
	index[1] = valpred[1] = 0;
	step[0] = stepsize_table[index[0]];
	step[1] = stepsize_table[index[1]];

	o = spl->data;

	while (size > 0) {
	    chunksize = pack_igetw(f);  /* size of compressed data */
	    outsize = pack_igetw(f);    /* size of output data */
	    id      = pack_igetl(f);    /* always 0x0000deaf */
	    size -= 8;

	    if (id != 0x0000deaf) {
		destroy_sample(spl);
		spl = NULL;
		goto getlost;
	    }

	    low_nibble = TRUE;
	    ch = 0;

	    while (chunksize > 0) {
		/* get next delta */
		if (low_nibble) {
		    store = pack_getc(f);
		    if (store == EOF) { 
			destroy_sample(spl);
			spl = NULL;
			goto getlost;
		    }
		    low_nibble = FALSE;
		    delta = store & 0xf;
		}
		else {
		    low_nibble = TRUE;
		    delta = (store>>4) & 0xf;
		    chunksize--;
		    size--;
		}

		/* compute difference and new predicted value */
		vpdiff = step[ch] >> 3;
		if (delta & 4) vpdiff += step[ch];
		if (delta & 2) vpdiff += step[ch] >> 1;
		if (delta & 1) vpdiff += step[ch] >> 2;

		/* calculate new sample value */
		if (delta & 0x8) valpred[ch] -= vpdiff;
		else valpred[ch] += vpdiff;

		/* clamp */
		if (valpred[ch] > 32767) valpred[ch] = 32767;
		else if (valpred[ch] < -32768) valpred[ch] = -32768;

		/* find new index value and update step value */
		index[ch] += index_table[delta];
		if (index[ch] < 0) index[ch] = 0;
		else if (index[ch] > 88) index[ch] = 88;
		step[ch] = stepsize_table[index[ch]];

		/* output */
		*o++ = valpred[ch]^0x8000;

		/* separate channel decoding */
		if (stereo) ch = !ch;
	    }
	}
    }

  getlost:
    
    return spl; 
}

SAMPLE *load_aud(char *filename)
{
    PACKFILE *f;
    SAMPLE *spl;

    f = pack_fopen(filename, F_READ);
    if (!f) return NULL;
    spl = load_aud_packfile(f);
    pack_fclose(f);
    return spl;
}


/*  save_aud: save a SAMPLE onto disk in .aud format.
 *  returns zero on error, nonzero on success.
 *  note: can only save 16bit samples
 */ 

void save_aud_packfile(PACKFILE *f, SAMPLE *spl)
{
    unsigned long length;
    unsigned long size=0, outsize, num_chunks;
    unsigned short *i;
    int stereo, ch;
    int index[2], step[2], valpred[2], val;
    int delta, sign, diff, vpdiff, store=0, low_nibble; 

    /* len of sample data (bytes) */
    /* outsize = even len * 2 (for 16bit data) * 2 (for stereo data) */
    stereo = spl->stereo ? 1 : 0;
    outsize = ((spl->len + (spl->len % 2)) << 1) << stereo;

    /* size of raw compressed data */
    size = (((spl->len << 1 << stereo) - 1) >> 2) + 1;

    /* number of chunks that data is split into) */
    num_chunks = size / 512 + 1;

    /* each chunk has 8 byte hdr */
    size += num_chunks * 8;

    pack_iputw(spl->freq, f);   /* frequency */
    pack_iputl(size, f);        /* compressed data size (incl/chunk hdrs) */
    pack_iputl(outsize, f);     /* output data size */
    pack_putc(2 | stereo, f);   /* 16bit | possibly stereo */
    pack_putc(99, f);           /* IMA ADPCM */

    index[0] = valpred[0] = 0; 
    index[1] = valpred[1] = 0; 
    step[0] = stepsize_table[index[0]];
    step[1] = stepsize_table[index[1]];
    length = spl->len * (stereo+1);
    low_nibble = TRUE;
    ch = 0;
    size = 0;

    i = spl->data;

    while (length) {
	/* time for another chunk header? */
	if (size <= 0) {
	    if (length/2 + length%2 >= 512)
		size = 512;
	    else
		size = length/2 + length%2;

	    pack_iputw(size, f);        /* size of compressed data */
	    pack_iputw(size*4, f);      /* size of output data */
	    pack_iputl(0x0000deaf, f);  /* magic */
	}

	/* get next sample */
	val = *i++ - 0x8000;
	length--;

	/* find diff between this sample and previous sample */
	/* sign is bit 4 of the output delta */
	diff = val - valpred[ch];
	if (diff < 0) {
	    diff = -diff;
	    sign = 8;
	} 
	else sign=0;

	/* these are the 3 low-order bits of output delta */
	delta = 0;
	vpdiff = (step[ch] >> 3);

	if (diff >= step[ch]) {
	    delta = 4;
	    diff -= step[ch];
	    vpdiff += step[ch];
	}
	step[ch] >>= 1;
	if (diff >= step[ch]) {
	    delta |= 2;
	    diff -= step[ch];
	    vpdiff += step[ch];
	}
	step[ch] >>= 1;
	if (diff >= step[ch]) {
	    delta |= 1;
	    vpdiff += step[ch];
	}

	/* update previous sample */
	if (sign) valpred[ch]-=vpdiff;
	else valpred[ch]+=vpdiff;

	/* clamp to 16 bits */
	if (valpred[ch] > 32767) valpred[ch] = 32767;
	else if (valpred[ch] < -32768) valpred[ch] = -32768; 

	/* update index and step */
	index[ch] += index_table[delta];
	if (index[ch] < 0) index[ch] = 0;
	else if (index[ch] > 88) index[ch] = 88; 
	step[ch] = stepsize_table[index[ch]];

	/* assemble output value */
	delta |= sign;

	/* output, or store for outputing */
	if (low_nibble) {
	    store = delta & 0xf;
	    low_nibble = FALSE;
	} 
	else {
	    pack_putc(store | ((delta<<4) & 0xf0), f);
	    low_nibble = TRUE;
	    /* bring us closer to having to output another chunk header */
	    size--;
	} 

	/* encode channels independantly */
	if (stereo) ch = !ch;
    }

    /* in case of uneven length */
    if (!low_nibble)
	pack_putc(store, f);
}

int save_aud(char *filename, SAMPLE *spl)
{
    PACKFILE *f;

    if (spl->bits != 16)
	return FALSE;

    f = pack_fopen(filename, F_WRITE);
    if (!f) return FALSE;
    save_aud_packfile(f, spl);
    pack_fclose(f);
    return TRUE;
}


/*  load_imaadpcm_wav: load a 4-bit compressed IMA/DVI ADPCM wav into memory
 *  NOTE: WORK IN PROGRESS, see readme.txt
 */

#if TRY_IMAADPCM_WAV
SAMPLE *load_imaadpcm_wav(char *filename)
{
   PACKFILE *f;
   char buffer[25];
   int i;
   int length, len;
   int freq = 22050;
   int bits = 8;
   int channels = 1;
   signed short s;
   SAMPLE *spl = NULL;
   int ima = FALSE, samples_per_block = 0;
   int outsize = 0;

   f = pack_fopen(filename, F_READ);
   if (!f)
      return NULL;

   pack_fread(buffer, 12, f);          /* check RIFF header */
   if (memcmp(buffer, "RIFF", 4) || memcmp(buffer+8, "WAVE", 4))
      goto getout;

   while (!pack_feof(f)) {
      if (pack_fread(buffer, 4, f) != 4)
	 break;

      length = pack_igetl(f);          /* read chunk length */

      if (memcmp(buffer, "fmt ", 4) == 0) {
	 i = pack_igetw(f);            /* should be 1 for PCM data */
				       /* or 0x0011 for DVI/IMA ADPCM data */
	 length -= 2;
	 if (i == 1)
	    ima = FALSE;
	 else if (i == 0x0011)
	    ima = TRUE;
	 else
	    goto getout;

	 channels = pack_igetw(f);     /* mono or stereo data */
	 length -= 2;
	 if ((channels != 1) && (channels != 2))
	    goto getout;

	 freq = pack_igetl(f);         /* sample frequency */
	 length -= 4;

	 pack_igetl(f);                /* skip six bytes */
	 pack_igetw(f);
	 length -= 6;

	 bits = pack_igetw(f);         /* 8 or 16 bit data? */
	 length -= 2;

	 if ((ima && bits != 4) || (!ima && (bits != 8) && (bits != 16)))
	    goto getout;

	 if (ima) {
	    pack_igetw(f);             /* cbSize, should be 2 */
	    samples_per_block = pack_igetw(f);             /* samples per block */
	    length -= 4;
	 }
      }
      else if (memcmp(buffer, "fact", 4) == 0) {
	 outsize = pack_igetl(f);      /* output (decompressed) size */
	 length -= 4;
      }
      else if (memcmp(buffer, "data", 4) == 0) {
	 if (!ima) {                   /* PCM data */
	    len = length / channels;

	    if (bits == 16)
	       len /= 2;

	    spl = create_sample(bits, ((channels == 2) ? TRUE : FALSE), freq, len);

	    if (spl) { 
	       if (bits == 8) {
		  pack_fread(spl->data, length, f);
	       }
	       else {
		  for (i=0; i<len*channels; i++) {
		     s = pack_igetw(f);
		     ((signed short *)spl->data)[i] = s^0x8000;
		  }
	       }

	       length = 0;

	       if (errno) {
		  destroy_sample(spl);
		  spl = NULL;
	       }
	    }
	 }
	 else {                        /* IMA ADPCM data */
	    int index[2], step[2], valpred[2];
	    int ch, delta, vpdiff;
	    int store = 0, low_nibble;
	    signed short *o;

	    len = outsize;

	    spl = create_sample(16, ((channels == 2) ? TRUE : FALSE), freq, len);

	    if (spl) { 
	       o = spl->data;

	       while (length > 0)
	       {
		  /* read in block header */
		  for (ch=0; ch<channels; ch++) 
		  {
		     valpred[ch] = pack_igetw(f);
		     index[ch] = pack_getc(f);
		     step[ch] = stepsize_table[index[ch]];
		     pack_getc(f);        /* reserved */
		     length -= 4;
		  }
		  len = samples_per_block*channels-channels;
		  low_nibble = TRUE;
		  ch = 0;

		  *o++ = valpred[ch]^0x8000; 

		  while (len > 0 && length > 0)
		  {
		     /* get next delta */
		     if (low_nibble)
		     {
			store = pack_getc(f);
			if (store == EOF) {
			   destroy_sample(spl);
			   spl = NULL;
			   goto getout;
			}
			low_nibble = FALSE;
			delta = store & 0xf;
			len--;
			length--;
		     }
		     else
		     {
			low_nibble = TRUE;
			delta = (store>>4) & 0xf;
			len--;
		     }

		     /* calculate the difference */
		     vpdiff = step[ch] >> 3;
		     if (delta & 4) vpdiff += step[ch];
		     if (delta & 2) vpdiff += step[ch] >> 1;
		     if (delta & 1) vpdiff += step[ch] >> 2;

		     /* calculate new sample value */
		     if (delta & 8) valpred[ch] -= vpdiff;
		     else valpred[ch] += vpdiff;

		     /* clamp */
		     if (valpred[ch] > 32767) valpred[ch] = 32767;
		     else if (valpred[ch] < -32768) valpred[ch] = -32768;

		     /* update step table index and value */
		     index[ch] += index_table[delta];
		     if (index[ch] < 0) index[ch] = 0;
		     else if (index[ch] > 88) index[ch] = 88; 
		     step[ch] = stepsize_table[index[ch]]; 

		     /* output */
		     *o++ = valpred[ch]^0x8000;

		     /* channels are decoded independantly */
		     if (spl->stereo) {
			if (ch==0) ch = 1;
			else ch = 0;
		     }
		  }
	       }
	    }
	 }
      } 

      while (length > 0) {             /* skip the remainder of the chunk */
	 if (pack_getc(f) == EOF)
	    break;

	 length--;
      }
   }

   getout:
   pack_fclose(f);
   return spl;
}
#endif


/*  aud_init: registers DAT_AUD as a datafile object.
 */

static void *df_load(PACKFILE *f, long size)
{
    return load_aud_packfile(f);
}

static void df_destroy(void *data)
{
    if (data) destroy_sample(data);
}

void register_aud_file_type(void)
{
    register_datafile_object(DAT_AUD, df_load, df_destroy);
}
