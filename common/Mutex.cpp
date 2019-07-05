#include "Mutex.h"

#ifdef WIN32
CMutex::CMutex()
{
	InitializeCriticalSection(&_cs );
}

CMutex::~CMutex()
{
	DeleteCriticalSection( &_cs );
}

void CMutex::lock()
{
	EnterCriticalSection( &_cs );
}

void CMutex::unlock()
{
	LeaveCriticalSection( &_cs );
}

#else 

CMutex::CMutex()
{
	pthread_mutex_init( &_mutex, NULL );
	pthread_cond_init( &_cv, NULL );
}

CMutex::~CMutex()
{
	pthread_cond_destroy( &_cv );
	pthread_mutex_destroy( &_mutex );
}

void CMutex::lock()
{
	pthread_mutex_lock( &_mutex );
}

void CMutex::unlock()
{
	pthread_mutex_unlock( &_mutex );
}

#endif
