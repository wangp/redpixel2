/* sync.c - client and server thread synchronisation
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "sync.h"
#include "yield.h"


#ifdef THREADS_PTHREAD


#include <pthread.h>


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


int sync_server_stop_requested ()
{
    return 0;
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


#endif


#ifdef THREADS_WIN32


#include <winalleg.h>
#include <process.h>


static int threaded;
static HANDLE thread;
static int stop_requested;
static HANDLE semaphore;


#define SEM_VALUE_MAX   ((int) ((~0u) >> 1))


void sync_init (void *(*server_thread)(void *))
{
    if (server_thread) {
	threaded = 1;
	semaphore = CreateSemaphore (NULL, 1, 1, NULL);
	stop_requested = 0;
	thread = (HANDLE) _beginthread ((void (*) (void*)) server_thread, 0, NULL);
    }
}


void sync_shutdown ()
{
    if (threaded) {
	stop_requested = 1;
	WaitForSingleObject (thread, INFINITE);
	CloseHandle (semaphore);
	allegro_errno = &errno;	/* errno is thread-specific */
	threaded = 0;
    }
}


void sync_server_lock ()
{
    if (threaded) {
	WaitForSingleObject (semaphore, INFINITE);
	allegro_errno = &errno;	/* errno is thread-specific */
    }
}


void sync_server_unlock ()
{
    if (!threaded)
	yield ();
    else
	ReleaseSemaphore (semaphore, 1, NULL);
}


int sync_server_stop_requested ()
{
    return stop_requested;
}


void sync_client_lock ()
{
    if (threaded) {
	WaitForSingleObject (semaphore, INFINITE);
	allegro_errno = &errno;	/* errno is thread-specific */
    }
}


void sync_client_unlock ()
{
    if (!threaded)
	yield ();
    else
	ReleaseSemaphore (semaphore, 1, NULL);
}


#endif
