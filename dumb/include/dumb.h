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
 * dumb.h - The user header file for DUMB.          / / \  \
 *                                                 | <  /   \_
 * Include this file in any of your files in       |  \/ /\   /
 * which you wish to use the DUMB functions         \_  /  > /
 * and variables.                                     | \ / /
 *                                                    |  ' /
 *                                                     \__/
 */

#ifndef DUMB_H
#define DUMB_H


#include <stdlib.h>
#include <stdio.h>


#ifdef __cplusplus
	extern "C" {
#endif


#define DUMB_MAJOR_VERSION    0
#define DUMB_MINOR_VERSION    9
#define DUMB_REVISION_VERSION 1

#define DUMB_VERSION (DUMB_MAJOR_VERSION*10000 + DUMB_MINOR_VERSION*100 + DUMB_REVISION_VERSION)

#define DUMB_VERSION_STR "0.9.1"

#define DUMB_NAME "DUMB v"DUMB_VERSION_STR

#define DUMB_YEAR  2002
#define DUMB_MONTH 12
#define DUMB_DAY   19

#define DUMB_YEAR_STR2  "02"
#define DUMB_YEAR_STR4  "2002"
#define DUMB_MONTH_STR1 "12"
#define DUMB_DAY_STR1   "19"

#if DUMB_MONTH < 10
#define DUMB_MONTH_STR2 "0"DUMB_MONTH_STR1
#else
#define DUMB_MONTH_STR2 DUMB_MONTH_STR1
#endif

#if DUMB_DAY < 10
#define DUMB_DAY_STR2 "0"DUMB_DAY_STR1
#else
#define DUMB_DAY_STR2 DUMB_DAY_STR1
#endif


/* WARNING: The month and day were inadvertently swapped in the v0.8 release.
 *          Please do not compare this constant against any date in 2002. In
 *          any case, DUMB_VERSION is probably more useful for this purpose.
 */
#define DUMB_DATE (DUMB_YEAR*10000 + DUMB_MONTH*100 + DUMB_DAY)

#define DUMB_DATE_STR DUMB_DAY_STR1"."DUMB_MONTH_STR1"."DUMB_YEAR_STR4


#undef MIN
#undef MAX
#undef MID

#define MIN(x,y)   (((x) < (y)) ? (x) : (y))
#define MAX(x,y)   (((x) > (y)) ? (x) : (y))
#define MID(x,y,z) MAX((x), MIN((y), (z)))

#undef ABS
#define ABS(x) (((x) >= 0) ? (x) : (-(x)))


/** WARNING - TEMPORARY HACK */
#ifdef DEBUGMODE

#ifndef ASSERT
#include <assert.h>
#define ASSERT(n) assert(n)
#endif
#ifndef TRACE
#define TRACE 1 ? (void)0 : printf
#endif

#else

#ifndef ASSERT
#define ASSERT(n)
#endif
#ifndef TRACE
#define TRACE 1 ? (void)0 : printf
#endif

#endif


#define DUMB_ID(a,b,c,d) (((unsigned int)(a) << 24) | \
                          ((unsigned int)(b) << 16) | \
                          ((unsigned int)(c) <<  8) | \
                          ((unsigned int)(d)      ))



/** WARNING - THE FOLLOWING DEFINITIONS ARE NOT PORTABLE */

#ifndef LONG_LONG
#ifdef __GNUC__
#define LONG_LONG long long
#elif defined _MSC_VER
#define LONG_LONG __int64
#endif
#endif

#if __GNUC__ >= 3
#define DUMB_DECLARE_DEPRECATED
#define DUMB_DEPRECATED __attribute__((__deprecated__))
#else
#define DUMB_DEPRECATED
#endif


/* Basic Sample Type */

typedef int sample_t;


/* Library Clean-up Management */

int dumb_atexit(void (*proc)(void));

void dumb_exit(void);


/* File Input Functions */

typedef struct DUMBFILE_SYSTEM
{
	void *(*open)(const char *filename);
	int (*skip)(void *f, long n);
	int (*getc)(void *f);
	long (*getnc)(char *ptr, long n, void *f);
	void (*close)(void *f);
}
DUMBFILE_SYSTEM;

typedef struct DUMBFILE DUMBFILE;

void register_dumbfile_system(DUMBFILE_SYSTEM *dfs);

DUMBFILE *dumbfile_open(const char *filename);
DUMBFILE *dumbfile_open_ex(void *file, DUMBFILE_SYSTEM *dfs);

long dumbfile_pos(DUMBFILE *f);
int dumbfile_skip(DUMBFILE *f, long n);

int dumbfile_getc(DUMBFILE *f);

int dumbfile_igetw(DUMBFILE *f);
int dumbfile_mgetw(DUMBFILE *f);

long dumbfile_igetl(DUMBFILE *f);
long dumbfile_mgetl(DUMBFILE *f);

unsigned long dumbfile_cgetul(DUMBFILE *f);
signed long dumbfile_cgetsl(DUMBFILE *f);

long dumbfile_getnc(char *ptr, long n, DUMBFILE *f);

int dumbfile_error(DUMBFILE *f);
int dumbfile_close(DUMBFILE *f);


/* stdio File Input Module */

void dumb_register_stdfiles(void);

DUMBFILE *dumbfile_open_stdfile(FILE *p);


/* Memory File Input Module */

DUMBFILE *dumbfile_open_memory(const char *data, long size);


/* DUH Management */

typedef struct DUH DUH;

#define DUH_SIGNATURE DUMB_ID('D','U','H','!')

void unload_duh(DUH *duh);

DUH *load_duh(const char *filename);
DUH *read_duh(DUMBFILE *f);

long duh_get_length(DUH *duh);


/* Signal Rendering Functions */

typedef struct DUH_SIGRENDERER DUH_SIGRENDERER;

DUH_SIGRENDERER *duh_start_sigrenderer(DUH *duh, int sig, int n_channels, long pos);

typedef void (*DUH_SIGRENDERER_CALLBACK)(void *data, sample_t **samples, int n_channels, long length);

void duh_sigrenderer_set_callback(
	DUH_SIGRENDERER *sigrenderer,
	DUH_SIGRENDERER_CALLBACK callback, void *data
);

int duh_sigrenderer_get_n_channels(DUH_SIGRENDERER *sigrenderer);
long duh_sigrenderer_get_position(DUH_SIGRENDERER *sigrenderer);

void duh_set_sigparam(DUH_SIGRENDERER *sigrenderer, unsigned char id, long value);

long duh_render_signal(
	DUH_SIGRENDERER *sigrenderer,
	float volume, float delta,
	long size, sample_t **samples
);

void duh_end_sigrenderer(DUH_SIGRENDERER *sigrenderer);


/* DUH Rendering Functions */

long duh_render(
	DUH_SIGRENDERER *sigrenderer,
	int bits, int unsign,
	float volume, float delta,
	long size, void *sptr
);

#ifdef DUMB_DECLARE_DEPRECATED

typedef DUH_SIGRENDERER DUH_RENDERER DUMB_DEPRECATED;
/* Please use DUH_SIGRENDERER instead of DUH_RENDERER. */

DUH_SIGRENDERER *duh_start_renderer(DUH *duh, int n_channels, long pos) DUMB_DEPRECATED;
/* Please use duh_start_sigrenderer() instead. Pass 0 for 'sig'. */

int duh_renderer_get_n_channels(DUH_SIGRENDERER *dr) DUMB_DEPRECATED;
long duh_renderer_get_position(DUH_SIGRENDERER *dr) DUMB_DEPRECATED;
/* Please use the duh_sigrenderer_*() equivalents of these two functions. */

void duh_end_renderer(DUH_SIGRENDERER *dr) DUMB_DEPRECATED;
/* Please use duh_end_sigrenderer() instead. */

DUH_SIGRENDERER *duh_renderer_encapsulate_sigrenderer(DUH_SIGRENDERER *sigrenderer) DUMB_DEPRECATED;
DUH_SIGRENDERER *duh_renderer_get_sigrenderer(DUH_SIGRENDERER *dr) DUMB_DEPRECATED;
DUH_SIGRENDERER *duh_renderer_decompose_to_sigrenderer(DUH_SIGRENDERER *dr) DUMB_DEPRECATED;
/* These functions have become no-ops that just return the parameter.
 * So, for instance, replace
 *   duh_renderer_encapsulate_sigrenderer(my_sigrenderer)
 * with
 *   my_sigrenderer
 */

#endif


/* Impulse Tracker Support */

extern int dumb_it_max_to_mix;

typedef struct DUMB_IT_SIGDATA DUMB_IT_SIGDATA;
typedef struct DUMB_IT_SIGRENDERER DUMB_IT_SIGRENDERER;

DUMB_IT_SIGDATA *duh_get_it_sigdata(DUH *duh);
DUH_SIGRENDERER *duh_encapsulate_it_sigrenderer(DUMB_IT_SIGRENDERER *it_sigrenderer, int n_channels, long pos);
DUMB_IT_SIGRENDERER *duh_get_it_sigrenderer(DUH_SIGRENDERER *sigrenderer);

DUH_SIGRENDERER *dumb_it_start_at_order(DUH *duh, int n_channels, int startorder);

void dumb_it_set_loop_callback(DUMB_IT_SIGRENDERER *sigrenderer, int (*callback)(void *data), void *data);
void dumb_it_set_xm_speed_zero_callback(DUMB_IT_SIGRENDERER *sigrenderer, int (*callback)(void *data), void *data);
void dumb_it_set_midi_callback(DUMB_IT_SIGRENDERER *sigrenderer, int (*callback)(void *data, int channel, unsigned char byte), void *data);

int dumb_it_callback_terminate(void *data);
int dumb_it_callback_midi_block(void *data, int channel, unsigned char byte);

DUH *dumb_load_it(const char *filename);
DUH *dumb_load_xm(const char *filename);
DUH *dumb_load_s3m(const char *filename);
DUH *dumb_load_mod(const char *filename);

DUH *dumb_read_it(DUMBFILE *f);
DUH *dumb_read_xm(DUMBFILE *f);
DUH *dumb_read_s3m(DUMBFILE *f);
DUH *dumb_read_mod(DUMBFILE *f);

int dumb_it_sd_get_n_orders(DUMB_IT_SIGDATA *sd);

int dumb_it_sd_get_initial_global_volume(DUMB_IT_SIGDATA *sd);
void dumb_it_sd_set_initial_global_volume(DUMB_IT_SIGDATA *sd, int gv);

int dumb_it_sd_get_mixing_volume(DUMB_IT_SIGDATA *sd);
void dumb_it_sd_set_mixing_volume(DUMB_IT_SIGDATA *sd, int mv);

int dumb_it_sd_get_initial_speed(DUMB_IT_SIGDATA *sd);
void dumb_it_sd_set_initial_speed(DUMB_IT_SIGDATA *sd, int speed);

int dumb_it_sd_get_initial_tempo(DUMB_IT_SIGDATA *sd);
void dumb_it_sd_set_initial_tempo(DUMB_IT_SIGDATA *sd, int tempo);

int dumb_it_sd_get_initial_channel_volume(DUMB_IT_SIGDATA *sd, int channel);
void dumb_it_sd_set_initial_channel_volume(DUMB_IT_SIGDATA *sd, int channel, int volume);

int dumb_it_sr_get_current_order(DUMB_IT_SIGRENDERER *sr);
int dumb_it_sr_get_current_row(DUMB_IT_SIGRENDERER *sr);

int dumb_it_sr_get_global_volume(DUMB_IT_SIGRENDERER *sr);
void dumb_it_sr_set_global_volume(DUMB_IT_SIGRENDERER *sr, int gv);

int dumb_it_sr_get_tempo(DUMB_IT_SIGRENDERER *sr);
void dumb_it_sr_set_tempo(DUMB_IT_SIGRENDERER *sr, int tempo);

int dumb_it_sr_get_speed(DUMB_IT_SIGRENDERER *sr);
void dumb_it_sr_set_speed(DUMB_IT_SIGRENDERER *sr, int speed);

#define DUMB_IT_N_CHANNELS 64
#define DUMB_IT_N_NNA_CHANNELS 192
#define DUMB_IT_TOTAL_CHANNELS (DUMB_IT_N_CHANNELS + DUMB_IT_N_NNA_CHANNELS)

/* Channels passed to any of these functions are 0-based */
int dumb_it_sr_get_channel_volume(DUMB_IT_SIGRENDERER *sr, int channel);
void dumb_it_sr_set_channel_volume(DUMB_IT_SIGRENDERER *sr, int channel, int volume);

typedef struct DUMB_IT_CHANNEL_STATE DUMB_IT_CHANNEL_STATE;

struct DUMB_IT_CHANNEL_STATE
{
	int channel; /* 0-based; meaningful for NNA channels */
	int sample; /* 1-based; 0 if nothing playing, then other fields undef */
	int freq; /* in Hz */
	float volume; /* 1.0 maximum; affected by ALL factors, inc. mixing vol */
	unsigned char pan; /* 0-64, 100 for surround */
	unsigned char filter_cutoff;    /* 0-127    cutoff=127 AND resonance=0 */
	unsigned char filter_resonance; /* 0-127    -> no filters              */
};

/* Values of 64 or more will access NNA channels here. */
void dumb_it_sr_get_channel_state(DUMB_IT_SIGRENDERER *sr, int channel, DUMB_IT_CHANNEL_STATE *state);


/* Signal Design Helper Values */

/* Use pow(DUMB_SEMITONE_BASE, n) to get the 'delta' value to transpose up by
 * n semitones. To transpose down, use negative n.
 */
#define DUMB_SEMITONE_BASE 1.059463094359295309843105314939748495817

/* Use pow(DUMB_QUARTERTONE_BASE, n) to get the 'delta' value to transpose up
 * by n quartertones. To transpose down, use negative n.
 */
#define DUMB_QUARTERTONE_BASE 1.029302236643492074463779317738953977823

/* Use pow(DUMB_PITCH_BASE, n) to get the 'delta' value to transpose up by n
 * units. In this case, 256 units represent one semitone; 3072 units
 * represent one octave. These units are used by the sequence signal (SEQU).
 */
#define DUMB_PITCH_BASE 1.000225659305069791926712241547647863626


/* Signal Design Function Types */

typedef void sigdata_t;
typedef void sigrenderer_t;

typedef sigdata_t *(*DUH_LOAD_SIGDATA)(DUH *duh, DUMBFILE *file);

typedef sigrenderer_t *(*DUH_START_SIGRENDERER)(
	DUH *duh,
	sigdata_t *sigdata,
	int n_channels,
	long pos
);

typedef void (*DUH_SET_SIGPARAM)(
	sigrenderer_t *sigrenderer,
	unsigned char id, long value
);

typedef long (*DUH_RENDER_SIGNAL)(
	sigrenderer_t *sigrenderer,
	float volume, float delta,
	long size, sample_t **samples
);

typedef void (*DUH_END_SIGRENDERER)(sigrenderer_t *sigrenderer);

typedef void (*DUH_UNLOAD_SIGDATA)(sigdata_t *sigdata);


/* Signal Design Function Registration */

typedef struct DUH_SIGTYPE_DESC
{
	long type;
	DUH_LOAD_SIGDATA      load_sigdata;
	DUH_START_SIGRENDERER start_sigrenderer;
	DUH_SET_SIGPARAM      set_sigparam;
	DUH_RENDER_SIGNAL     render_signal;
	DUH_END_SIGRENDERER   end_sigrenderer;
	DUH_UNLOAD_SIGDATA    unload_sigdata;
}
DUH_SIGTYPE_DESC;

void dumb_register_sigtype(DUH_SIGTYPE_DESC *desc);


// Decide where to put these functions; new heading?

sigdata_t *duh_get_raw_sigdata(DUH *duh, int sig, long type);

DUH_SIGRENDERER *duh_encapsulate_raw_sigrenderer(sigrenderer_t *vsigrenderer, DUH_SIGTYPE_DESC *desc, int n_channels, long pos);
sigrenderer_t *duh_get_raw_sigrenderer(DUH_SIGRENDERER *sigrenderer, long type);


/* Sample Buffer Allocation Helpers */

sample_t **create_sample_buffer(int n_channels, long length);
void destroy_sample_buffer(sample_t **samples);


/* Silencing Helper */

void dumb_silence(sample_t *samples, long length);


/* Click Removal Helpers */

typedef struct DUMB_CLICK_REMOVER DUMB_CLICK_REMOVER;

DUMB_CLICK_REMOVER *dumb_create_click_remover(void);
void dumb_record_click(DUMB_CLICK_REMOVER *cr, long pos, sample_t step);
void dumb_remove_clicks(DUMB_CLICK_REMOVER *cr, sample_t *samples, long length, float halflife);
void dumb_destroy_click_remover(DUMB_CLICK_REMOVER *cr);

DUMB_CLICK_REMOVER **dumb_create_click_remover_array(int n);
void dumb_remove_clicks_array(int n, DUMB_CLICK_REMOVER **cr, sample_t **samples, long length, float halflife);
void dumb_destroy_click_remover_array(int n, DUMB_CLICK_REMOVER **cr);


/* Resampling Helpers */

/* DUMB_EXTRA_SAMPLES is the maximum number of extra samples that might be
 * required after the end by the resampling algorithms. For a looping sample,
 * the extra samples should be set the same as the samples at the loop-back
 * point. For a non-looping sample, they should be set to zero.
 *
 * Note that the resampling algorithms will probably be improved at some
 * point, so the extra samples are not necessary; this is why dumb_resample()
 * has been deemed not ready for use yet (see the 'Resampling Helpers'
 * section in docs/dumb.txt).
 */
#define DUMB_EXTRA_SAMPLES 3

extern int dumb_resampling_quality;

typedef int (*DUMB_RESAMPLE_PICKUP)(
	sample_t *src, long *_src_pos, int *_src_subpos,
	long *_src_start, long *_src_end,
	int dir,
	void *data
);

sample_t dumb_resample_get_value(
	sample_t *src, long *_src_pos, int *_src_subpos,
	long src_start, long src_end,
	float volume, int *_dir,
	DUMB_RESAMPLE_PICKUP pickup, void *pickup_data
);

long dumb_resample(
	sample_t *src, long *_src_pos, int *_src_subpos,
	long src_start, long src_end,
	sample_t *dst, long dst_size,
	float volume, float delta, int *_dir,
	DUMB_RESAMPLE_PICKUP pickup, void *pickup_data
);


/* DUH Construction */

DUH *make_duh(
	long length,
	int n_signals,
	DUH_SIGTYPE_DESC *desc[],
	sigdata_t *sigdata[]
);


#ifdef __cplusplus
	}
#endif


#endif /* DUMB_H */

