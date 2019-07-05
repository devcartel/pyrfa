#ifndef RDMCLIENT_H
#define RDMCLIENT_H

#include "StdAfx.h"

// class forwarding
class LoginHandler;
class DirectoryHandler;
class DictionaryHandler;
class SymbolListHandler;
class MarketPriceHandler;
class MarketByOrderHandler;
class MarketByPriceHandler;
class OMMCProvServer;

class RDMClient :
    public rfa::common::Client
{
public:
    RDMClient(rfa::sessionLayer::Session& session, 
        rfa::common::EventQueue& eventQueue,       
        const std::string& serviceName,
        rfa::logger::ComponentLogger& componentLogger);

    RDMClient(rfa::sessionLayer::Session& session, 
        rfa::common::EventQueue& eventQueue,       
        const std::string& serviceName,
        const std::string& vendorName,
        rfa::logger::ComponentLogger& componentLogger);

	void createOMMConsumer();
    void createOMMProvider();
    void login(std::string username, std::string instanceId);
    bool isLoggedIn() const;
    bool receivedLoginStatus() const;
    void directoryRequest();
    bool isConnectionUp();
    void dictionaryRequest(const rfa::common::RFA_String &fieldDictionaryFilename, const rfa::common::RFA_String &enumTypeFilename, const rfa::common::RFA_String &dumpDataDict);
    bool isNetworkDictionaryAvailable() const;
    bool isDictionaryRefreshComplete() const;
    //int symbolListRequest(const std::string &listName, const rfa::common::UInt32 &maxSymbols);
    bool isSymbolListRefreshComplete() const;
    std::string* getSymbolList();
    void printSymbolList();
    //std::string* getWatchList();
    std::string* getMarketPriceWatchList();
    std::string* getMarketByOrderWatchList();
    std::string* getMarketByPriceWatchList();
    void printWatchList();
    void marketPriceRequest(const std::string& itemName);
    void marketPriceCloseRequest(const std::string& itemName);
    void marketPriceCloseAllRequest();
    void marketByOrderRequest(const std::string& itemName);
    void marketByOrderCloseRequest(const std::string& itemName);
    void marketByOrderCloseAllRequest();
    void marketByPriceRequest(const std::string& itemName);
    void marketByPriceCloseRequest(const std::string& itemName);
    void marketByPriceCloseAllRequest();
    void directorySubmit();
    void marketPriceSubmit(const std::string& itemName, const rfa::common::RFA_Vector<rfa::common::RFA_String>& fieldList);
    void marketByOrderSubmit(const std::string& itemName, const rfa::common::RFA_Vector<rfa::common::RFA_String>& fieldList, const std::string& mapAction, const std::string& mapKey);
    void marketByPriceSubmit(const std::string& itemName, const rfa::common::RFA_Vector<rfa::common::RFA_String>& fieldList, const std::string& mapAction, const std::string& mapKey);
    void symbolListSubmit(const std::string& itemName, const rfa::common::RFA_Vector<rfa::common::RFA_String>& fieldList, const std::string& mapAction, const std::string& mapKey);
    void closeSubmit(const std::string& itemName);
    void closeAllSubmit();

    void cleanup();
    void setDebugMode(const bool &debug);

    virtual ~RDMClient(void);

    rfa::common::RFA_String _out;

protected:

    //implements Client::processEvent()
    void processEvent(const rfa::common::Event& event);

private:
    RDMClient(const RDMClient&);
    RDMClient& operator=(const RDMClient&);

    void processConnectionEvent(const rfa::sessionLayer::ConnectionEvent & CEvent);
    void processOMMItemEvent(const rfa::sessionLayer::OMMItemEvent & UMEvent);
    void processOMMCmdErrorEvent(const rfa::sessionLayer::OMMCmdErrorEvent &CmdEvent);

    rfa::sessionLayer::Session      &_session;
    rfa::common::EventQueue         &_eventQueue;
    rfa::sessionLayer::OMMConsumer  *_pOMMConsumer;
    rfa::sessionLayer::OMMProvider  *_pOMMProvider;
    rfa::logger::ComponentLogger    &_componentLogger;

    std::string _serviceName;
    const std::string _vendorName;

    //const std::string& _fieldDictFile;
    //const std::string& _enumDictFile;

    rfa::common::Handle			*_pConnHandle;
    rfa::common::Handle         *_pErrHandle;

    LoginHandler                *_pLoginHandler;
    DirectoryHandler            *_pDirectoryHandler;
    DictionaryHandler           *_pDictionaryHandler;
    SymbolListHandler           *_pSymbolListHandler;
    MarketPriceHandler          *_pMarketPriceHandler;
    MarketByOrderHandler        *_pMarketByOrderHandler;
    MarketByPriceHandler        *_pMarketByPriceHandler;
    OMMCProvServer              *_pOMMCProvServer;
    std::string                 _symbolList;
    std::string                 _watchList;
    rfa::common::RFA_String     _log;
    bool                        _isConnectionUp;
    bool                        _debug;

};

#endif