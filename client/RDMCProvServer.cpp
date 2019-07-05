#ifdef WIN32
#pragma warning( disable : 4786)
#endif

#ifndef _WIN32
    #include <stdio.h>
#endif

#include "../common/RDMUtils.h"
#include "../common/RDMDict.h"
#include "RDMCProvServer.h"

#include <boost/lexical_cast.hpp>

#define DICT_ID    1
#define FIELD_LIST_ID 3

OMMCProvServer::OMMCProvServer(rfa::sessionLayer::OMMProvider* pOMMProvider,
                               rfa::common::Handle *loginHandle,
                               const std::string& vendorName,
                               const RDMFieldDict* dict,
                               rfa::logger::ComponentLogger& componentLogger):
    _pOMMProvider(pOMMProvider),
    _pLoginHandle(loginHandle),
    _pConnHandle(0),
    _pErrHandle(0),
    _bSendRefresh(false),
    _vendorName(vendorName.c_str()),
    _pItemList(&_itemList),
    _pDict(dict),
    _debug(false),
    _serviceState(1),
    _componentLogger(componentLogger)
{
}

OMMCProvServer::~OMMCProvServer()
{
}

void OMMCProvServer::cleanup()
{
    if ( _pItemList )
        delete _pItemList;

    // Clear out map. Don't need clean up token since application doesn't create them.
    unsigned int count = _itemMap.size(); 
    for(unsigned int i = 0; i < count; i++)
        delete _itemMap[i];

    _itemMap.clear();
    _itemList.clear();
}

void OMMCProvServer::directorySubmit(const UInt8 &domainType, rfa::common::RFA_String& serviceName)
{
    _respMsg.clear();
    _map.clear();
    if ( _pLoginHandle )
        _map.setAssociatedMetaInfo(*_pLoginHandle);
    
    if(domainType > 0)
         _encoder.setDomainModelType(domainType);

    // Encode msg header
    _attribInfo.clear();
    _attribInfo.setDataMask(SERVICE_INFO_FILTER | SERVICE_STATE_FILTER);

    if(_itemMap.empty()) {
        // directory refresh
        ItemInfo *itemInfo = new ItemInfo();
        itemInfo->name = serviceName;
        itemInfo->srvName = serviceName;
        itemInfo->token = &(_pOMMProvider->generateItemToken());
        _itemMap.push_back(itemInfo);

        // Set RespTypeNum
        _respStatus.clear();
        _respStatus.setStreamState(RespStatus::OpenEnum);
        _respStatus.setDataState(RespStatus::OkEnum);    
        _respStatus.setStatusCode(RespStatus::NoneEnum);
        RFA_String  tmpStr("Refresh Completed");
        _respStatus.setStatusText(tmpStr);

        _encoder.encodeDirectoryMsg(&_respMsg, _attribInfo, _respStatus);

        // Reset respTypeNum to unsolicited refresh
        _respMsg.setRespTypeNum(rfa::rdm::REFRESH_UNSOLICITED);

        // Encode Data Body 
        _qos.setRate(QualityOfService::realTime);
        _qos.setTimeliness(QualityOfService::tickByTick);

        _encoder.encodeDirectoryDataBody(&_map,serviceName,_vendorName, _serviceState, &_qos);    
        _respMsg.setPayload(_map);

        _log = "[OMMCProvServer::directorySubmit] Submitting directory REFRESH with domain type: ";
        _log.append(domainType);
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
    } else {
        // directory update
        _encoder.encodeDirectoryMsg(&_respMsg, _attribInfo, _respStatus, false);
        _encoder.encodeDirectoryDataBody(&_map,serviceName,_vendorName, _serviceState, &_qos);
        _respMsg.setPayload(_map);

        _log = "[OMMCProvServer::directorySubmit] Submitting directory UPDATE with domain type: ";
        _log.append(domainType);
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
    }
    rfa::sessionLayer::OMMItemCmd itemCmd;
    itemCmd.setMsg(static_cast<rfa::common::Msg&>(_respMsg));
    itemCmd.setItemToken(_itemMap[0]->token);
    _pOMMProvider->submit(&itemCmd);
}

void OMMCProvServer::clearPublishedItemList() {
    _itemMap.clear();
    _itemList.clear();
    _encoder.clearAllDomainModelTypes();
    _log = "[OMMCProvServer::clearPublishedItemList] Clear published item map and item list";
    _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
}

void OMMCProvServer::submitData(const rfa::common::RFA_String& item, const rfa::common::RFA_Vector<rfa::common::RFA_String>& fieldList, int domainType, const std::string& msgType, rfa::common::RFA_String& serviceName, const std::string& mapAction, const std::string& mapKey)
{
    if(item.empty()) {
        _log = "[OMMCProvServer::submitData] RIC must be specified when publishing data.";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Warning,_log.c_str());
        return;
    }
    
    int i = _pItemList->getPositionOf(item + "." + serviceName);
    _respMsg.clear();

    // if the data model is new then submit directory update to update server new capability.
    if(!_encoder.hasDomainModelType(domainType))
        directorySubmit(domainType, serviceName);

    switch(domainType) {
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

    /*
    * if not found in itemList then it's new. Send refresh message then updates
    * or force send refresh is set
    * new item is always sent as refresh message
    */
    if(i < 0 || msgType == "image") {
        // if item is new, create new itemInfo and save in itemMap
        if(i < 0) {
            // save item in itemList
            _pItemList->push_back(item + "." + serviceName);
            i =_pItemList->getPositionOf(item + "." + serviceName);
            
            ItemInfo* itemInfo = new ItemInfo();
            //itemInfo->name = ((*_pItemList)[i]);
            itemInfo->name = item;
            itemInfo->srvName = serviceName;
            itemInfo->token = &(_pOMMProvider->generateItemToken());
            _itemMap.push_back(itemInfo);
        }
        i++;// skip directory

        _attribInfo.clear();    
        _attribInfo.setNameType(rfa::rdm::INSTRUMENT_NAME_RIC);
        _attribInfo.setName(_itemMap[i]->name);
        _attribInfo.setServiceName(_itemMap[i]->srvName);

        if(_debug) {
            cout << "[OMMCProvServer::submitData] sending refresh item: " << _itemMap[i]->name.c_str() << endl;
            cout << "[OMMCProvServer::submitData] sending refresh service: " << _itemMap[i]->srvName.c_str() << endl;
        }

        // Set RespTypeNum
        _respStatus.clear();
        _respStatus.setStreamState(RespStatus::OpenEnum);
        _respStatus.setDataState(RespStatus::OkEnum);    
        _respStatus.setStatusCode(RespStatus::NoneEnum);
        RFA_String  tmpStr("Refresh Completed");
        _respStatus.setStatusText(tmpStr);
                
        _qos.setRate(QualityOfService::tickByTick);
        _qos.setTimeliness(QualityOfService::realTime);

        // Encode header and databody
        switch(domainType) {
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

        // Reset respTypeNum to unsolicited refresh (non-interactive publishing)
        //_respMsg.setRespTypeNum(rfa::rdm::REFRESH_UNSOLICITED);
    } else if(msgType == "update") {
        //skip directory
        i++;

        /*
        * attribute is not neccessary in case of update, it uses item token
        */
        _attribInfo.clear();    
        _attribInfo.setNameType(rfa::rdm::INSTRUMENT_NAME_RIC);
        _attribInfo.setName(_itemMap[i]->name);
        _attribInfo.setServiceName(_itemMap[i]->srvName);

        if(_debug) {
            cout << "[OMMCProvServer::submitData] sending update item: " << _itemMap[i]->name.c_str() << endl;
            cout << "[OMMCProvServer::submitData] sending update service: " << _itemMap[i]->srvName.c_str() << endl;
        }

        // Encode header and databody
        switch(domainType) {
            case rfa::rdm::MMT_MARKET_PRICE:
                _encoder.encodeMarketPriceMsg(&_respMsg, RespMsg::UpdateEnum, _attribInfo,  _respStatus, &_qos, true);
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
        _log = "[OMMCProvServer::submitData] MTYPE: ";
        _log.append(msgType.c_str());
        _log.append(" not allowed. Only IMAGE or UPDATE.");
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Warning,_log.c_str());
        return;
    }
    rfa::sessionLayer::OMMItemCmd itemCmd;
    itemCmd.setMsg(static_cast<rfa::common::Msg&>(_respMsg));
    itemCmd.setItemToken(_itemMap[i]->token);
    _pOMMProvider->submit(&itemCmd);
}

void OMMCProvServer::closeSubmit(rfa::common::RFA_String item, const std::string& serviceName){
    if (item.find(RFA_String(".")) < 0) {
        item.append(".");
        item.append(serviceName.c_str());
    }
    int i = _pItemList->getPositionOf(item);
    if (i < 0)
        return;

    //skip directory
    i++;

    _respMsg.clear();
    _attribInfo.clear();    
    _respStatus.clear();

    // set attribute info
    _attribInfo.setNameType(rfa::rdm::INSTRUMENT_NAME_RIC);
    _attribInfo.setName(_itemMap[i]->name);
    _attribInfo.setServiceName(_itemMap[i]->srvName);

    // Set RespTypeNum, dataState and statusCode not neccessary (always are Suspect and None)
    _respStatus.setStreamState(RespStatus::ClosedEnum);
    //_respStatus.setDataState(RespStatus::UnspecifiedEnum);    
    //_respStatus.setStatusCode(RespStatus::NoneEnum);
    RFA_String  tmpStr("Item Closed");
    _respStatus.setStatusText(tmpStr);

    if(_debug) {
        _log = "[OMMCProvServer::closeSubmit] Close item publication for ";
        _log.append(_itemMap[i]->name.c_str());
        _log.append(" in ");
        _log.append(_itemMap[i]->srvName.c_str());
    }

    // Encode MarketPrice msg header
    _encoder.encodeMarketPriceMsg(&_respMsg, RespMsg::StatusEnum, _attribInfo,  _respStatus, NULL, true);

    //_respMsg.setPayload(_dataBody);

    rfa::sessionLayer::OMMItemCmd itemCmd;
    itemCmd.setMsg(static_cast<rfa::common::Msg&>(_respMsg));
    itemCmd.setItemToken(_itemMap[i]->token);
    _pOMMProvider->submit(&itemCmd);

    // remove item in itemList
    _pItemList->removeValue(item);
    _itemMap.removePosition(i);

    if(_debug) {
        _log += ", ItemList size is now: ";
        _log.append(_pItemList->size());
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
    }
}

void OMMCProvServer::closeAllSubmit(const rfa::common::RFA_String& serviceName){
    if (serviceName.empty()) {
        while(_pItemList->size() > 0) {
            closeSubmit((*_pItemList)[0]);
        }
    } else {
        RFA_Vector<rfa::common::RFA_String> toDelete;
        for(size_t i = 0; i < _pItemList->size(); i++) {
            if (((*_pItemList)[i]).find(serviceName) >= 0) {
                toDelete.push_back((*_pItemList)[i]);
            }
        }
        for(size_t i = 0; i < toDelete.size(); i++) {
            closeSubmit(toDelete[i]);
        }
    }
}

void OMMCProvServer::serviceStateSubmit(int serviceState, rfa::common::RFA_String& serviceName) {
    _respMsg.clear();
    _map.clear();
    if ( _pLoginHandle )
        _map.setAssociatedMetaInfo(*_pLoginHandle);
    
    //const UInt8 domainType = rfa::rdm::MMT_DIRECTORY;
    
    // Encode msg header
    _attribInfo.clear();
    _attribInfo.setDataMask(SERVICE_INFO_FILTER | SERVICE_STATE_FILTER);

    if(_itemMap.empty()) {
        // directory refresh
        ItemInfo *itemInfo = new ItemInfo();
        itemInfo->name = serviceName;
        itemInfo->srvName = serviceName;
        itemInfo->token = &(_pOMMProvider->generateItemToken());
        _itemMap.push_back(itemInfo);
    }
    // Set RespTypeNum
    _respStatus.clear();
    _respStatus.setStreamState(RespStatus::OpenEnum);
    _respStatus.setDataState(RespStatus::SuspectEnum);    
    _respStatus.setStatusCode(RespStatus::NoneEnum);
    RFA_String  tmpStr("Service state updated");
    _respStatus.setStatusText(tmpStr);

    _encoder.encodeDirectoryMsg(&_respMsg, _attribInfo, _respStatus, false);

    // Reset respTypeNum to unsolicited refresh
    //_respMsg.setRespTypeNum(rfa::rdm::REFRESH_UNSOLICITED);

    // Encode Data Body 
    _qos.setRate(QualityOfService::realTime);
    _qos.setTimeliness(QualityOfService::tickByTick);

    _encoder.encodeDirectoryDataBody(&_map,serviceName,_vendorName, serviceState, &_qos);    
    _respMsg.setPayload(_map);

    _log = "[OMMCProvServer::serviceStateSubmit] Updating directory with service state: ";
    _log.append(serviceState);
    _log.append(" for service ");
    _log.append(serviceName);
    _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
    
    rfa::sessionLayer::OMMItemCmd itemCmd;
    itemCmd.setMsg(static_cast<rfa::common::Msg&>(_respMsg));
    itemCmd.setItemToken(_itemMap[0]->token);
    _pOMMProvider->submit(&itemCmd);
}

void OMMCProvServer::staleSubmit(rfa::common::RFA_String item, const std::string& serviceName){
    if (item.find(RFA_String(".")) < 0) {
        item.append(".");
        item.append(serviceName.c_str());
    }
    int i = _pItemList->getPositionOf(item);
    if (i < 0)
        return;

    //skip directory
    i++;

    _respMsg.clear();
    _attribInfo.clear();    
    _respStatus.clear();

    // set attribute info
    _attribInfo.setNameType(rfa::rdm::INSTRUMENT_NAME_RIC);
    _attribInfo.setName(_itemMap[i]->name);
    _attribInfo.setServiceName(_itemMap[i]->srvName);

    // Set RespTypeNum
    _respStatus.setStreamState(RespStatus::OpenEnum);
    _respStatus.setDataState(RespStatus::SuspectEnum);    
    //_respStatus.setStatusCode(RespStatus::NoneEnum);
    RFA_String  tmpStr("Item Stale");
    _respStatus.setStatusText(tmpStr);

    if(_debug) {
        _log = "[OMMCProvServer::staleSubmit] Stale item publication for ";
        _log.append(_itemMap[i]->name.c_str());
        _log.append(" in ");
        _log.append(_itemMap[i]->srvName.c_str());
    }

    // Encode MarketPrice msg header
    _encoder.encodeMarketPriceMsg(&_respMsg, RespMsg::StatusEnum, _attribInfo,  _respStatus, NULL, true);

    //_respMsg.setPayload(_dataBody);

    rfa::sessionLayer::OMMItemCmd itemCmd;
    itemCmd.setMsg(static_cast<rfa::common::Msg&>(_respMsg));
    itemCmd.setItemToken(_itemMap[i]->token);
    _pOMMProvider->submit(&itemCmd);
    
    if(_debug) {
        _log += ", ItemList size is now: ";
        _log.append(_pItemList->size());
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
    }
}

void OMMCProvServer::staleAllSubmit(const rfa::common::RFA_String& serviceName){
    if (serviceName.empty()) {
        for(size_t i = 0; i < _pItemList->size(); i++) {
            staleSubmit((*_pItemList)[i]);
        }
    } else {
        RFA_Vector<rfa::common::RFA_String> toStale;
        for(size_t i = 0; i < _pItemList->size(); i++) {
            if (((*_pItemList)[i]).find(serviceName) >= 0) {
                toStale.push_back((*_pItemList)[i]);
            }
        }
        for(size_t i = 0; i < toStale.size(); i++) {
            staleSubmit(toStale[i]);
        }
    }
}

void OMMCProvServer::setDebugMode(const bool &debug) {
    _debug = debug;
    _encoder.setDebugMode(_debug);
}
