// pyrfa.h
#if !defined(__PYRFA_H)
#define __PYRFA_H

#include "Python.h"
#include "StdAfx.h"
#include "config/configdb.h"

//Boost Header files
#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>

// class forwarding
class LoginHandler;
class DirectoryHandler;
class DictionaryHandler;
class SymbolListHandler;
class MarketPriceHandler;
class MarketByOrderHandler;
class MarketByPriceHandler;
class OMMCProvServer;
class OMMInteractiveProvider;
class TimeSeriesHandler;
class TS1DefDb;
class TS1Series;
class HistoryHandler;
class OMMPost;

//RFA Header files

using namespace std;
using namespace rfa::common;
using namespace boost::python;

class Pyrfa : public rfa::common::Client
{
public:
    Pyrfa();
    virtual ~Pyrfa(void);
    int                             initPythonLib();
    int                             initializeRFA();
    void                            uninitializeRFA();
    void                            createConfigDb(object const &argv);
    void                            printConfigDb(object const &argv);
    std::string                     getConfigDb(object const &argv);
    void                            createEventQueue();
    ConfigDb&                       configDb() { return *_pConfigDb; }
    void                            acquireLogger();
    void                            acquireSession(object const &argv);
    void                            createOMMConsumer();
    void                            createOMMProvider();
    void                            login(object const &argv1, object const &argv2, object const &argv3, object const &argv4);
    bool                            isLoggedIn() const;
    boost::python::tuple            directoryRequest();
    bool                            isDictionaryRefreshComplete() const;
    void                            dictionaryRequest();
    int                             getFieldType(object const &argv);
    int                             getFieldID(object const &argv);
    void                            symbolListRequest(object const &argv);
    void                            symbolListCloseRequest(object const &argv);
    void                            symbolListCloseAllRequest();
    bool                            isNetworkDictionaryAvailable() const;
    bool                            isSymbolListRefreshComplete() const;
    bool                            isTimeSeriesParseComplete() const;
    bool                            isHistoryRefreshComplete() const;
    bool                            isConnectionUp() const;
    std::string                     getSymbolList(object const &argv);
    boost::python::tuple            getTimeSeries(object const &argv);
    std::string                     getSymbolListWatchList();
    std::string                     getMarketPriceWatchList();
    std::string                     getMarketByOrderWatchList();
    std::string                     getMarketByPriceWatchList();
    std::string                     getTimeSeriesWatchList();
    std::string                     getHistoryWatchList();
    void                            marketPriceRequest(object const &argv);
    void                            marketPriceCloseRequest(object const &argv);
    void                            marketPriceCloseAllRequest();
    void                            marketPricePause(object const &argv);
    void                            marketPriceResume(object const &argv);
    void                            marketByOrderRequest(object const &argv);
    void                            marketByOrderCloseRequest(object const &argv);
    void                            marketByOrderCloseAllRequest();
    void                            marketByPriceRequest(object const &argv);
    void                            marketByPriceCloseRequest(object const &argv);
    void                            marketByPriceCloseAllRequest();
    void                            setInteractionType(object const &argv);
    void                            setView(object const &argv);
    void                            setTimeSeriesPeriod(object const &argv);
    void                            setTimeSeriesMaxRecords(int maxRecords);
    void                            timeSeriesRequest(object const &argv);
    void                            timeSeriesCloseRequest(object const &argv);
    void                            timeSeriesCloseAllRequest();
    void                            historyRequest(object const &argv);
    void                            historyCloseRequest(object const &argv);
    void                            historyCloseAllRequest();
    boost::python::tuple            dispatchEventQueue(long timeout);
    void                            dispatchLoggerEventQueue(long timeout);
    void                            deactivateEventQueue();
    void                            directorySubmit(object const &argv1, object const &argv2);
    void                            marketPriceSubmit(object const &argv);
    void                            marketByOrderSubmit(object const &argv);
    void                            marketByPriceSubmit(object const &argv);
    void                            symbolListSubmit(object const &argv);
    void                            historySubmit(object const &argv);
    void                            closeSubmit(object const &argv1, object const &argv2);
    void                            closeAllSubmit();
    void                            staleSubmit(object const &argv1, object const &argv2);
    void                            staleAllSubmit();
    void                            serviceDownSubmit(object const &argv);
    void                            serviceUpSubmit(object const &argv);
    void                            logoutSubmit(object const &argv);
    void                            logoutAllSubmit();
    void                            marketPricePost(object const &argv);
    std::string                     getClientSessions();
    std::string                     getClientWatchList(object const &argv);
    void                            pauseAll();
    void                            resumeAll();
    void                            cleanUp();
    void                            setDebugMode(object const &argv);
    void                            logInfo(object const &argv);
    void                            logWarning(object const &argv);
    void                            logError(object const &argv);
    void                            setServiceName(object const &argv);
    std::string                     getServiceName();
    rfa::common::EventQueue&        eventQueue() { return *_pEventQueue; }
    rfa::logger::ApplicationLogger& logger() { return *_pLogger; }
    rfa::sessionLayer::Session&     session() { return *_pSession; }
    //const rfa::common::RFA_String*  getMsg( const unsigned long id ) const;

protected:
    void processEvent(const rfa::common::Event& event); // must implement

private:
    void                            processConnectionEvent(const rfa::sessionLayer::ConnectionEvent & CEvent);
    void                            processOMMItemEvent(const rfa::sessionLayer::OMMItemEvent &OMMEvent);
    void                            processOMMCmdErrorEvent(const rfa::sessionLayer::OMMCmdErrorEvent &CmdEvent);
    void                            processOMMActiveClientSessionEvent(const rfa::sessionLayer::OMMActiveClientSessionEvent& event);
    void                            processOMMInactiveClientSessionEvent(const rfa::sessionLayer::OMMInactiveClientSessionEvent& event);
    void                            processOMMSolicitedItemEvent(const rfa::sessionLayer::OMMSolicitedItemEvent& event);
    void                            _logInfo(std::string log);
    void                            _logWarning(std::string log);
    void                            _logError(std::string log);
    ConfigDb                        *_pConfigDb;
    rfa::common::EventQueue         *_pEventQueue;
    rfa::common::EventQueue         *_pLoggerEventQueue;
    rfa::logger::ApplicationLogger  *_pLogger;
    rfa::logger::ComponentLogger    *_pComponentLogger;
    AppLoggerClient                 *_pLoggerClient;
    rfa::sessionLayer::Session      *_pSession;
    rfa::sessionLayer::OMMConsumer  *_pOMMConsumer;
    rfa::sessionLayer::OMMProvider  *_pOMMProvider;
    LoginHandler                    *_pLoginHandler;
    DirectoryHandler                *_pDirectoryHandler;
    DictionaryHandler               *_pDictionaryHandler;
    SymbolListHandler               *_pSymbolListHandler;
    MarketPriceHandler              *_pMarketPriceHandler;
    MarketByOrderHandler            *_pMarketByOrderHandler;
    MarketByPriceHandler            *_pMarketByPriceHandler;
    TimeSeriesHandler               *_pTimeSeriesHandler;
    rfa::ts1::TS1Series             *_pTimeSeries;
    rfa::ts1::TS1DefDb              *_pTS1DictDb;
    HistoryHandler                  *_pHistoryHandler;
    rfa::common::Handle             *_pConnHandle;
    rfa::common::Handle             *_pErrHandle;
    rfa::common::Handle             *_pListConnHandle;
    rfa::common::Handle             *_pClientSessListHandle;
    rfa::sessionLayer::RequestToken *_pLoginToken;
    bool                            _debug;
    bool                            _isSymbolListAvailable;
    bool                            _isHistoryAvailable;
    bool                            _isPublisher;
    boost::python::tuple            _out;
    boost::python::tuple            _eventData;
    rfa::common::RFA_String         _log;
    bool                            _isConnectionUp;
    OMMCProvServer                  *_pOMMCProvServer;
    OMMInteractiveProvider          *_pOMMInteractiveProvider;
    OMMPost                         *_pOMMPost;
    std::string                     _symbolList;
    std::string                     _watchList;
    std::string                     _serviceName;
    std::string                     _vendorName;
    std::string                     _connectionType;
    boost::python::tuple            _timeSeries;
    rfa::ts1::TS1Series::Period     _timeSeriesPeriod;
    int                             _timeSeriesMaxRecords;
    std::string                     _userName;
    rfa::common::UInt8              _interactionType;
    std::string                     _viewFIDs;
    rfa::common::RFA_String         _fieldDictionaryFilename;
    rfa::common::RFA_String         _enumTypeFilename;
    boost::python::tuple            _directory;
};

class py_error {
    public:
        py_error(const std::string str) {_str = str;}
        ~py_error() {}
        std::string what() const {return _str;}
    private:
        std::string _str;
};
#endif // __PYRFA_H
