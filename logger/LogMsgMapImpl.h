
#ifndef LOGMSGMAPIMPL_H
#define LOGMSGMAPIMPL_H

#include "StdAfx.h"

class LogMsgMapImpl : public rfa::common::LogMsgMap
{
public:
    LogMsgMapImpl();
    ~LogMsgMapImpl();
    const rfa::common::RFA_String* getMsg( const unsigned long id ) const;
    LogMsgMapImpl(const LogMsgMapImpl&);
    LogMsgMapImpl& operator=(const LogMsgMapImpl&);

private:
    rfa::common::RFA_String         _LM_GENERIC_ONE;
    rfa::common::RFA_String         _LM_GENERIC_TWO;
    rfa::common::RFA_String         _LM_UNEXPECTED_ONE;
    rfa::common::RFA_String         _LM_UNDEFINED;

};

#endif // end LOGMSGMAPIMPL_H
