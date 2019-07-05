//
//|---------------------------------------------------------------
//|                Copyright (C) 2001 Reuters,                  --
//|         3 Times Square, New York, NY 10036                  --
//| All rights reserved. Duplication or distribution prohibited --
//|---------------------------------------------------------------
//

/**
 *  This class provides a common class for client logging     
 *  for RFA. The constructor sets the logger.  The method
 *  processEvent logs the messages to a file.
 *
 */

#include "ClientLog.h"
#include <fstream>
#include <iostream>

#include <assert.h>

#ifdef _ASSERTE
#undef _ASSERTE
#endif

#define _ASSERTE    assert

using namespace std;
using namespace rfa::sessionLayer;
using namespace rfa::logger;

//////////////////////////////////////////////////////////////////////
// AppLoggerClient Implementation
//////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------
/*
AppLoggerClient::AppLoggerClient(rfa::logger::ApplicationLogger * pLogger
                                 ,rfa::common::EventQueue  * pEQueue
                                 ,const char* outFilename) :
    _pLogger(pLogger), _pLoggerMonitor(0)
    ,_pLoggerHandle(0)
    ,_outFilename(outFilename)
*/
AppLoggerClient::AppLoggerClient(rfa::logger::ApplicationLogger * pLogger
                                 ,rfa::common::EventQueue  * pEQueue) :
    _pLogger(pLogger), _pLoggerMonitor(0)
    ,_pLoggerHandle(0), _debug(false)
// ----------------------------------------------------------------------
{
    /*
    ofstream* ofile = new ofstream();
    ofile->open(_outFilename.c_str(),ios::out);
    if (ofile->is_open())
        _outFile = ofile;
    else {
        _outFile = &cerr;
    }
    */
    // *************** create EventSource *******
    _pLoggerMonitor = 
        _pLogger->createApplicationLoggerMonitor("LoggerMonitor",false);
    _ASSERTE (_pLoggerMonitor);

    // *************** create an InterestSpec ****
    rfa::logger::AppLoggerInterestSpec appLoggerInterestSpec;
    appLoggerInterestSpec.setMinSeverity(rfa::common::Success);

    // *************** register interest with EventSource
    _pLoggerHandle = 
        _pLoggerMonitor->registerLoggerClient(*pEQueue
            ,appLoggerInterestSpec
            ,*this
            ,NULL);
}

// ----------------------------------------------------------------------
AppLoggerClient::~AppLoggerClient()
// ----------------------------------------------------------------------
{
    // Unregister Logger Client and release application logger
    /*
    if (_outFile != &cerr) {
        ((ofstream*)_outFile)->close();
        delete _outFile;
    }
    */
    if (_pLoggerMonitor && _pLoggerHandle) 
        _pLoggerMonitor->unregisterLoggerClient(_pLoggerHandle);

    if (_pLoggerMonitor)            _pLoggerMonitor->destroy();
}

// ----------------------------------------------------------------------
void AppLoggerClient::processEvent(const rfa::common::Event& event) {
// ----------------------------------------------------------------------
    if (event.getType() == rfa::logger::LoggerNotifyEventEnum)
    {
        const rfa::logger::LoggerNotifyEvent* pCLogEvent = 
            static_cast<const rfa::logger::LoggerNotifyEvent *>(&event);
        rfa::logger::LoggerNotifyEvent* pLogEvent = 
            const_cast<rfa::logger::LoggerNotifyEvent *>(pCLogEvent);

        std::string  componentname = pLogEvent->getComponentName().c_str();
        //long logid = pLogEvent->getLogID();
        //long eventtype = pLogEvent->getSeverity();
        std::string msgtext = pLogEvent->getMessageText().c_str();

        // ************* print logger event msg to file *******
        /*
        *_outFile << "Log " << componentname 
            << "\t context is " << msgtext 
            << std::endl 
            << std::flush;
        */
        // ************* print logger event msg *******
        //std::cout << "Log " << componentname
        //    << "\t context is " << msgtext
        //    << std::endl;
        if(_debug)
            cout << msgtext << endl;
    }
}
 void AppLoggerClient::setDebugMode(const bool &debugLevel) {
    _debug = debugLevel;
 }

