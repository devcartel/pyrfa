//
//|---------------------------------------------------------------
//|                Copyright (C) 2001 Reuters,                  --
//|         1400 Kensington Road, Oak Brook, IL. 60523          --
//| All rights reserved. Duplication or distribution prohibited --
//|---------------------------------------------------------------
//
//
// Clients.h: interface for the related client classes.
//

#if !defined(__CLIENT_LOG_H)
#define __CLIENT_LOG_H

#include <string>

#include "Common/Client.h"
#include "SessionLayer/SessionLayerEnums.h"
#include "Logger/LoggerEnums.h"
#include "Logger/Logger.h"
#include "Logger/AppLogger.h"
#include "Logger/AppLoggerInterestSpec.h"
#include "Logger/AppLoggerMonitor.h"
#include "Logger/LoggerNotifyEvent.h"


// **************** AppLoggerClient ************************
/**
 This client is implemented in order to receive log
 notifications from RFA ApplicationLogger
 */
class AppLoggerClient : public rfa::common::Client
{
public:
    AppLoggerClient(rfa::logger::ApplicationLogger * pLogger
        ,rfa::common::EventQueue  * pEQueue);
    virtual ~AppLoggerClient();

    // handles all logger related events 
    virtual void processEvent(const rfa::common::Event & event );
    void         setDebugMode(const bool &debug);

private:
    AppLoggerClient(const AppLoggerClient&);
    AppLoggerClient& operator=(const AppLoggerClient&);

    rfa::logger::ApplicationLogger *    _pLogger;
    rfa::logger::AppLoggerMonitor *        _pLoggerMonitor;
    rfa::common::Handle *                _pLoggerHandle;
    //std::string        _outFilename;
    //std::ostream*    _outFile;
    bool                                _debug;
};


#endif // !defined(__CLIENT_LOG_H)
