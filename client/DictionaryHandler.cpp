#include "StdAfx.h"

#include "common/RDMDict.h"
#include "common/RDMDictionaryDecoder.h"
#include "DictionaryHandler.h"
#include "common/RDMUtils.h"

DictionaryHandler::DictionaryHandler(rfa::sessionLayer::OMMConsumer* pOMMConsumer, 
                                     rfa::common::EventQueue* eventQueue,
                                     rfa::common::Client* client,
                                     std::string& serviceName,
                                     rfa::common::UInt32 dictTraceLevel,
                                     rfa::logger::ComponentLogger& componentLogger):
_pOMMConsumer(pOMMConsumer),
_pEventQueue(eventQueue),
_pClient(client),
_serviceName(serviceName),
_isAvailable(false),
_isDictionaryRefreshComplete(true),
_debug(false),
_log(""),
_componentLogger(componentLogger)
{
    // enable debug prints in Dictionary classes
    RDMDict::Trace = dictTraceLevel;
    //RDMDict::Trace = 0x03; // dump dictionary version
    //RDMDict::Trace = 0x30; // dump dictionary to screen  
    //RDMDict::Trace = 0x20; // dump fieldDictionary
    //RDMDict::Trace = 0x10; // dump enumTypedef

     // prepare to download dictionaries from network
    _pNetworkDictDecoder = new RDMNetworkDictionaryDecoder(_dict);
}

DictionaryHandler::DictionaryHandler(const std::string& fieldDictFile,
                                     const std::string& enumDictFile,
                                     rfa::logger::ComponentLogger& componentLogger):
_debug(false),
_log(""),
_componentLogger(componentLogger)
{
    // load dictionaries from disk
    _pFileDictDecoder = new RDMFileDictionaryDecoder(_dict);
    _isAvailable = _pFileDictDecoder->load(fieldDictFile.c_str(), enumDictFile.c_str());
    if (!_isAvailable) {
        _log = "[DictionaryHandler::DictionaryHandler] Unable to load dictionaries from  ";
        _log.append(fieldDictFile.c_str());
        _log +=", ";
        _log.append(enumDictFile.c_str());
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error,_log.c_str());
        if(_debug)
            cout << "[DictionaryHandler::DictionaryHandler] Unable to load dictionaries from  "<< fieldDictFile << ", " << enumDictFile << endl;
    } else {
        if(_debug)
            cout << "[DictionaryHandler::DictionaryHandler] Successfully load dictionaries from  "<< fieldDictFile << ", " << enumDictFile << endl;
    }
}

DictionaryHandler::~DictionaryHandler(void)
{
}


void DictionaryHandler::sendRequest() {

    rfa::message::ReqMsg reqMsg;
    rfa::message::AttribInfo attribInfo(true);    
    rfa::sessionLayer::OMMItemIntSpec intSpec;

    // request field dictionary
    const rfa::common::RFA_String fieldDictName = "RWFFld";
    attribInfo.setName(fieldDictName);
    attribInfo.setDataMask(rfa::rdm::DICTIONARY_NORMAL);

    attribInfo.setServiceName(_serviceName.c_str());
    reqMsg.setIndicationMask(rfa::message::ReqMsg::AttribInfoInUpdatesFlag);
    reqMsg.setAttribInfo(attribInfo);

    reqMsg.setMsgModelType(rfa::rdm::MMT_DICTIONARY);
    reqMsg.setInteractionType( rfa::message::ReqMsg::InitialImageFlag| rfa::message::ReqMsg::InterestAfterRefreshFlag);
    intSpec.setMsg( &reqMsg );
    //rfa::common::Handle* pFieldDictHandle = _pOMMConsumer->registerClient(_pEventQueue, &intSpec, *_pClient);
    _pOMMConsumer->registerClient(_pEventQueue, &intSpec, *_pClient);

    // request enum dictionary
    const rfa::common::RFA_String enumDictName = "RWFEnum";
    attribInfo.setName(enumDictName.c_str());
    reqMsg.setAttribInfo(attribInfo);

    //rfa::common::Handle* pEnumDictHandle =_pOMMConsumer->registerClient(_pEventQueue, &intSpec,*_pClient);
    _pOMMConsumer->registerClient(_pEventQueue, &intSpec,*_pClient);

    _isDictionaryRefreshComplete = false;
}

void DictionaryHandler::closeRequest() {
}

void DictionaryHandler::processResponse( const rfa::message::RespMsg& respMsg){
    switch (respMsg.getRespType())
    {    
    case rfa::message::RespMsg::RefreshEnum:
        {
            processRefreshMsg(respMsg);
            break;
        }
    case rfa::message::RespMsg::StatusEnum:
        {
            processStatusMsg(respMsg);
            break;
        }
    default:
        _log = "[DictionaryHandler::processResponse] DictionaryHandler: Unexpected Response Type ";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error,_log.c_str());
    }
}

void DictionaryHandler::processRefreshMsg( const rfa::message::RespMsg& respMsg){

    bool moreFragments;
    
    if(respMsg.getIndicationMask() & rfa::message::RespMsg::RefreshCompleteFlag)
    {
        _isAvailable = true;
        moreFragments = false;
        if(_debug) {
            _log = "[DictionaryHandler::processRefreshMsg] Refresh Complete";
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
        }
    }
    else
        moreFragments = true;

    //check RespStatus
    if(respMsg.getHintMask() &  rfa::message::RespMsg::RespStatusFlag)
    {
        const rfa::common::RespStatus& respStatus = respMsg.getRespStatus();

        if(respStatus.getStreamState() == rfa::common::RespStatus::ClosedRecoverEnum)
        {
            _isAvailable = false;
            _log = "[DictionaryHandler::processRefreshMsg] Received Closed Recover due to ";
            _log.append(respStatus.getStatusText().c_str());
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error,_log.c_str());
            return;
        }
        else if(respStatus.getStreamState() == rfa::common::RespStatus::ClosedEnum)
        {
            _isAvailable = false;
            _log ="[DictionaryHandler::processRefreshMsg] Received Closed due to ";
            _log.append(respStatus.getStatusText().c_str());
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error,_log.c_str());
            return;
        }
    }

    if (respMsg.getHintMask() & rfa::message::RespMsg::PayloadFlag) {
        if (respMsg.getHintMask() & rfa::message::RespMsg::AttribInfoFlag) {
            const rfa::message::AttribInfo& attrib = respMsg.getAttribInfo();
            if (attrib.getName() == "RWFFld") {
               _pNetworkDictDecoder->loadAppendix_A(respMsg.getPayload(), moreFragments);
            } else if (attrib.getName()== "RWFEnum") {
                _pNetworkDictDecoder->loadEnumTypeDef(respMsg.getPayload(), moreFragments);
                if (!moreFragments) {
                    _isDictionaryRefreshComplete = true;
                }
            } else {
                _log = "[DictionaryHandler::processRefreshMsg] Unknown dictionary";
                _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error,_log.c_str());
            }
        }
        else {
            _log = "[DictionaryHandler::processRefreshMsg] Dictionary has no AttribInfo!";
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error,_log.c_str());
        }
    }
    else {
        _log = "[DictionaryHandler::processRefreshMsg] Empty Dictionary!";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error,_log.c_str());
    }
}

void DictionaryHandler::processStatusMsg( const rfa::message::RespMsg& respMsg){

    //the only info we care about in the status Msg is if the StreamState is Closed or ClosedRecover
    // OR an new Dict version is indicated by a DataSuspect
    if(respMsg.getHintMask() &  rfa::message::RespMsg::RespStatusFlag)
    {
        const rfa::common::RespStatus& respStatus = respMsg.getRespStatus();

        if(respStatus.getStreamState() == rfa::common::RespStatus::ClosedRecoverEnum)
        {
            _isAvailable = false;
            _log = "[DictionaryHandler::processStatusMsg] Received Closed Recover due to ";
            _log.append(respStatus.getStatusText().c_str());
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error,_log.c_str());
            return;

        }
        else if(respStatus.getStreamState() == rfa::common::RespStatus::ClosedEnum)
        {
            _isAvailable = false;
            _log = "[DictionaryHandler::processStatusMsg] Received Closed due to ";
            _log.append(respStatus.getStatusText().c_str());
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error,_log.c_str());
            return;
        }        

        if(respStatus.getDataState() == rfa::common::RespStatus::SuspectEnum)
        {
            // Note: this indicates provider has a newer minor version of dictionary
            // The consumer may issue a close and re-request here to receive new dictionary
            _log = "[DictionaryHandler::processStatusMsg] Dictionary suspect ";
            _log.append(respStatus.getStatusText().c_str());
            _log += " streamState=\"";
            _log.append(RDMUtils::streamStateToString(respStatus.getStreamState()).c_str());
            _log += "\"";
            _log += " statusCode=\"";
            _log.append(RDMUtils::statusCodeToString(respStatus.getStatusCode()).c_str());
            _log += "\"";
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Warning,_log.c_str());
            return;
        }
    }
}

bool DictionaryHandler::isAvailable() const {
    return _isAvailable;
}

const RDMFieldDict* DictionaryHandler::getDictionary() const {
    if (isAvailable()) 
        return &_dict;
    else
        return NULL;
}

bool DictionaryHandler::isDictionaryRefreshComplete() const {
    return _isDictionaryRefreshComplete;
}

void DictionaryHandler::setDebugMode(const bool &debug) {
    _debug = debug;
}
