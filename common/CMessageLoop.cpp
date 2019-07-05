//
//|---------------------------------------------------------------
//|                Copyright (C) 2001 Reuters,                  --
//|         1400 Kensington Road, Oak Brook, IL. 60523          --
//| All rights reserved. Duplication or distribution prohibited --
//|---------------------------------------------------------------
//


// CMessageLoop.cpp: implementation of the related CMessageLoop class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#pragma warning( disable : 4786)
#endif

#include "../Common/CMessageLoop.h"
#include "../Common/CtrlBreakHandler.h"

#include "Common/EventQueue.h"


//////////////////////////////////////////////////////////////////////
// RFA_MessageLoop Implementation
//////////////////////////////////////////////////////////////////////

CMessageLoop::CMessageLoop():_timerClient(0)
{

}

CMessageLoop::~CMessageLoop()
{

}

void CMessageLoop::setTimerClient(CTimerClient& timerClient)
{
	_timerClient = &timerClient;
}

void CMessageLoop::run(rfa::common::EventQueue  *pEQueue, CTimer* timer)
{
	// initialize the Control handler
	CtrlBreakHandler::init();
	_isActive = true;

	long nextTimerVal = 0;
	while( !CtrlBreakHandler::isTerminated() && _isActive )
	{
		// Process all the timers that already expired or will expire within Timer_MinimumInterval 
		// millisec.
		// Timer_MinimumInterval  delta will prevent from missing timers
		if( ( nextTimerVal = timer->nextTimer() ) != -1 && nextTimerVal <= Timer_MinimumInterval )
		{
			//cout << "1. processExpiredTimers " << endl;
			timer->processExpiredTimers();
		}
		
			// dispatch until there is an event within "nextTimerVal" msecs; 
			// if nextTimerVal == -1, don't block on the dispatch 
			// also, if there are items to be dispatched keep dispatching them 
			// until they are drained from the event queue(s)
		//	while (0 <= pEQueue->dispatch( ((nextTimerVal == -1) ? 0 : nextTimerVal) ));
			while (0 <= pEQueue->dispatch( 100 ) );
	}

	CtrlBreakHandler::exit();
}

