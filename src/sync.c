/* sync.c - client and server thread synchronisation
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include <pthread.h>
#include "sync.h"
#include "yield.h"


static int threaded;
static pthread_t thread;
static pthread_mutex_t mutex;
static pthread_cond_t cond;


void sync_init (void *(*server_thread)(void *))
{
    if (server_thread) {
	threaded = 1;
	pthread_mutex_init (&mutex, NULL);
	pthread_cond_init (&cond, NULL);
	pthread_create (&thread, NULL, server_thread, NULL);
    }
}


void sync_shutdown ()
{
    if (threaded) {
	pthread_join (thread, NULL);
	pthread_cond_destroy (&cond);
	pthread_mutex_destroy (&mutex);
	allegro_errno = &errno;	/* errno is thread-specific */
	threaded = 0;
    }
}


void sync_server_lock ()
{
    if (threaded) {
	pthread_mutex_lock (&mutex);
	pthread_cond_wait (&cond, &mutex);
	allegro_errno = &errno;	/* errno is thread-specific */
    }
}


void sync_server_unlock ()
{
    if (!threaded)
	yield ();
    else
	pthread_mutex_unlock (&mutex);
}


void sync_client_lock ()
{
    if (threaded) {
	pthread_mutex_lock (&mutex);
	allegro_errno = &errno;	/* errno is thread-specific */
    }
}


void sync_client_unlock ()
{
    if (!threaded)
	yield ();
    else {
	pthread_cond_signal (&cond);
	pthread_mutex_unlock (&mutex);
    }
}


