#ifndef SYMBOLLISTHANDLER_H
#define SYMBOLLISTHANDLER_H

#include "StdAfx.h"
#include <boost/python.hpp>

class RDMFieldDict;

using namespace boost::python;

class SymbolListHandler
{
public:
    SymbolListHandler(rfa::sessionLayer::OMMConsumer* pOMMConsumer, 
                    rfa::common::EventQueue& eventQueue,
                    rfa::common::Client& client,
                    const std::string& serviceName, 
                    const RDMFieldDict* dict,
                    rfa::logger::ComponentLogger& componentLogger);
    ~SymbolListHandler(void);
    void                                        sendRequest(const std::string &itemName);
    void                                        closeRequest(const std::string &itemName);
    void                                        closeAllRequest();
    void                                        processResponse(const rfa::message::RespMsg& respMsg, rfa::common::Handle* handle, boost::python::tuple& out);
    std::string                                 getItemName(rfa::common::Handle* handle);
    std::string                                 getItemServiceName(rfa::common::Handle* handle);
    bool                                        isSymbolListRefreshComplete() const;
    rfa::common::Handle*                        getHandle(const std::string &itemName);
    void                                        setDebugMode(const bool &debugLevel);
    const std::list<std::string>*               getSymbolList() const;
    void                                        prettyPrint(boost::python::tuple& inputTuple);
    std::map<rfa::common::Handle*,std::string>  &getWatchList();

private:
    void                                        decodeSymbolList(const rfa::common::Data& data, boost::python::tuple &out, const std::string &itemName, const std::string &serviceName, const std::string &mtype);
    void                                        decodeFieldList(const rfa::data::FieldList& fieldList, dict& d);
    rfa::sessionLayer::OMMConsumer              *_pOMMConsumer;
    rfa::common::EventQueue                     &_eventQueue;
    rfa::common::Client                         &_client;
    rfa::common::Handle                         *_pHandle;
    const std::string                           &_serviceName;
    const RDMFieldDict                          *_pDict;
    bool                                        _isSymbolListRefreshComplete;
    std::list<std::string>                      _symbolList; // cache symbolList response in _symbolList
    bool                                        _debug;
    rfa::common::RFA_String                     _log;
    rfa::logger::ComponentLogger                &_componentLogger;
    std::map<rfa::common::Handle*,std::string>  _watchList;
    int                                         _refreshCount;
};
#endif
