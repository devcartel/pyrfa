//
// This file handles our RFA Message loop
// MessageLoop.cpp: implementation of the related MessageLoop class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning( disable : 4786)

#include "MessageLoop.h"
#include "Common/Dispatchable.h"
#include "CtrlBreakHandler.h"
#include <cstdio>

using namespace rfa::common;

//////////////////////////////////////////////////////////////////////
// RFA_MessageLoop Implementation
//////////////////////////////////////////////////////////////////////
RFA_MessageLoop::RFA_MessageLoop() :
 _pDispatchable(0)
{
}

RFA_MessageLoop::~RFA_MessageLoop()
{
}

//////////////////////////////////////////////////////////////////////
// WinMsg_RFA_MessageLoop Implementation
//////////////////////////////////////////////////////////////////////
RFA_Vector<int>						WinMsg_RFA_MessageLoop::_threadIds;
RFA_Vector<WinMsg_RFA_MessageLoop*>	WinMsg_RFA_MessageLoop::_threadPtrs;

WinMsg_RFA_MessageLoop::WinMsg_RFA_MessageLoop() :
 _threadId(0),
 _quit(false),
 _totMsgsDispatched(0)
{
	// Register our static CtrlHandler() method
	// Note the CtrlBreakHandler ignores multiple registrations for the same method.
	CtrlBreakHandler::init(&CtrlHandler);
}

WinMsg_RFA_MessageLoop::~WinMsg_RFA_MessageLoop()
{
}

void WinMsg_RFA_MessageLoop::quit()
{
	_quit = true;
#ifdef WIN32
	::PostThreadMessage( _threadId, WM_QUIT, 0, 0 );
#endif
}

BOOL WINAPI WinMsg_RFA_MessageLoop::CtrlHandler( DWORD CtrlType )
{
	switch ( CtrlType )
	{
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
		{
			// Set quit flag & post WM_QUIT for all instances of this class
			for ( unsigned int idx = 0; idx < WinMsg_RFA_MessageLoop::_threadIds.size(); idx++ )
			{
				_threadPtrs[idx]->_quit = true;
				::PostThreadMessage(_threadIds[idx], WM_QUIT, 0, 0);
			}
		}
		break;
	}

	// this CtrlBreakHandler always returns FALSE
	// since it may only stop the threads it knows about!
	// This way we make sure that the application's CtrlBreakHandler
	// will get its chance to break out of its message loop!
	return FALSE;
}

void WinMsg_RFA_MessageLoop::notify( rfa::common::Dispatchable& eventSource, void* closure )
{
	// post message to this thread's queue
	::PostThreadMessage( _threadId, WM_USER, 0, 0 );
}

void WinMsg_RFA_MessageLoop::init( rfa::common::Dispatchable& disp )
{
	// protect from reinitializing of the message loop
	if ( _pDispatchable ) return;

	// get this thread id
	_threadId = GetCurrentThreadId();

	// and save it on the threads list
	_threadIds.push_back( _threadId );

	// save this pointer of each instance for the static CtrlHandler() method
	_threadPtrs.push_back( this );

	_pDispatchable = &disp;

	// force this thread to create its own message queue
	MSG msg;
	::PeekMessage( &msg, NULL, WM_USER, WM_USER, PM_NOREMOVE );
}

void WinMsg_RFA_MessageLoop::run()
{
	// if it is not initialized yet, do exit
	if ( !_pDispatchable ) return;

	MSG msg;

	// GetMessage() returns 0 if it processes a WM_QUIT msg.
	// However, the Windows OS will not post WM_QUIT msgs
	// to a thread's msg queue if it appears to be busy (msgs backed up).
	// So we use a bool flag to indicate when its time to exit 
	// the msg loop (see CtrlHandler() implementation above)
	// and also exit if WM_QUIT was posted, even though we aren't expecting it.
	long msgsDispatched  = 0;
	BOOL retValGetMessage = 0;
	while ( !_quit && ( retValGetMessage = ::GetMessage( &msg, NULL, 0, 0 ) ) )
	{
		if( retValGetMessage == -1 ) break; // An error occurred in GetMessage()
		msgsDispatched = dispatchQueue();
//		printf("WinMsg_RFA_MessageLoop::run() while loop, last iter dispatched %ld msgs, total msgs dispatched = %ld\n", msgsDispatched, _totMsgsDispatched);
	}
	if( retValGetMessage == -1 )
		printf("WinMsg_RFA_MessageLoop::run() EXITED while loop GetMessage() ERROR = %lu, last iter dispatched %ld msgs, total msgs dispatched = %ld\n", GetLastError(), msgsDispatched, _totMsgsDispatched);
//	else if( !retValGetMessage )
//		printf("WinMsg_RFA_MessageLoop::run() EXITED while loop processed WM_QUIT msg, last iter dispatched %ld msgs, total msgs dispatched = %ld\n", msgsDispatched, _totMsgsDispatched);
//	else if( _quit )
//		printf("WinMsg_RFA_MessageLoop::run() EXITED while loop QUIT FLAG = true, last iter dispatched %ld msgs, total msgs dispatched = %ld\n", msgsDispatched, _totMsgsDispatched);
}

void WinMsg_RFA_MessageLoop::cleanup()
{
	// no need to cleanup if it was not initialized
	if ( !_pDispatchable ) return;

	// Unregister interest before attempting to drain the queue
	_pDispatchable->unregisterNotificationClient( *this );

	// Applications are not required to dispatch any msgs that remain in the event queue.
	// However, this application illustrates a technique to drain the event queue.

	// Setting the _quit flag to false allows dispatchQueue() 
	// to remove all the remaining msgs from the event queue.
	_quit = false;
	long count = dispatchQueue();
	printf("WinMsg_RFA_MessageLoop::cleanup() %ld more msgs were dispatched, total msgs dispatched = %ld\n", count, _totMsgsDispatched);

	// The removal of the thread ID and pointer vectors needs to be paired like it is
	// in init() for later access in CtrlHandler().
	_threadPtrs.removeValue( this );
	_threadIds.removeValue( this->_threadId );

	// Prevent further use of the dispatchable pointer
	// Don't delete it here the RDMCosnumer Client instance will take care of it
	_pDispatchable = 0;
}

long WinMsg_RFA_MessageLoop::dispatchQueue()
{
	// Dispatches all currently available msgs from the event queue
	// and returns the number of msgs that were dispatched.

	// Dispatch msgs from the event queue while examining the return value.
	// Note if dispatch() returns a non-negative value then a msg was dispatched from the event queue 
	// If dispatch() returns a negative value then no msg was dispatched
	// and the value indicates the reason why no msg was dispatched.

	// Please refer to the RFA Architecture Model, RFA Developers Guide, and or
	// RFA Reference Manual for more details about evnt queue dispatching.
	long retValDispatch = 0;
	long msgsDispatched = 0;
	bool moreToDispatch = true;
	while( moreToDispatch && !_quit )
	{
		retValDispatch = _pDispatchable->dispatch( 0 );
		if( retValDispatch < 0 )
		{
			switch( retValDispatch )
			{
			// The event queue is currently empty so break and return
			case rfa::common::Dispatchable::NothingDispatched:
				moreToDispatch = false;	break;
			
			// The following 3 cases are not expected to ever occur in this application
			// but they are checked for illustratvie purposes:

			// This will occur only if this application sets the queue to inactive
			case rfa::common::Dispatchable::NothingDispatchedInActive:
			// This wil occur only if this appilication uses an event queue group
			case rfa::common::Dispatchable::NothingDispatchedPartOfGroup:
			// This will only occur if there are no active event streams. 
			// However, this application registered interest in the login stream
			// before the msg loop run() method was invoked and won't unregister interest
			// until after the msg loop run() method has returned.
			case rfa::common::Dispatchable::NothingDispatchedNoActiveEventStreams:
			
			// For any unexpected value, set the quit flag to true and break and return
			default: _quit = true; break;
			}
		}
		else
			msgsDispatched++;
	}
	if( msgsDispatched )
		_totMsgsDispatched += msgsDispatched;
	return msgsDispatched;
}
