// PollMessageLoop.h: interface for the related MessageLoop class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _POLL_MESSAGE_LOOP_H_
#define _POLL_MESSAGE_LOOP_H_

#include "Common/DispatchableNotificationClient.h"
#include "Common/RFA_Vector.h"
#include "Mutex.h"
#include <unistd.h>
#include <poll.h>

/**
	The idea of RFA_MessageLoop is to create an abstract user interface 
	that can be used to integrate the RFA event distribution logic into 
	an existing event dispatching mechanism

	Application may use multiple RFA_MessageLoop instances to independently
	dispatch messages from several EventQueues.
 */

class RFA_MessageLoop : public rfa::common::DispatchableNotificationClient
{
public:
	RFA_MessageLoop();
	virtual ~RFA_MessageLoop();

	virtual void init( rfa::common::Dispatchable& disp ) = 0;
	virtual void run() = 0;
	virtual void quit() = 0;
	virtual void cleanup() = 0;
	virtual void notify( rfa::common::Dispatchable& eventSource, void* closure ) = 0;

protected:
	rfa::common::Dispatchable*	_pDispatchable;
	bool						_bInit;

private:
	// do not implement!!
	RFA_MessageLoop(const RFA_MessageLoop&);
	RFA_MessageLoop& operator=(const RFA_MessageLoop&);
};

/**
	It represents a concrete Polling Message loop version of RFA_MessageLoop.
 */

class Poll_MessageLoop : public RFA_MessageLoop
{
public:
	Poll_MessageLoop();
	virtual ~Poll_MessageLoop();

	virtual void init( rfa::common::Dispatchable& disp );
	virtual void run();
	virtual void quit();
	virtual void cleanup();
	virtual void notify( rfa::common::Dispatchable& eventSource, void* closure );

private:
	// do not implement!!
	Poll_MessageLoop( const Poll_MessageLoop& );
	Poll_MessageLoop& operator=( const Poll_MessageLoop& );

	// data members
	struct pollfd		_pollFds[1];
	int                 _pipe[2];

	int					_pipeCount;
	CMutex				_pipeMutex;
};

#endif // !defined(RFA_EAP2_STTICKER_MESSAGELOOP_H)

