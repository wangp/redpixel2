
#include <pthread.h>

extern pthread_mutex_t mutex;

#define lua_lock(L)	pthread_mutex_lock(*(pthread_mutex_t **)L)
#define lua_unlock(L)	pthread_mutex_unlock(*(pthread_mutex_t **)L)


void openthreads (lua_State *L);


#define LUA_USERSTATE		pthread_mutex_t *mutex;

#define LUA_USERINIT(L)		(\
	*(pthread_mutex_t **)L = &mutex, \
	openstdlibs(L), openthreads(L))

