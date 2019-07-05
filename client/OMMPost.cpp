#ifdef WIN32
#pragma warning( disable : 4786)
#endif

#ifndef _WIN32
    #include <stdio.h>
#endif

#include "../common/RDMUtils.h"
#include "../common/RDMDict.h"
#include "OMMPost.h"

#include <boost/lexical_cast.hpp>

#define DICT_ID    1
#define FIELD_LIST_ID 3

OMMPost::OMMPost(rfa::sessionLayer::OMMConsumer* pOMMConsumer,
                               rfa::common::Handle *loginHandle,
                               const RDMFieldDict* dict,
                               rfa::logger::ComponentLogger& componentLogger):
    _pOMMConsumer(pOMMConsumer),
    _pLoginHandle(loginHandle),
    _pDict(dict),
    _debug(false),
    _log(""),
    _componentLogger(componentLogger)
{
}

OMMPost::~OMMPost()
{
}

void OMMPost::submitData(const rfa::common::RFA_String& item, const rfa::common::RFA_Vector<rfa::common::RFA_String>& fieldList, int mType, const std::string& msgType, const rfa::common::RFA_String& serviceName, const std::string& mapAction, const std::string& mapKey)
{
    if(item.empty()) {
        _log = "[OMMPost::submitData] RIC must be specified in posting data.";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Warning,_log.c_str());
        return;
    }
    _respMsg.clear();
    switch(mType) {
        case rfa::rdm::MMT_MARKET_PRICE:
            _fieldList.clear();
            _fieldList.setAssociatedMetaInfo(*_pLoginHandle);
            break;
        case rfa::rdm::MMT_MARKET_BY_ORDER:
        case rfa::rdm::MMT_MARKET_BY_PRICE:
        case rfa::rdm::MMT_SYMBOL_LIST:
            _map.clear();
            _map.setAssociatedMetaInfo(*_pLoginHandle);
            _fieldList.clear();
            _fieldList.setAssociatedMetaInfo(*_pLoginHandle);
            break;
        case rfa::rdm::MMT_HISTORY:
            _series.clear();
            _series.setAssociatedMetaInfo(*_pLoginHandle);
            _fieldList.clear();
            _fieldList.setAssociatedMetaInfo(*_pLoginHandle);
            break;
    }

    PostMsg postMsg;
    postMsg.setMsgModelType(mType);
    postMsg.setIndicationMask(PostMsg::MessageInitFlag | PostMsg::MessageCompleteFlag);

    _attribInfo.clear();    
    _attribInfo.setNameType(rfa::rdm::INSTRUMENT_NAME_RIC);
    _attribInfo.setName(item);
    _attribInfo.setServiceName(serviceName);
    postMsg.setAttribInfo(_attribInfo);
        
    if(msgType == "image") {
        if(_debug) {
            cout << "[OMMPost::submitData] sending refresh item: " << item.c_str() << endl;
            cout << "[OMMPost::submitData] sending refresh service: " << serviceName.c_str() << endl;
        }
        // Encode header and databody
        switch(mType) {
            case rfa::rdm::MMT_MARKET_PRICE:
                _encoder.encodeMarketPriceMsg(&_respMsg, RespMsg::RefreshEnum, _attribInfo,  _respStatus, &_qos, true, false); 
                _encoder.encodeMarketPriceDataBody(&_fieldList, RespMsg::RefreshEnum, fieldList, _pDict);
                _respMsg.setPayload(_fieldList);
                break;
            case rfa::rdm::MMT_MARKET_BY_ORDER:
                _encoder.encodeMarketByOrderMsg(&_respMsg, RespMsg::RefreshEnum, _attribInfo,  _respStatus, &_qos, true, false);
                _encoder.encodeMarketByOrderDataBody(&_map, &_fieldList, RespMsg::RefreshEnum, fieldList, _pDict, mapAction, mapKey);
                _respMsg.setPayload(_map);
                break;
            case rfa::rdm::MMT_MARKET_BY_PRICE:
                _encoder.encodeMarketByPriceMsg(&_respMsg, RespMsg::RefreshEnum, _attribInfo,  _respStatus, &_qos, true, false);
                _encoder.encodeMarketByPriceDataBody(&_map, &_fieldList, RespMsg::RefreshEnum, fieldList, _pDict, mapAction, mapKey);
                _respMsg.setPayload(_map);
                break;
            case rfa::rdm::MMT_SYMBOL_LIST:
                _encoder.encodeSymbolListMsg(&_respMsg, RespMsg::RefreshEnum, _attribInfo,  _respStatus, &_qos, true, false);
                _encoder.encodeSymbolListDataBody(&_map, &_fieldList, RespMsg::RefreshEnum, fieldList, _pDict, mapAction, mapKey);
                _respMsg.setPayload(_map);
                break;
            case rfa::rdm::MMT_HISTORY:
                _encoder.encodeHistoryMsg(&_respMsg, RespMsg::RefreshEnum, _attribInfo,  _respStatus, &_qos, true, false); 
                _encoder.encodeHistoryDataBody(&_series, &_fieldList, RespMsg::RefreshEnum, fieldList, _pDict);
                _respMsg.setPayload(_series);
                break;
        }
    } else if(msgType == "update") {
        if(_debug) {
            cout << "[OMMPost::submitData] sending update item: " << item.c_str() << endl;
            cout << "[OMMPost::submitData] sending update service: " << serviceName.c_str() << endl;
        }
        // Encode header and databody
        switch(mType) {
            case rfa::rdm::MMT_MARKET_PRICE:
                _encoder.encodeMarketPriceMsg(&_respMsg, RespMsg::UpdateEnum, _attribInfo,  _respStatus, &_qos, true, true);
                _encoder.encodeMarketPriceDataBody(&_fieldList, RespMsg::UpdateEnum, fieldList, _pDict);
                _respMsg.setPayload(_fieldList);
                break;
            case rfa::rdm::MMT_MARKET_BY_ORDER:
                _encoder.encodeMarketByOrderMsg(&_respMsg, RespMsg::UpdateEnum, _attribInfo,  _respStatus, &_qos, true);
                _encoder.encodeMarketByOrderDataBody(&_map, &_fieldList, RespMsg::UpdateEnum, fieldList, _pDict, mapAction, mapKey);
                _respMsg.setPayload(_map);
                break;
            case rfa::rdm::MMT_MARKET_BY_PRICE:
                _encoder.encodeMarketByPriceMsg(&_respMsg, RespMsg::UpdateEnum, _attribInfo,  _respStatus, &_qos, true);
                _encoder.encodeMarketByPriceDataBody(&_map, &_fieldList, RespMsg::UpdateEnum, fieldList, _pDict, mapAction, mapKey);
                _respMsg.setPayload(_map);
                break;
            case rfa::rdm::MMT_SYMBOL_LIST:
                _encoder.encodeSymbolListMsg(&_respMsg, RespMsg::UpdateEnum, _attribInfo,  _respStatus, &_qos, true);
                _encoder.encodeSymbolListDataBody(&_map, &_fieldList, RespMsg::UpdateEnum, fieldList, _pDict, mapAction, mapKey);
                _respMsg.setPayload(_map);
                break;
            case rfa::rdm::MMT_HISTORY:
                _encoder.encodeHistoryMsg(&_respMsg, RespMsg::UpdateEnum, _attribInfo,  _respStatus, &_qos, true); 
                _encoder.encodeHistoryDataBody(&_series, &_fieldList, RespMsg::UpdateEnum, fieldList, _pDict);
                _respMsg.setPayload(_series);
                break;
        }
    } else {
        _log = "[OMMPost::submitData] MTYPE: ";
        _log.append(msgType.c_str());
        _log.append(" not allowed. Only IMAGE or UPDATE.");
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Warning,_log.c_str());
        return;
    }
        
    _respMsg.setAttribInfo(_attribInfo);
    //Set respMsg in postMsg
    postMsg.setPayload(_respMsg);
    rfa::sessionLayer::OMMHandleItemCmd handleCmd;
    handleCmd.setHandle(*_pLoginHandle);
    handleCmd.setMsg(postMsg);
    _pOMMConsumer->submit(&handleCmd);
}

void OMMPost::setDebugMode(const bool &debug) {
    _debug = debug;
    _encoder.setDebugMode(_debug);
}
