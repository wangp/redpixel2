/* This file is part of the loadaud package. See README for details. */

#ifndef __LOADAUD_H
#define __LOADAUD_H

#include <allegro.h>


#ifdef __cplusplus
extern "C" {
#endif


#define LOADAUD_VERSION          0
#define LOADAUD_SUB_VERSION      90


#define DAT_AUD DAT_ID('A','U','D',' ')


SAMPLE *load_aud_packfile(PACKFILE *f);
SAMPLE *load_aud(char *filename);

void save_aud_packfile(PACKFILE *f, SAMPLE *spl);
int save_aud(char *filename, SAMPLE *spl);

void register_aud_file_type(void);



/*  Does not work yet, but if you still want to try it (or fix it =)
 *  change the #define to 1.
 */ 

#define TRY_IMAADPCM_WAV    0

#if TRY_IMAADPCM_WAV 
SAMPLE *load_imaadpcm_wav(char *filename);
#endif


#ifdef __cplusplus
}
#endif
#endif