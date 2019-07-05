#ifdef WIN32
#pragma warning( disable : 4786)
#endif

#include "Thread.h"

CThread::CThread() :
 _isStopping(false),
 _isActive(false),
 _priority(0),
#ifdef WIN32
 _handle(0),
#endif
 _threadId(0)		// dual type and role for WIN and UNIX
{
}

CThread::~CThread()
{
	// Stop followed by wait should be called  before destructor
#ifdef WIN32
	assert( _handle == 0 );
#else
	assert( _threadId == 0 );
#endif
}

#ifdef WIN32
unsigned __stdcall CThread::runThread( void* arg )
{
	((CThread*)arg)->run();
	return 0;
}
#else
extern "C" 
{
	void* runThreadExternC( void* arg )
	{
		return CThread::runThread( arg );
	}
}
void* CThread::runThread( void* arg )
{
	((CThread*)arg)->run();
	return 0;
}
#endif

void CThread::start()
{
	_isStopping = false;

#ifdef WIN32
	_handle = (HANDLE)_beginthreadex( NULL, 0, runThread, this, 0, &_threadId );
	assert( _handle != 0 );

	SetThreadPriority( _handle, _priority );
#else
	int ret = pthread_create( &_threadId, NULL, runThreadExternC, this );
	assert( ret == 0 );
#endif

	_isActive = true;
}

void CThread::stop()
{
	_isStopping = true;
}

void CThread::wait()
{
	if ( _isActive )
	{
		_isActive = false;
#ifdef WIN32
		WaitForSingleObject( _handle, INFINITE );
		CloseHandle( _handle );
		_handle = 0;
		_threadId = 0;
#else
		pthread_join( _threadId, NULL );
		_threadId = 0;
#endif
	}
}

bool CThread::isStopping()
{
	return _isStopping;
}

bool CThread::setPriority( int pri )
{
	// on Win32, you need the _threadId before you can set the
	// thread priority; _threadId is returned after the thread is
	// created; in our usage, we set the priority, before the thread
	// is created
	_priority = pri;
#ifndef WIN32
  	pthread_attr_init( &m_attr );
	m_param.sched_priority = _priority;
	pthread_attr_setschedparam( &m_attr, &m_param );
#endif

	return false;
}

void CThread::Sleep( unsigned long millisecs )
{
#if defined (_WIN32)
    ::Sleep(millisecs);
#else
    struct timespec sleeptime;
    sleeptime.tv_sec = millisecs / 1000;
    sleeptime.tv_nsec = (millisecs % 1000) * 1000000;
    nanosleep(&sleeptime,0);
#endif
}
