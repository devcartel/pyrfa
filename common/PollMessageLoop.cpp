// PollMessageLoop.cpp: implementation of the related MessageLoop class.
//
//////////////////////////////////////////////////////////////////////

#include "PollMessageLoop.h"
#include "Common/Dispatchable.h"
#include "CtrlBreakHandler.h"

#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <new>

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
// Poll_MessageLoop Implementation
//////////////////////////////////////////////////////////////////////

Poll_MessageLoop::Poll_MessageLoop() :
 _pipeCount(0)
{
	_pipe[0] = -1;
    _pipe[1] = -1;

	// instantiate the notification pipe
	pipe( _pipe );

	// set pipe to be nonblocking
	int currentFlags =  fcntl( _pipe[0], F_GETFL, 0 );
	fcntl( _pipe[0], F_SETFL, ( currentFlags | O_NONBLOCK | F_LOCK ) );

    _pollFds[0].fd = _pipe[0];
    _pollFds[0].events = POLLIN;

	CtrlBreakHandler::init();
}

Poll_MessageLoop::~Poll_MessageLoop()
{
	_pDispatchable = 0;
}

void Poll_MessageLoop::notify( Dispatchable& eventSource, void* closure )
{
	// Poll_MessageLoop uses the notification client, which
	// is used to "notify" application on every message
	// placed by RFA in the EventQueue. 
	// _pipeCount is used to determine if RFA needs to write
	// to the _pipe, or just to increment the _pipeCount
	_pipeMutex.lock();

	++_pipeCount;
	if ( _pipeCount == 1 )
	{
		write( _pipe[1], "f", 1 );
	}

	_pipeMutex.unlock();
}

void Poll_MessageLoop::init( Dispatchable& disp )
{	
	if ( _pDispatchable ) return;

	_pDispatchable = &disp;
}

void Poll_MessageLoop::run()
{
	// The loop below needs to periodically check if Ctrl-C was pressed,
	// otherwise, this loop MAY infinitely wait inside poll().
	// This situation occurs while application does not use EventQueue.
	// This implementation of the message loop is just a functional
	// example. For details on how to implement a message loop with a better
	// performance, please see the rfa performance applications.

    char input[100];
		
	while (	!CtrlBreakHandler::isTerminated() )
    {
		int retval = poll( &_pollFds[0], 1, 500 );

		if ( retval == 0 )
		{
			// a timeout occured, do nothing
			continue;
		}

		if ( retval < 0 )
		{
			// an error ocured
			if ( errno == EINTR || errno == EAGAIN )
			{
				// a signal occured while poll-ing
				//printf("\n*********EINTR signal received while polling*********\n");
                continue;
			}
			else
			{
				char buffer[101];
				strerror_r( errno, buffer, 100 );
				printf( "\n*********Error while polling: %s*********\n", buffer );
				return;
			}
		}

		if ( ( _pollFds[0].revents & POLLHUP ) || 
			 ( _pollFds[0].revents & POLLERR ) || 
			 ( _pollFds[0].revents & POLLNVAL ) )
		{
//			if(_pollFds[0].revents & POLLHUP)
//				printf("\n*********poll() returned POLLHUP*********\n");
//			else if(_pollFds[0].revents & POLLERR)
//				printf("\n*********poll() returned POLLERR*********\n");
//			else if(_pollFds[0].revents & POLLNVAL)
//				printf("\n*********poll() returned POLLNVAL*********\n");
//			else
//				printf("\n*********poll() returned %d*********\n", _pollFds[0].revents);
			return;
		}
		else if ( ( _pollFds[0].revents & POLLIN ) ||
				  ( _pollFds[0].revents & POLLPRI ) )
		{
			_pipeMutex.lock();
			
			int numread = _pipeCount;

			// read / clean the _pipe only if there was anything put in the EventQueue
			if ( numread > 0 )
				read( _pipe[0], &input, 100 );

			_pipeCount = 0;

			_pipeMutex.unlock();

			// dispatch as many messages as it was received
			while ( numread > 0 ) 
			{
				_pDispatchable->dispatch( 0 );
				numread--;
			}
		}
	}
}

void Poll_MessageLoop::quit()
{
	// closing the pipe's file descriptor will cause the run() method to return
	close( _pollFds[0].fd );
}

void Poll_MessageLoop::cleanup()
{
	if ( _pDispatchable )
	{
		_pDispatchable->unregisterNotificationClient( *this );

		_pipeMutex.lock();
		int numread = _pipeCount;
		_pipeMutex.unlock();

		// dispatch all messages left in queue 
		while ( numread > 0 ) 
		{
			_pDispatchable->dispatch( 0 );
			numread--;
		}
		_pDispatchable = 0;
		_pipeCount = 0;
	}
	
	close( _pollFds[0].fd );
}
