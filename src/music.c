/* music.c
 * 
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <ctype.h>
#include <pthread.h>
#include <string.h>
#include <allegro.h>
#include "aldumb.h"
#include "alloc.h"
#include "music.h"
#include "ral.h"


struct playlist_entry
{
    char *name;
    struct playlist_entry *next;
};


static int playlist_length;
static struct playlist_entry *playlist;


static pthread_t the_thread;
static int thread_started;


float music_desired_volume = 1.0; /* 0.0 to 1.0 */



/*
 * Various helpers
 */


static void strip_leading_trailing_whitespace (char *s)
{
    char *x;

    if (!*s) return;

    x = s + strlen (s) - 1;
    while (isspace (*x)) x--;
    *(x+1) = '\0';

    for (x = s; isspace (*x); x++);
    memmove (s, x, strlen (x) + 1);
}


static void my_rest (int msecs)
{
#ifdef ALLEGRO_UNIX
    
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = msecs * 1000;
    select(0, 0, 0, 0, &tv);

#else
#ifdef ALLEGRO_WINDOWS
    
    /* This seems to give better results under Windows that Sleep(0)
     * (which is what yield_timeslice() uses).  */

    void __stdcall Sleep (int);
    Sleep (msecs);

#else

#error my_rest unimplemented

#endif
#endif
}


/* Returns non-zero if S1 is a suffix of S2.  The comparison is
 * case-insensitive.
 */
static int is_suffix_ci(const char *s1, const char *s2)
{
    int n1 = strlen(s1);
    int n2 = strlen(s2);

    return (n1 <= n2) && (stricmp(s2 + (n2 - n1), s1) == 0);
}


/* Load a music module, using the filename extension to determine the
 * file type.
 */
static DUH *my_load_duh (const char *filename)
{
    DUH *(*loader)(const char *filename);

    if (is_suffix_ci (".it", filename))
	loader = dumb_load_it;
    else if (is_suffix_ci (".xm", filename))
	loader = dumb_load_xm;
    else if (is_suffix_ci (".s3m", filename))
	loader = dumb_load_s3m;
    else if (is_suffix_ci (".mod", filename))
	loader = dumb_load_mod;
    else
	return NULL;

    return loader (filename);
}


/* Force a DUH player not to loop the module. */
static void force_no_loop (AL_DUH_PLAYER *dp)
{
    DUH_SIGRENDERER *sr = al_duh_get_sigrenderer (dp);
    DUMB_IT_SIGRENDERER *itsr = duh_get_it_sigrenderer (sr);
    dumb_it_set_loop_callback (itsr, &dumb_it_callback_terminate, NULL);
    dumb_it_set_xm_speed_zero_callback
	(itsr, &dumb_it_callback_terminate, NULL);
}



/*
 * The module proper
 */


/* [main thread] */
static void load_playlist (const char *filename)
{
    FILE *fp;
    unsigned char *line;
    struct playlist_entry *pe;

    fp = fopen (filename, "r");
    if (!fp)
	return;

    playlist_length = 0;
    playlist = NULL;

    while ((line = read_a_line (fp, RAL_TRIM, NULL, NULL))) {

	strip_leading_trailing_whitespace (line);
	if (!*line) {
	    free (line);
	    continue;
	}

	pe = alloc (sizeof *pe);
	pe->name = line;
	pe->next = playlist;
	playlist = pe;
	playlist_length++;
    }

    fclose (fp);
}


/* [main thread] */
static void free_playlist (void)
{
    while (playlist) {
	struct playlist_entry *next = playlist->next;
	free (playlist->name);
	free (playlist);
	playlist = next;
    }

    playlist_length = 0;    
}


/* [DUMB poller thread] */
const char *random_playlist_filename (void)
{
    struct playlist_entry *pe;
    int n;

    n = rand() % playlist_length;
    for (pe = playlist; n != 0; pe = pe->next)
	n--;

    return pe->name;
}


/* This is the DUMB poller thread. */
static void *player_thread_func (void *unused)
{
    const int freq = 22050;
    const int bufsize = 1024;
    const int n = (freq / bufsize) + 15;
    const int ms = 1000 / n;

    DUH *duh = NULL;
    AL_DUH_PLAYER *dp = NULL;
    const char *last_song = NULL;
    const char *song;
    float actual_volume = music_desired_volume;

    (void)unused;

    while (thread_started) {

	if (duh) {
	    if (al_poll_duh (dp) != 0) {
		al_stop_duh (dp);
		unload_duh (duh);
		duh = NULL;
	    }

	    if (music_desired_volume != actual_volume) {
		actual_volume = music_desired_volume;
		al_duh_set_volume (dp, actual_volume);
	    }
	}

	if (!duh) {
	    if (playlist_length == 1)
		song = playlist[0].name;
	    else
		do {
		    song = random_playlist_filename ();
		} while (song == last_song);

	    last_song = song;

	    duh = my_load_duh (song);
	    if (duh) {
		actual_volume = music_desired_volume;
		dp = al_start_duh (duh, 2, 0, actual_volume, bufsize, freq);
		force_no_loop (dp);
	    }
	}

	my_rest (ms);
    }

    al_stop_duh (dp);
    unload_duh (duh);

    return NULL;
}


void music_init (void)
{
    dumb_register_stdfiles ();
}


void music_select_playlist (const char *filename)
{
    music_stop_playlist ();

    load_playlist (filename);
    if (playlist) {
	thread_started = 1;
	pthread_create (&the_thread, NULL, player_thread_func, NULL);
    }
}


void music_stop_playlist (void)
{
    if (thread_started) {
	thread_started = 0;
	pthread_join (the_thread, NULL);

	free_playlist ();
    }
}


void music_shutdown (void)
{
    music_stop_playlist ();
    dumb_exit ();
}
