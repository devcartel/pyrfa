
#include "RDMClient.h"
#include "LoginHandler.h"
#include "DirectoryHandler.h"
#include "DictionaryHandler.h"
#include "SymbolListHandler.h"
#include "MarketPriceHandler.h"
#include "MarketByOrderHandler.h"
#include "MarketByPriceHandler.h"
#include "RDMCProvServer.h"

RDMClient::RDMClient(rfa::sessionLayer::Session& session, 
                     rfa::common::EventQueue& eventQueue,                  
                     const std::string& serviceName,
                     rfa::logger::ComponentLogger& componentLogger): 
_out(""),
_session(session),
_eventQueue(eventQueue),
_pOMMConsumer(0),
_pOMMProvider(0),
_componentLogger(componentLogger),
_serviceName(serviceName),
_pLoginHandler(0),
_pDirectoryHandler(0),
_pDictionaryHandler(0),
_pSymbolListHandler(0),
_pMarketPriceHandler(0),
_pMarketByOrderHandler(0),
_pOMMCProvServer(0),
_symbolList(""),
_watchList(""),
_isConnectionUp(false),
_debug(false)
{
}

RDMClient::RDMClient(rfa::sessionLayer::Session& session, 
                     rfa::common::EventQueue& eventQueue,                  
                     const std::string& serviceName,
                     const std::string& vendorName,
                     rfa::logger::ComponentLogger& componentLogger): 
_out(""),
_session(session),
_eventQueue(eventQueue),
_pOMMConsumer(0),
_pOMMProvider(0),
_componentLogger(componentLogger),
_serviceName(serviceName),
_vendorName(vendorName),
_pLoginHandler(0),
_pDirectoryHandler(0),
_pDictionaryHandler(0),
_pSymbolListHandler(0),
_pMarketPriceHandler(0),
_pMarketByOrderHandler(0),
_pMarketByPriceHandler(0),
_pOMMCProvServer(0),
_symbolList(""),
_watchList(""),
_isConnectionUp(false),
_debug(false)
{
}

RDMClient::~RDMClient(void)
{
}


void RDMClient::createOMMConsumer() {
    //create OMMConsumer
    _pOMMConsumer = _session.createOMMConsumer("OMMConsumer");

    // register for ConnectionEvents
    rfa::sessionLayer::OMMConnectionIntSpec connIntSpec;
    _pOMMConsumer->registerClient(&_eventQueue, &connIntSpec, *this);
}

void RDMClient::createOMMProvider() {
    //create OMMProvider
    _pOMMProvider = _session.createOMMProvider("OMMProvider", true);

    rfa::sessionLayer::OMMConnectionIntSpec connIntSpec;
    _pConnHandle = _pOMMProvider->registerClient(&_eventQueue, &connIntSpec, *this);
    assert(_pConnHandle);

    // Register for Cmd Error events (These events are sent back if the submit() call fails)
    rfa::sessionLayer::OMMErrorIntSpec intErrSpec;
	_pErrHandle = _pOMMProvider->registerClient(&_eventQueue, &intErrSpec, *this);
}

void RDMClient::login(std::string username, std::string instanceId) {
    if(true) {
        _log = "[RDMClient::login] Logging in with username: ";
        _log.append(username.c_str());
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information, _log.c_str());
    }

    if(_pOMMConsumer) {
        _pLoginHandler = new LoginHandler(_pOMMConsumer, _eventQueue, *this, _componentLogger);
    } else if(_pOMMProvider) {
        _pLoginHandler = new LoginHandler(_pOMMProvider, _eventQueue, *this, _componentLogger);
    }
    
    _pLoginHandler->sendRequest(username, instanceId);
}

bool RDMClient::isLoggedIn() const {
    return _pLoginHandler->isLoggedIn();
}

bool RDMClient::receivedLoginStatus() const {
    return _pLoginHandler->receivedLoginStatus();
}

void RDMClient::directoryRequest() {
    if(!_pOMMConsumer)
        return;

    if(_debug) {
        _log = "[RDMClient::directoryRequest] Directory request... ";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information, _log.c_str());
    }
    
    _pDirectoryHandler = new DirectoryHandler(_pOMMConsumer, _eventQueue, *this, _serviceName, _componentLogger);
    _pDirectoryHandler->setDebugMode(_debug);
    _pDirectoryHandler->sendRequest();
}

void RDMClient::dictionaryRequest(const rfa::common::RFA_String &fieldDictionaryFilename, const rfa::common::RFA_String &enumTypeFilename, const rfa::common::RFA_String &dumpDataDict) {
    if(!_pOMMConsumer && !_pOMMProvider) {
        _log = "[RDMClient::dictionaryRequest] ERROR. No OMMconsumer or OMMprovider created.";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error, _log.c_str());
        return;
    }

    rfa::common::UInt32 dictTraceLevel = 0;
    if (fieldDictionaryFilename.length() && enumTypeFilename.length()) {
        if(_debug) {
            _log = "[RDMClient::dictionaryRequest] Using local dictionary... ";
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information, _log.c_str());
        }
        
        _pDictionaryHandler = new DictionaryHandler(fieldDictionaryFilename.c_str(), enumTypeFilename.c_str(), _componentLogger);
        _pDictionaryHandler->setDebugMode(_debug);

    } else {
                
        // wait for request dictionary from network until service is up
        if(_debug) {
            _log = "[RDMClient::dictionaryRequest] Downloading data dictionary from server... ";
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information, _log.c_str());
        }

        if(dumpDataDict == "true") {
            //RDMDict::Trace = 0x03; // dump dictionary version
            //RDMDict::Trace = 0x30; // dump dictionary to screen  
            //RDMDict::Trace = 0x20; // dump fieldDictionary
            //RDMDict::Trace = 0x10; // dump enumTypedef
            dictTraceLevel = 0x33;
        }

        _pDictionaryHandler = new DictionaryHandler(_pOMMConsumer, &_eventQueue, this, _serviceName, dictTraceLevel, _componentLogger);
        _pDictionaryHandler->setDebugMode(_debug);
        
        if(_pDirectoryHandler == NULL) {
            _log = "[RDMClient::dictionaryRequest] ERROR. Using network dict. Must invoke Directory request first!!";
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error, _log.c_str());
            return;
        }

        // if _serviceName is up then send dictionary request
        if(_pDirectoryHandler->isServiceUp()) {
            if (!_pDictionaryHandler->isAvailable())
                _pDictionaryHandler->sendRequest();
        } else {
            _log = "[RDMClient::dictionaryRequest] ERROR. This service is down: ";
            _log.append(_serviceName.c_str());
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error, _log.c_str());
        }
    }
    assert(_pDictionaryHandler);
}

bool RDMClient::isNetworkDictionaryAvailable() const {
    if(!_pDictionaryHandler)
        return false;
    return _pDictionaryHandler->isAvailable();
}
/*
int RDMClient::symbolListRequest(const std::string &listName, const rfa::common::UInt32 &maxSymbols) {
    if(!_pOMMConsumer)
        return 1;

    if(_debug) {
        _log = "[RDMClient::symbolListRequest] SymbolList request for: ";
        _log.append(listName.c_str());
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information, _log.c_str());
    }

    if(_pDictionaryHandler == NULL || _pDirectoryHandler == NULL) {
        _log = "[RDMClient::symbolListRequest] ERROR. Must invoke Directory/Dictionary requests first!!";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error, _log.c_str());
        return 1;
    }

    if(_pSymbolListHandler == NULL) {
        _pSymbolListHandler = new SymbolListHandler(_pOMMConsumer, _eventQueue, *this, _serviceName, listName, _pDictionaryHandler->getDictionary(), _componentLogger);
        _pSymbolListHandler->setDebugMode(_debug);
    }

    // if maxSymbols has a value greater than zero the set it
    // otherwise use system max (999999)
    if(maxSymbols > 0)
        _pSymbolListHandler->setMaxSymbols(maxSymbols);
    
    if(_pDirectoryHandler->isServiceUp()) {
        _pSymbolListHandler->sendRequest();
    } else {
        _log = "[RDMClient::symbolListRequest] ERROR. This service is down: ";
        _log.append(_serviceName.c_str());
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error, _log.c_str());
        return 1;
    }
    return 0;
}
*/
std::string* RDMClient::getSymbolList() {
    if(!_pSymbolListHandler)
        return &_symbolList;

    const list<std::string> *symbolList;
    symbolList = _pSymbolListHandler->getSymbolList();
    
    if(symbolList->size() > 0) {
        for(list<std::string>::const_iterator i=symbolList->begin(); i != symbolList->end(); ++i) {
            if(_debug)
                cout << *i << endl;;
            // serialize into a Tcl list
            _symbolList.append(*i);
            _symbolList.append(" ");
        }
    } else {
        if(_debug) {
            _log = "[RDMClient::getSymbolList] Symbol list is empty.";
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information, _log.c_str());
        }
    }
    return &_symbolList;
}

std::string* RDMClient::getMarketPriceWatchList() {
    if(!_pMarketPriceHandler)
        return &_watchList;

    _watchList.clear();

    std::map<std::string,rfa::common::Handle*> watchList;
    watchList = _pMarketPriceHandler->getWatchList();
    
    std::map<std::string,rfa::common::Handle*>::iterator it = watchList.begin();
    for(it = watchList.begin(); it != watchList.end(); it++) {
        // serialize into a Tcl list
        _watchList.append(it->first);
        _watchList.append(" ");
    }
    return &_watchList;
}

std::string* RDMClient::getMarketByOrderWatchList() {
    if(!_pMarketByOrderHandler)
        return &_watchList;

    _watchList.clear();

    std::map<std::string,rfa::common::Handle*> watchList;
    watchList = _pMarketByOrderHandler->getWatchList();
    
    std::map<std::string,rfa::common::Handle*>::iterator it = watchList.begin();
    for(it = watchList.begin(); it != watchList.end(); it++) {
        // serialize into a Tcl list
        _watchList.append(it->first);
        _watchList.append(" ");
    }
    return &_watchList;
}
std::string* RDMClient::getMarketByPriceWatchList() {
    if(!_pMarketByPriceHandler)
        return &_watchList;

    _watchList.clear();

    std::map<std::string,rfa::common::Handle*> watchList;
    watchList = _pMarketByPriceHandler->getWatchList();
    
    std::map<std::string,rfa::common::Handle*>::iterator it = watchList.begin();
    for(it = watchList.begin(); it != watchList.end(); it++) {
        // serialize into a Tcl list
        _watchList.append(it->first);
        _watchList.append(" ");
    }
    return &_watchList;
}
/*
void RDMClient::printWatchList() {
    const list<std::string> *symbolList;
    symbolList = _pSymbolListHandler->getSymbolList();
    
    if(symbolList->size() > 0) {
        for(list<std::string>::const_iterator i=symbolList->begin(); i != symbolList->end(); ++i) {
            cout << *i << endl;;
        }
    }
}
*/
bool RDMClient::isSymbolListRefreshComplete() const {
    if(_pSymbolListHandler != NULL) {
        return _pSymbolListHandler->isSymbolListRefreshComplete();
    } else {
        return true;
    }
}

bool RDMClient::isDictionaryRefreshComplete() const {
    if(_pDictionaryHandler != NULL) {
        return _pDictionaryHandler->isDictionaryRefreshComplete();
    } else {
        return true;
    }
}

void RDMClient::marketPriceRequest(const std::string& itemName) {
    if(_pMarketPriceHandler == NULL) {
        _pMarketPriceHandler = new MarketPriceHandler(_pOMMConsumer, _eventQueue, *this, _serviceName, _pDictionaryHandler->getDictionary(), _componentLogger);
        _pMarketPriceHandler->setDebugMode(_debug);

        if(_debug) {
            _log = "[RDMClient::marketPriceRequest] Subscribe to a service: ";
            _log.append(_serviceName.c_str());
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information, _log.c_str());
        }
    }
    
    // if dictionary is available then send request
    // we don't process subscription without dictionary
    if (_pDictionaryHandler->isAvailable() && _pLoginHandler->isLoggedIn()) {
        _pMarketPriceHandler->sendRequest(itemName);
    }
}

void RDMClient::marketPriceCloseRequest(const std::string& itemName) {
    if(_pMarketPriceHandler) {
        _pMarketPriceHandler->closeRequest(itemName);
    }
}

void RDMClient::marketPriceCloseAllRequest() {
    if(_pMarketPriceHandler) {
        _pMarketPriceHandler->closeAllRequest();
    }
}

void RDMClient::marketByOrderRequest(const std::string& itemName) {
    if(_pMarketByOrderHandler == NULL) {
        _pMarketByOrderHandler = new MarketByOrderHandler(_pOMMConsumer, _eventQueue, *this, _serviceName, _pDictionaryHandler->getDictionary(), _componentLogger);
        _pMarketByOrderHandler->setDebugMode(_debug);

        if(_debug) {
            _log = "[RDMClient::marketByOrderRequest] Subscribe to a service: ";
            _log.append(_serviceName.c_str());
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information, _log.c_str());
        }
    }
    
    // if dictionary is available then send request
    // we don't process subscription without dictionary
    if (_pDictionaryHandler->isAvailable() && _pLoginHandler->isLoggedIn()) {
        _pMarketByOrderHandler->sendRequest(itemName);
    }
}

void RDMClient::marketByOrderCloseRequest(const std::string& itemName) {
    if(_pMarketByOrderHandler) {
        _pMarketByOrderHandler->closeRequest(itemName);
    }
}

void RDMClient::marketByOrderCloseAllRequest() {
    if(_pMarketByOrderHandler) {
        _pMarketByOrderHandler->closeAllRequest();
    }
}

void RDMClient::marketByPriceRequest(const std::string& itemName) {
    if(_pMarketByPriceHandler == NULL) {
        _pMarketByPriceHandler = new MarketByPriceHandler(_pOMMConsumer, _eventQueue, *this, _serviceName, _pDictionaryHandler->getDictionary(), _componentLogger);
        _pMarketByPriceHandler->setDebugMode(_debug);

        if(_debug) {
            _log = "[RDMClient::marketByPriceRequest] Subscribe to a service: ";
            _log.append(_serviceName.c_str());
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information, _log.c_str());
        }
    }
    
    // if dictionary is available then send request
    // we don't process subscription without dictionary
    if (_pDictionaryHandler->isAvailable() && _pLoginHandler->isLoggedIn()) {
        _pMarketByPriceHandler->sendRequest(itemName);
    }
}

void RDMClient::marketByPriceCloseRequest(const std::string& itemName) {
    if(_pMarketByPriceHandler) {
        _pMarketByPriceHandler->closeRequest(itemName);
    }
}

void RDMClient::marketByPriceCloseAllRequest() {
    if(_pMarketByPriceHandler) {
        _pMarketByPriceHandler->closeAllRequest();
    }
}

void RDMClient::directorySubmit() {
    if(!_pOMMProvider) {
        _log = "[RDMClient::directorySubmit] ERROR. No OMMprovider created.";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error, _log.c_str());
        return;
    }

    if(_pDictionaryHandler == NULL) {
        _log = "[RDMClient::directorySubmit] ERROR. Must load local dictionary first.";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error, _log.c_str());
        return;
    }

    if(!_pLoginHandler->isLoggedIn()) {
        _log = "[RDMClient::directorySubmit] Not logged in.";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error, _log.c_str());
        return;
    }

    if(_pOMMCProvServer == NULL) {
        _pOMMCProvServer = new OMMCProvServer(_pOMMProvider, _pLoginHandler->_pLoginHandle, _serviceName, _vendorName, _pDictionaryHandler->getDictionary(), _componentLogger);
        _pOMMCProvServer->setDebugMode(_debug); 
    }

    _pOMMCProvServer->directorySubmit();
}

/*
* provider
*/
void RDMClient::marketPriceSubmit(const std::string& itemName, const rfa::common::RFA_Vector<rfa::common::RFA_String>& fieldList) {
    if(!_pOMMProvider) {
        _log = "[RDMClient::marketPriceSubmit] ERROR. No OMMprovider created.";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error, _log.c_str());
        return;
    }

    if(_pDictionaryHandler == NULL) {
        _log = "[RDMClient::marketPriceSubmit] ERROR. Must load local dictionary first.";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error, _log.c_str());
        return;
    }

    //if(!_pLoginHandler->isLoggedIn()) {
    //    _log = "[RDMClient::marketPriceSubmit] Not logged in.";
    //    _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error, _log.c_str());
    //    return;
    //}

    if(_pOMMCProvServer == NULL) {
        _pOMMCProvServer = new OMMCProvServer(_pOMMProvider, _pLoginHandler->_pLoginHandle, _serviceName, _vendorName, _pDictionaryHandler->getDictionary(), _componentLogger);
        _pOMMCProvServer->setDebugMode(_debug);
        _pOMMCProvServer->directorySubmit();
    }

    // submit refresh/update
    rfa::common::RFA_String item(itemName.c_str());
    _pOMMCProvServer->submitData(item, fieldList, rfa::rdm::MMT_MARKET_PRICE);
}

void RDMClient::marketByOrderSubmit(const std::string& itemName, const rfa::common::RFA_Vector<rfa::common::RFA_String>& fieldList, const std::string& mapAction, const std::string& mapKey) {
    if(!_pOMMProvider) {
        _log = "[RDMClient::marketByOrderSubmit] ERROR. No OMMprovider created.";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error, _log.c_str());
        return;
    }

    if(_pDictionaryHandler == NULL) {
        _log = "[RDMClient::marketByOrderSubmit] ERROR. Must load local dictionary first.";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error, _log.c_str());
        return;
    }

    //if(!_pLoginHandler->isLoggedIn()) {
    //    _log = "[RDMClient::marketPriceSubmit] Not logged in.";
    //    _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error, _log.c_str());
    //    return;
    //}

    if(_pOMMCProvServer == NULL) {
        _pOMMCProvServer = new OMMCProvServer(_pOMMProvider, _pLoginHandler->_pLoginHandle, _serviceName, _vendorName, _pDictionaryHandler->getDictionary(), _componentLogger);
        _pOMMCProvServer->setDebugMode(_debug);
        _pOMMCProvServer->directorySubmit();
    }

    // submit refresh/update
    rfa::common::RFA_String item(itemName.c_str());
    _pOMMCProvServer->submitData(item, fieldList, rfa::rdm::MMT_MARKET_BY_ORDER, mapAction, mapKey);
}

void RDMClient::marketByPriceSubmit(const std::string& itemName, const rfa::common::RFA_Vector<rfa::common::RFA_String>& fieldList, const std::string& mapAction, const std::string& mapKey) {
    if(!_pOMMProvider) {
        _log = "[RDMClient::marketByPriceSubmit] ERROR. No OMMprovider created.";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error, _log.c_str());
        return;
    }

    if(_pDictionaryHandler == NULL) {
        _log = "[RDMClient::marketByPriceSubmit] ERROR. Must load local dictionary first.";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error, _log.c_str());
        return;
    }

    //if(!_pLoginHandler->isLoggedIn()) {
    //    _log = "[RDMClient::marketPriceSubmit] Not logged in.";
    //    _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error, _log.c_str());
    //    return;
    //}

    if(_pOMMCProvServer == NULL) {
        _pOMMCProvServer = new OMMCProvServer(_pOMMProvider, _pLoginHandler->_pLoginHandle, _serviceName, _vendorName, _pDictionaryHandler->getDictionary(), _componentLogger);
        _pOMMCProvServer->setDebugMode(_debug);
        _pOMMCProvServer->directorySubmit();
    }

    // submit refresh/update
    rfa::common::RFA_String item(itemName.c_str());
    _pOMMCProvServer->submitData(item, fieldList, rfa::rdm::MMT_MARKET_BY_PRICE, mapAction, mapKey);
}

void RDMClient::symbolListSubmit(const std::string& itemName, const rfa::common::RFA_Vector<rfa::common::RFA_String>& fieldList, const std::string& mapAction, const std::string& mapKey) {
    if(!_pOMMProvider) {
        _log = "[RDMClient::symbolListSubmit] ERROR. No OMMprovider created.";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error, _log.c_str());
        return;
    }

    if(_pDictionaryHandler == NULL) {
        _log = "[RDMClient::symbolListSubmit] ERROR. Must load local dictionary first.";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error, _log.c_str());
        return;
    }

    //if(!_pLoginHandler->isLoggedIn()) {
    //    _log = "[RDMClient::marketPriceSubmit] Not logged in.";
    //    _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error, _log.c_str());
    //    return;
    //}

    if(_pOMMCProvServer == NULL) {
        _pOMMCProvServer = new OMMCProvServer(_pOMMProvider, _pLoginHandler->_pLoginHandle, _serviceName, _vendorName, _pDictionaryHandler->getDictionary(), _componentLogger);
        _pOMMCProvServer->setDebugMode(_debug);
        _pOMMCProvServer->directorySubmit();
    }

    // submit refresh/update
    rfa::common::RFA_String item(itemName.c_str());
    _pOMMCProvServer->submitData(item, fieldList, rfa::rdm::MMT_SYMBOL_LIST, mapAction, mapKey);
}

void RDMClient::closeSubmit(const std::string& itemName) {
    if(_pOMMCProvServer) {
        rfa::common::RFA_String item(itemName.c_str());
        _pOMMCProvServer->closeSubmit(item);
    }
}

void RDMClient::closeAllSubmit() {
    if(_pOMMCProvServer) {
        _pOMMCProvServer->closeAllSubmit();
    }
}

void RDMClient::cleanup() {
    if(_pLoginHandler) {
        _pLoginHandler->cleanup();
    }
    
    if(_pSymbolListHandler) {
        delete _pSymbolListHandler;
        _pSymbolListHandler = 0;
    }

    if(_pMarketPriceHandler) {
        delete _pMarketPriceHandler;
        _pMarketPriceHandler = 0;
    }

    if(_pMarketByOrderHandler) {
        delete _pMarketByOrderHandler;
        _pMarketByOrderHandler = 0;
    }

    if(_pMarketByPriceHandler) {
        delete _pMarketByPriceHandler;
        _pMarketByPriceHandler = 0;
    }

    if(_pOMMConsumer) {
        _pOMMConsumer->unregisterClient();
        _pOMMConsumer->destroy();
        _pOMMConsumer = 0;
    }

    if(_pOMMProvider) {
		if ( _pConnHandle )
			_pOMMProvider->unregisterClient(_pConnHandle);
		if ( _pErrHandle )
			_pOMMProvider->unregisterClient(_pErrHandle);

        _pOMMProvider->destroy();
        _pOMMProvider = 0;
    }
}

void RDMClient::processEvent(const rfa::common::Event& event){

    switch (event.getType())
    {

    case rfa::logger::LoggerNotifyEventEnum:
        {
            const rfa::logger::LoggerNotifyEvent &loggerEvent = static_cast<const rfa::logger::LoggerNotifyEvent &>(event);
            if(_debug) {
                _log = "[RDMClient::processEvent] LoggerNotifyEvent from ";
                _log.append(loggerEvent.getComponentName().c_str());
                _log += "(";
                _log.append((rfa::common::Int32)loggerEvent.getSeverity());
                _log += "):";
                _log.append(loggerEvent.getMessageText().c_str());
                _componentLogger.log(LM_UNEXPECTED_ONE,rfa::common::Error, _log.c_str());
            }
        }
        break;

    case rfa::sessionLayer::ConnectionEventEnum:
        {
            const rfa::sessionLayer::ConnectionEvent & CEvent = 
                static_cast<const rfa::sessionLayer::ConnectionEvent&>(event);
            processConnectionEvent(CEvent);
        }
        break;

    case rfa::sessionLayer::OMMItemEventEnum:
        {
            const rfa::sessionLayer::OMMItemEvent & OMMEvent = 
                static_cast<const rfa::sessionLayer::OMMItemEvent&>(event);
            processOMMItemEvent(OMMEvent); 
        }
        break;
    
    case rfa::sessionLayer::OMMCmdErrorEventEnum:
        {
            const rfa::sessionLayer::OMMCmdErrorEvent & CmdEvent =
                static_cast<const rfa::sessionLayer::OMMCmdErrorEvent&>(event);
            processOMMCmdErrorEvent(CmdEvent);
        }
        break;

    default:
        {
            _log = "[RDMClient::processEvent] Event type <";
            _log.append(event.getType());
            _log += "> not handled!! ";
            _componentLogger.log(LM_UNEXPECTED_ONE,rfa::common::Error, _log.c_str());
        }
        break;			
    }
}

void RDMClient::processConnectionEvent(const rfa::sessionLayer::ConnectionEvent & CEvent) 
{
    const rfa::sessionLayer::ConnectionStatus& status = CEvent.getStatus();
    if (status.getState() == rfa::sessionLayer::ConnectionStatus::Up)
    {
        if(_debug) {
            _log = "[RDMClient::processConnectionEvent] Connection Up!";
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information, _log.c_str());
        }
        _isConnectionUp = true;
    } 
    else 
    {
        if(_debug) {
            _log = "[RDMClient::processConnectionEvent] Connection Down!!!!";
            _componentLogger.log(LM_UNEXPECTED_ONE,rfa::common::Error, _log.c_str());
        }
        _isConnectionUp = false;
        
        // remove published itemList
        if(_pOMMCProvServer != NULL) {
            _pOMMCProvServer->clearPublishedItemList();
        }
    }
}

bool RDMClient::isConnectionUp()
{
    return _isConnectionUp;
}

void RDMClient::processOMMItemEvent(const rfa::sessionLayer::OMMItemEvent & UMEvent)
{
    // output string meant for Tcl dict
    _out.clear();

    const rfa::message::RespMsg& respMsg = static_cast<const rfa::message::RespMsg&> (UMEvent.getMsg());
    switch (respMsg.getMsgModelType())
    {
    case rfa::rdm::MMT_LOGIN:
        _pLoginHandler->processResponse(respMsg);
        break;
    case rfa::rdm::MMT_DIRECTORY:
        _pDirectoryHandler->processResponse(respMsg);
        break;
    case rfa::rdm::MMT_DICTIONARY:
        _pDictionaryHandler->processResponse(respMsg);
        break;
    case rfa::rdm::MMT_SYMBOL_LIST:  
        //_pSymbolListHandler->processResponse(respMsg);
        break;
    case rfa::rdm::MMT_MARKET_PRICE:
        _pMarketPriceHandler->processResponse( respMsg, _pMarketPriceHandler->getItemName(UMEvent.getHandle()), _out);
        break;
    case rfa::rdm::MMT_MARKET_BY_ORDER:
        _pMarketByOrderHandler->processResponse( respMsg, _pMarketByOrderHandler->getItemName(UMEvent.getHandle()), _out);
        break;
    case rfa::rdm::MMT_MARKET_BY_PRICE:
        _pMarketByPriceHandler->processResponse( respMsg, _pMarketByPriceHandler->getItemName(UMEvent.getHandle()), _out);
        break;
    default:
        _log = "[RDMClient::processOMMItemEvent] Message Model <";
        _log.append((int) respMsg.getMsgModelType());
        _log += "> not handled!! ";
        _componentLogger.log(LM_UNEXPECTED_ONE,rfa::common::Error, _log.c_str());
        break;
    }
}

void RDMClient::processOMMCmdErrorEvent(const rfa::sessionLayer::OMMCmdErrorEvent &CmdEvent) {
    cout << "[RDMClient::processOMMCmdErrorEvent] Command error!!!!!!!!!!!!" << endl;
}

void RDMClient::setDebugMode(const bool &debug) {
    _debug = debug;
}
