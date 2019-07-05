#ifdef WIN32
#pragma warning( disable : 4786)
#endif

#include "Timer.h"

#include <assert.h>

#ifdef WIN32
#include <windows.h>
//#include <crtdbg.h>
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif

#include <stdlib.h>


CTimer::CTimer()
{
}



CTimer::~CTimer()
{
	assert(_timerCallbacks.empty());
}

void	CTimer::addTimerClient( CTimerClient &timerClient, long millisec, bool repeating, void* pClosure  )
{
	CTimerCallback *pTimerCallback  = new CTimerCallback;
	pTimerCallback->_pTimerClient	= &timerClient;
	pTimerCallback->_millisec		= millisec < Timer_MinimumInterval ? Timer_MinimumInterval : millisec;
	pTimerCallback->_repeating		= repeating;
	pTimerCallback->_pClosure		= pClosure;

	addTimerCallback( pTimerCallback );

}


void    CTimer::dropTimerClient( CTimerClient &timerClient )
{
	for( _TimerCallbacks::iterator it = _timerCallbacks.begin(); it != _timerCallbacks.end(); )
	{
		CTimerCallback *pTimerCallBack = *it;
		if( pTimerCallBack->_pTimerClient == &timerClient )
		{
			it = _timerCallbacks.erase( it );
			delete pTimerCallBack;
		}
		else
			it++;
	}
}


void  CTimer::addTimerCallback( CTimerCallback *pTimerCallback )
{
	pTimerCallback->_expirationTime.set( pTimerCallback->_millisec );

	if( !_timerCallbacks.empty() )
	{		
		_TimerCallbacks::iterator it;
		for( it = _timerCallbacks.begin(); 
			   it != _timerCallbacks.end() && (*it)->_expirationTime <= pTimerCallback->_expirationTime; it++ );
		_timerCallbacks.insert(it, pTimerCallback);

	}
	else
		_timerCallbacks.push_back( pTimerCallback );

}


long	CTimer::nextTimer() const
{
	if( !_timerCallbacks.empty() )
	{
			long retVal =  _timerCallbacks.front()->_expirationTime - CTime::getCurrentTime();

// This check enforces that the already expired timers will be fired right away
			if( retVal < Timer_MinimumInterval )
				retVal = 0;
			return retVal;
	}
	else
#ifdef WIN32
		return INFINITE;
#else
		return -1;
#endif
}


void    CTimer::processExpiredTimers()
{
	CTime currentTime = CTime::getCurrentTime();

	_TimerCallbacks::iterator it = _timerCallbacks.begin(); 
	while(  it != _timerCallbacks.end() && (*it)->_expirationTime <= currentTime )
	{
		CTimerCallback *pTimerCallBack = *it;

		_timerCallbacks.erase( it );

		it = _timerCallbacks.begin();
		if( pTimerCallBack->_repeating )
			addTimerCallback( pTimerCallBack );

		pTimerCallBack->_pTimerClient->processTimer(pTimerCallBack->_pClosure);
	}



}


bool	CTimer::CTime::operator <= (  const CTime &two )
{
	return _sec < two._sec || ( _sec == two._sec && _millisec <= two._millisec );
}

long	CTimer::CTime::operator - ( const CTime &two )
{
	return ( _sec - two._sec ) * 1000 + ( _millisec - two._millisec );
}

void	CTimer::CTime::set( long millisec )
{
	setCurrentTime();

	//protect against large function input parameter millisec!!
	//first divide the millisec passed in by 1000 and add the quotient to _sec and add the remainder to _millisec
	ldiv_t dt = ldiv( millisec, 1000L );
	_sec += dt.quot;
	_millisec += dt.rem;

	//now carry over any seconds accumulated from _millisec to _sec
	dt = ldiv( _millisec, 1000L );
	_sec += dt.quot;
	_millisec = dt.rem;
}


CTimer::CTime CTimer::CTime::getCurrentTime()
{
	return CTime().setCurrentTime();
}



CTimer::CTime & CTimer::CTime::setCurrentTime()
{
#ifdef WIN32
	; // SEE MSDN Article: BUG: C2248 When Access Protected Nested Class in Derived Class, ID: Q122370 

	struct	_timeb	time;
	_ftime( &time );
	_sec = static_cast<long>(time.time);
	_millisec = time.millitm;
#else
	struct  timeval _time;
	gettimeofday( &_time, 0 );
	_sec = _time.tv_sec;
	_millisec = _time.tv_usec / 1000;
#endif

	return *this;
}



CTimerClient::CTimerClient()
{
}


CTimerClient::~CTimerClient()
{
}

