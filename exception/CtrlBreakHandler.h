#ifndef __SESSION_LAYER_THREADED_EXAMPLE__CTRL_BREAK_HANDLER__H__
#define __SESSION_LAYER_THREADED_EXAMPLE__CTRL_BREAK_HANDLER__H__

#ifdef WIN32
	#include <windows.h>
#else
    #include <unistd.h>
	#include <signal.h>
	#include <strings.h>
#endif



class CtrlBreakHandler
{
public:
	static void init();
	static void exit();
	static void forceExit();
	static void sleep( long msec );
	static bool isTerminated();

private:
	static bool _isTerminated;
	static bool _isInitialized;

#ifdef WIN32
	static BOOL WINAPI TermHandlerRoutine(  DWORD dwCtrlType );
#else
	static void   sigAction( int sig, siginfo_t * pSiginfo, void *);


	static struct sigaction 	_sigAction;
	static struct sigaction 	_oldSigAction;
#endif 


};



#endif //  __SESSION_LAYER_THREADED_EXAMPLE__CTRL_BREAK_HANDLER__H__
