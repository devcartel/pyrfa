#ifndef HISTORYHANDLER_H
#define HISTORYHANDLER_H

#include "StdAfx.h"
#include <boost/python.hpp>

class RDMFieldDict;

using namespace boost::python;

class HistoryHandler
{
public:
    HistoryHandler(rfa::sessionLayer::OMMConsumer* pOMMConsumer, 
                    rfa::common::EventQueue& eventQueue,
                    rfa::common::Client& client,
                    const std::string& serviceName, 
                    const RDMFieldDict* dict,
                    rfa::logger::ComponentLogger& componentLogger);
    ~HistoryHandler(void);
    void                                        sendRequest(const std::string &itemName, const rfa::common::UInt8 &interactionType);
    void                                        closeRequest(const std::string &itemName);
    void                                        closeAllRequest();
    void                                        processResponse(const rfa::message::RespMsg& respMsg, rfa::common::Handle* handle, boost::python::tuple& out);
    std::string                                 getItemName(rfa::common::Handle* handle);
    std::string                                 getItemServiceName(rfa::common::Handle* handle);
    bool                                        isHistoryRefreshComplete() const;
    rfa::common::Handle*                        getHandle(const std::string &itemName);
    void                                        setDebugMode(const bool &debugLevel);
    void                                        setMaxRecords(const rfa::common::UInt32 &maxRecords);
    const rfa::common::UInt32&                  getMaxRecords();
    const std::list<std::string>*               getHistory() const;
    void                                        prettyPrint(boost::python::tuple& inputTuple);
    std::map<rfa::common::Handle*,std::string>  &getWatchList();

private:
    void                                        decodeHistory(const rfa::common::Data& data, boost::python::tuple &out, const std::string &itemName, const std::string &serviceName, const std::string &mtype);
    rfa::sessionLayer::OMMConsumer              *_pOMMConsumer;
    rfa::common::EventQueue                     &_eventQueue;
    rfa::common::Client                         &_client;
    rfa::common::Handle                         *_pHandle;
    const std::string                           &_serviceName;
    const RDMFieldDict                          *_pDict;
    bool                                        _isHistoryRefreshComplete;
    std::list<std::string>                      _history;
    bool                                        _debug;
    rfa::common::UInt32                         _maxRecords;
    rfa::common::RFA_String                     _log;
    rfa::logger::ComponentLogger                &_componentLogger;
    std::map<rfa::common::Handle*,std::string>  _watchList;
    int                                         _refreshCount;
};
#endif
