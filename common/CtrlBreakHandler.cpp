#ifdef WIN32
#pragma warning( disable : 4786)
#endif

#include "CtrlBreakHandler.h"

bool CtrlBreakHandler::_isTerminated = false;
bool CtrlBreakHandler::_isInitialized = false;
rfa::common::RFA_Vector<CtrlBreakHandler::CALLBACK_PTR> CtrlBreakHandler::_callbacks;

#ifndef WIN32
	#include <time.h>
	#define MILLISEC	1000
	struct sigaction 	CtrlBreakHandler::_sigAction;
	struct sigaction 	CtrlBreakHandler::_oldSigAction;
#endif

#ifdef WIN32
BOOL WINAPI CtrlBreakHandler::TermHandlerRoutine( DWORD dwCtrlType )
{
	switch(dwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_SHUTDOWN_EVENT:
	case CTRL_C_EVENT:
		_isTerminated = true;
		return TRUE;
	}
	return FALSE;
}
#else
extern "C" {
	void sigActionExternC( int sig, siginfo_t* pSiginfo, void* pv )
	{
		CtrlBreakHandler::sigAction( sig, pSiginfo, pv );
	}
}
void CtrlBreakHandler::sigAction( int sig, siginfo_t* pSiginfo, void* pv )
{
	_isTerminated = true;
}
#endif 

void CtrlBreakHandler::init( CALLBACK_PTR callback )
{
	// Add the callback function pointer to the vector 
	// only if the vector does not currently contain it
	// and register the callback with the operating system.
	if(_callbacks.getPositionOf(callback) == -1)
	{
#ifdef WIN32
		_callbacks.push_back(callback);
		SetConsoleCtrlHandler(callback, TRUE);
#else
		_callbacks.push_back(callback);
		bzero( &_sigAction, sizeof( _sigAction ) );
		bzero( &_oldSigAction, sizeof( _oldSigAction ) );
		_sigAction.sa_sigaction = &sigActionExternC;
		_sigAction.sa_flags 	= SA_SIGINFO;
		sigaction( SIGINT, &_sigAction, &_oldSigAction );
#endif
	}
	if ( _isInitialized )
		return;

	_isInitialized = true;
	_isTerminated = false;
}

void CtrlBreakHandler::exit()
{
	if ( !_isInitialized )
		return;

	// Unregister each callback function that the vector contains
	for(unsigned int i=0; i<_callbacks.size(); i++)
	{
#ifdef WIN32
		SetConsoleCtrlHandler( _callbacks[i], FALSE );
#else
		sigaction( SIGINT, &_oldSigAction, NULL );
#endif
	}
	_isInitialized = false;
	_isTerminated = true;
}

void CtrlBreakHandler::forceExit()
{
	if ( !_isInitialized )
		return;

#ifdef WIN32
	// send CTRL_C_EVENT to all processes that share the console
	// of this process. For more info please see description
	// of the GenerateConsoleCtrlEvent()
	BOOL retCode = GenerateConsoleCtrlEvent( CTRL_C_EVENT, 0 );
	if ( retCode == 0 )
	{
		DWORD retError = GetLastError();
	}
#else
	// send SIGINT to the current process only!
	kill( getpid(), SIGINT );
#endif
}

void CtrlBreakHandler::sleep( long msec )
{
#ifdef WIN32
	Sleep( msec );
#else
	struct timespec sleeptime;
	sleeptime.tv_sec = msec / MILLISEC;
	sleeptime.tv_nsec = (msec % MILLISEC) * 1000000;
	nanosleep(&sleeptime,0);
#endif
}

bool CtrlBreakHandler::isTerminated()
{
	return _isTerminated;
}
