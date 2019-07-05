#ifndef MARKETPRICEHANDLER_H
#define MARKETPRICEHANDLER_H

#include "StdAfx.h"
#include <boost/python.hpp>

class RDMFieldDict;

using namespace boost::python;

class MarketPriceHandler
{
public:
    MarketPriceHandler(rfa::sessionLayer::OMMConsumer* pOMMConsumer, 
                    rfa::common::EventQueue& eventQueue,
                    rfa::common::Client& client,
                    const std::string& serviceName,
                    const RDMFieldDict* dict,
                    rfa::logger::ComponentLogger& componentLogger);
    ~MarketPriceHandler(void);
    void                                        sendRequest(const std::string &itemName, const std::string &viewFIDs, const rfa::common::UInt8 &interactionType);
    void                                        closeRequest(const std::string &itemName);
    void                                        closeAllRequest();
    void                                        processResponse(const rfa::message::RespMsg& respMsg, rfa::common::Handle* handle, boost::python::tuple& out);
    std::string                                 getItemName(rfa::common::Handle* handle);
    std::string                                 getItemServiceName(rfa::common::Handle* handle);
    rfa::common::Handle*                        getHandle(const std::string &itemName);
    void                                        setDebugMode(const bool &debugLevel);
    std::map<rfa::common::Handle*,std::string>  &getWatchList();
    void                                        prettyPrint(boost::python::tuple& inputTuple);

private:
    void                                        decodeMarketPrice(const rfa::common::Data& data, dict &d);
    rfa::sessionLayer::OMMConsumer              *_pOMMConsumer;
    rfa::logger::ComponentLogger                &_componentLogger;
    rfa::common::EventQueue                     &_eventQueue;
    rfa::common::Client                         &_client;
    const std::string                           &_serviceName;
    rfa::common::Handle                         *_pHandle;
    const RDMFieldDict*                         _pDict;
    bool                                        _debug;
    std::map<rfa::common::Handle*,std::string>  _watchList;
    int                                         _refreshCount;
    rfa::common::RFA_String                     _log;
};
#endif
