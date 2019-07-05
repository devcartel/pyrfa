//
//|---------------------------------------------------------------
//|                Copyright (C) 2001 Reuters,                  --
//|         1400 Kensington Road, Oak Brook, IL. 60523          --
//| All rights reserved. Duplication or distribution prohibited --
//|---------------------------------------------------------------
//

// MessageLoop.h: interface for the related MessageLoop class.
//
//////////////////////////////////////////////////////////////////////


#if !defined CMESSAGE_LOOP_H
#define CMESSAGE_LOOP_H

namespace rfa {
	namespace common {
		class EventQueue;
	}
}


#include "Timer.h"

class CMessageLoop
{
public:
	CMessageLoop();
	~CMessageLoop();

	void	run(rfa::common::EventQueue  *pEQueue, CTimer* timer);
	
	inline void terminate() { _isActive = false; }

	void	setTimerClient(CTimerClient& timerClient);

private:
	CMessageLoop(const CMessageLoop&);
	CMessageLoop& operator=(const CMessageLoop&);

	bool _isActive;
	CTimerClient*	_timerClient;
};


#endif // !defined(CMESSAGE_LOOP_H)
