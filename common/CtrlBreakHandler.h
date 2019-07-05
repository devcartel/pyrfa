#ifndef __SESSION_LAYER_THREADED_EXAMPLE__CTRL_BREAK_HANDLER__H__
#define __SESSION_LAYER_THREADED_EXAMPLE__CTRL_BREAK_HANDLER__H__

#ifdef WIN32
	#include <windows.h>
#else
    #include <unistd.h>
	#include <signal.h>
	#include <strings.h>
#endif

#include "Common/RFA_Vector.h"

// This class registers function pointers for callback from the operating system
// to handle "control break", "control c", etc... events/sginals from the keyboard.
// By default our own callback function pointer is registered
// otherwise any suitable callback function pointer is registered.

class CtrlBreakHandler
{
public:
#ifdef WIN32
	typedef BOOL (WINAPI *WIN_CALLBACK_PTR)(DWORD dwCtrlType);
	typedef WIN_CALLBACK_PTR CALLBACK_PTR;
	static void init(CALLBACK_PTR callback = &TermHandlerRoutine);
#else
	typedef void (*NON_WIN_CALLBACK_PTR)(int sig, siginfo_t* pSiginfo, void* pv);
	typedef NON_WIN_CALLBACK_PTR CALLBACK_PTR;
	static void init(CALLBACK_PTR = &sigAction);
#endif
	static void exit();
	static void forceExit();
	static void sleep( long msec );
	static bool isTerminated();

private:
	// Use a vector to remember each unique callback function pointer that is registered
	static rfa::common::RFA_Vector<CALLBACK_PTR> _callbacks;
	static bool _isTerminated;
	static bool _isInitialized;

#ifdef WIN32
	static BOOL WINAPI TermHandlerRoutine( DWORD dwCtrlType );
#else
public:
	static void sigAction( int sig, siginfo_t* pSiginfo, void* pv );
private:
	static struct sigaction 	_sigAction;
	static struct sigaction 	_oldSigAction;
#endif 
};

#endif //  __SESSION_LAYER_THREADED_EXAMPLE__CTRL_BREAK_HANDLER__H__
