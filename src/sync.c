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
static int server_awaken;


void sync_init (void *(*server_thread)(void *))
{
    if (server_thread) {
	threaded = 1;
	server_awaken = 0;
	pthread_mutex_init (&mutex, NULL);
	pthread_cond_init (&cond, NULL);
	pthread_create (&thread, NULL, server_thread, NULL);
    }
}


void sync_shutdown (void)
{
    if (threaded) {
	sync_client_unlock ();	/* force server to wake */
	pthread_join (thread, NULL);
	pthread_cond_destroy (&cond);
	pthread_mutex_destroy (&mutex);
	allegro_errno = &errno;	/* errno is thread-specific */
	threaded = 0;
    }
}


void sync_server_lock (void)
{
    if (threaded) {
	pthread_mutex_lock (&mutex);
	server_awaken = 0;
	while (!server_awaken)
	    pthread_cond_wait (&cond, &mutex);
	allegro_errno = &errno;	/* errno is thread-specific */
    }
}


int sync_server_stop_requested (void)
{
    return 0;
}


void sync_server_unlock (void)
{
    if (!threaded)
	yield ();
    else
	pthread_mutex_unlock (&mutex);
}


void sync_client_lock (void)
{
    if (threaded) {
	pthread_mutex_lock (&mutex);
	allegro_errno = &errno;	/* errno is thread-specific */
    }
}


void sync_client_unlock (void)
{
    if (threaded) {
	server_awaken = 1;
	pthread_cond_signal (&cond);
	pthread_mutex_unlock (&mutex);
    }
}


#endif



/* Win32 code based on fibres. 
   Doesn't exit properly yet, and I'm not sure it's any better than critical sections.
 */
#if 0


#include <winalleg.h>
#include <process.h>


static int threaded;
static int stop_requested;
static void *server_fiber;
static void *client_fiber;


void sync_init (void *(*server_thread)(void *))
{
    if (server_thread) {
	threaded = 1;
	ConvertThreadToFiber (NULL);
	stop_requested = 0;
	client_fiber = GetCurrentFiber ();
	server_fiber = CreateFiber (1024, (LPFIBER_START_ROUTINE) server_thread, NULL);
    }
}


void sync_shutdown (void)
{
    if (threaded) {
	stop_requested = 1;
	WaitForSingleObject (server_fiber, INFINITE);
// XXX non-proper cleanup
	allegro_errno = &errno;	/* errno is thread-specific */
	threaded = 0;
    }
}


void sync_server_lock (void)
{
    if (threaded) {
	allegro_errno = &errno;	/* errno is thread-specific */
    }
}


void sync_server_unlock (void)
{
    if (!threaded)
	yield ();
    else
	SwitchToFiber (client_fiber);
}


int sync_server_stop_requested (void)
{
    return stop_requested;
}


void sync_client_lock (void)
{
    if (threaded) {
	allegro_errno = &errno;	/* errno is thread-specific */
    }
}


void sync_client_unlock (void)
{
    if (!threaded)
	yield ();
    else
	SwitchToFiber (server_fiber);
}


#endif



/* Win32 code based on critical sections. */
#ifdef THREADS_WIN32


#include <winalleg.h>
#include <process.h>


static int threaded;
static HANDLE thread;
static int stop_requested;
static CRITICAL_SECTION cs;


void sync_init (void *(*server_thread)(void *))
{
    if (server_thread) {
	threaded = 1;
	InitializeCriticalSection (&cs);
	stop_requested = 0;
	thread = (HANDLE) _beginthread ((void (*) (void*)) server_thread, 0, NULL);
    }
}


void sync_shutdown (void)
{
    if (threaded) {
	stop_requested = 1;
	WaitForSingleObject (thread, INFINITE);
	DeleteCriticalSection (&cs);
	allegro_errno = &errno;	/* errno is thread-specific */
	threaded = 0;
    }
}


void sync_server_lock (void)
{
    if (threaded) {
	EnterCriticalSection (&cs);
	allegro_errno = &errno;	/* errno is thread-specific */
    }
}


void sync_server_unlock (void)
{
    if (!threaded)
	yield ();
    else
	LeaveCriticalSection (&cs);
}


int sync_server_stop_requested (void)
{
    return stop_requested;
}


void sync_client_lock (void)
{
    if (threaded) {
	EnterCriticalSection (&cs);
	allegro_errno = &errno;	/* errno is thread-specific */
    }
}


void sync_client_unlock (void)
{
    if (!threaded)
	yield ();
    else
	LeaveCriticalSection (&cs);
}


#endif



/* Win32 code based on events. */
#if 0


#include <winalleg.h>
#include <process.h>


static int threaded;
static HANDLE thread;
static int stop_requested;
static HANDLE server_event;
static HANDLE client_event;


void sync_init (void *(*server_thread)(void *))
{
    if (server_thread) {
	threaded = 1;
	server_event = CreateEvent (NULL, FALSE, FALSE, NULL);
	client_event = CreateEvent (NULL, FALSE, FALSE, NULL);	
	stop_requested = 0;
	thread = (HANDLE) _beginthread ((void (*) (void*)) server_thread, 0, NULL);
    }
}


void sync_shutdown (void)
{
    if (threaded) {
	stop_requested = 1;
	WaitForSingleObject (thread, INFINITE);
	CloseHandle (server_event);
	CloseHandle (client_event);
	allegro_errno = &errno;	/* errno is thread-specific */
	threaded = 0;
    }
}


void sync_server_lock (void)
{
    if (threaded) {
	SignalObjectAndWait (client_event, server_event, INFINITE, FALSE);
	allegro_errno = &errno;	/* errno is thread-specific */
    }
}


void sync_server_unlock (void)
{
    if (!threaded)
	yield ();
}


int sync_server_stop_requested (void)
{
    return stop_requested;
}


void sync_client_lock (void)
{
    if (threaded) {
	allegro_errno = &errno;	/* errno is thread-specific */
    }
}


void sync_client_unlock (void)
{
    if (!threaded)
	yield ();
    else
	SignalObjectAndWait (server_event, client_event, INFINITE, FALSE);
}


#endif



/* Win32 code based on semaphores.  */
#if 0


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


void sync_shutdown (void)
{
    if (threaded) {
	stop_requested = 1;
	WaitForSingleObject (thread, INFINITE);
	CloseHandle (semaphore);
	allegro_errno = &errno;	/* errno is thread-specific */
	threaded = 0;
    }
}


void sync_server_lock (void)
{
    if (threaded) {
	WaitForSingleObject (semaphore, INFINITE);
	allegro_errno = &errno;	/* errno is thread-specific */
    }
}


void sync_server_unlock (void)
{
    if (!threaded)
	yield ();
    else
	ReleaseSemaphore (semaphore, 1, NULL);
}


int sync_server_stop_requested (void)
{
    return stop_requested;
}


void sync_client_lock (void)
{
    if (threaded) {
	WaitForSingleObject (semaphore, INFINITE);
	allegro_errno = &errno;	/* errno is thread-specific */
    }
}


void sync_client_unlock (void)
{
    if (!threaded)
	yield ();
    else
	ReleaseSemaphore (semaphore, 1, NULL);
}


#endif
