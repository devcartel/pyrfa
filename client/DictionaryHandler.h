#ifndef DICTIONARYHANDLER_H
#define DICTIONARYHANDLER_H

#include "../common/RDMDictDef.h"
#include "../common/RDMDict.h"


class RDMNetworkDictionaryDecoder;
class RDMFileDictionaryDecoder;

class DictionaryHandler
{
public:

    DictionaryHandler(rfa::sessionLayer::OMMConsumer* pOMMConsumer, 
        rfa::common::EventQueue* eventQueue,
        rfa::common::Client* client,
        std::string& serviceName,
        rfa::common::UInt32 dictTraceLevel,
        rfa::logger::ComponentLogger& componentLogger);

    DictionaryHandler(const string& fieldDictFile, const string& enumDictFile, rfa::logger::ComponentLogger& componentLogger);

    ~DictionaryHandler(void);

    void sendRequest();
    void closeRequest();

    void processResponse( const rfa::message::RespMsg& respMsg);

    bool isAvailable() const;
    bool isDictionaryRefreshComplete() const;
    const RDMFieldDict* getDictionary() const;
    void setDebugMode(const bool &debugLevel);

private:

    void processRefreshMsg( const rfa::message::RespMsg& respMsg);
    void processStatusMsg( const rfa::message::RespMsg& respMsg);

    RDMFieldDict _dict;
    RDMNetworkDictionaryDecoder     *_pNetworkDictDecoder;
    RDMFileDictionaryDecoder        *_pFileDictDecoder;
    rfa::sessionLayer::OMMConsumer  *_pOMMConsumer;
    rfa::common::EventQueue         *_pEventQueue;
    rfa::common::Client             *_pClient;
    const std::string               _serviceName;
    bool                            _isAvailable;
    bool                            _isDictionaryRefreshComplete;
    bool                            _debug;
    rfa::common::RFA_String         _log;
    rfa::logger::ComponentLogger    &_componentLogger;
};

#endif
