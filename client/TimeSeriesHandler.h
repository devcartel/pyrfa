#ifndef TIMESERIESHANDLER_H
#define TIMESERIESHANDLER_H

#include "StdAfx.h"
#include <boost/python.hpp>

class RDMFieldDict;
class TS1DefDb;
class TS1Series;
class TS1SampleIterator;

using namespace boost::python;

class TimeSeriesHandler
{
public:
    TimeSeriesHandler(rfa::sessionLayer::OMMConsumer* pOMMConsumer, 
                    rfa::common::EventQueue& eventQueue,
                    rfa::common::Client& client,
                    const std::string& serviceName,
                    const RDMFieldDict* dict,
                    rfa::logger::ComponentLogger& componentLogger,
                    rfa::ts1::TS1DefDb& TS1DictDb);
    ~TimeSeriesHandler(void);
    void                                        sendRequest(const std::string &itemName);
    void                                        closeRequest(const std::string &itemName);
    void                                        closeAllRequest();
    void                                        processResponse(const rfa::message::RespMsg& respMsg, rfa::common::Handle* handle, boost::python::tuple& out);
    std::string                                 getItemName(rfa::common::Handle* handle);
    std::string                                 getItemServiceName(rfa::common::Handle* handle);
    rfa::common::Handle*                        getHandle(const std::string &itemName);
    const std::list<std::string>*               getTimeSeries(const int &maxRecords);
    void                                        setDebugMode(const bool &debugLevel);
    void                                        initTimeSeries(rfa::ts1::TS1Series* pTimeSeries);
    std::map<rfa::common::Handle*,std::string>  &getWatchList();
    bool                                        isTimeSeriesParseComplete();

private:
    void                                        decodeMarketPrice(const rfa::common::Data& data, dict &d);
    void                                        decodeTimeSeries(const rfa::message::RespMsg& respMsg, dict &d);
    rfa::sessionLayer::OMMConsumer              *_pOMMConsumer;
    rfa::logger::ComponentLogger                &_componentLogger;
    rfa::common::EventQueue                     &_eventQueue;
    rfa::common::Client                         &_client;
    const std::string                           &_serviceName;
    rfa::common::Handle                         *_pHandle;
    const RDMFieldDict*                         _pDict;
    bool                                        _debug;
    std::map<rfa::common::Handle*,std::string>  _watchList;
    rfa::ts1::TS1DefDb                          &_TS1DictDb;
    rfa::ts1::TS1Series                         *_pTimeSeries;
    std::list<std::string>                      _timeSeries; // cached timeseries
    int                                         _numberOfDictParsed;
    int                                         _numberOfTimeSeriesRefreshParsed;
    bool                                        _isTimeSeriesParseComplete;
    void                                        prettyPrint(boost::python::tuple& inputTuple);
    rfa::common::RFA_String                     _log;
};
#endif
