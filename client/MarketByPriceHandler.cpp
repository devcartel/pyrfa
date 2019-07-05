#include "common/RDMUtils.h"
#include "common/RDMDict.h"

#include "MarketByPriceHandler.h"
#include <boost/algorithm/string.hpp>

using namespace rfa::data;

MarketByPriceHandler::MarketByPriceHandler(rfa::sessionLayer::OMMConsumer* pOMMConsumer,
                                       rfa::common::EventQueue& eventQueue,
                                       rfa::common::Client& client,
                                       const std::string& serviceName,
                                       const RDMFieldDict* dict,
                                       rfa::logger::ComponentLogger& componentLogger):
_pOMMConsumer(pOMMConsumer),
_componentLogger(componentLogger),
_eventQueue(eventQueue),
_client(client),
_serviceName(serviceName),
_pDict(dict),
_debug(false),
_refreshCount(0),
_log("")
{
}


MarketByPriceHandler::~MarketByPriceHandler(void){}


void MarketByPriceHandler::sendRequest(const std::string &itemName, const rfa::common::UInt8 &interactionType){
    rfa::message::ReqMsg reqMsg;
    rfa::message::AttribInfo attribInfo(true) ;

    //
    // request _itemName from _serviceName
    //
    attribInfo.setName(itemName.c_str());
    attribInfo.setNameType(rfa::rdm::INSTRUMENT_NAME_RIC);
    attribInfo.setServiceName(_serviceName.c_str());
    reqMsg.setAttribInfo(attribInfo);

    reqMsg.setMsgModelType(rfa::rdm::MMT_MARKET_BY_PRICE);
    reqMsg.setInteractionType( interactionType );

    rfa::sessionLayer::OMMItemIntSpec intSpec;
    intSpec.setMsg( &reqMsg );

    std::map<rfa::common::Handle*,std::string>::iterator it;
    std::pair<map<rfa::common::Handle*,std::string>::iterator,bool> ret;

    it = _watchList.find(getHandle(itemName));

    // If item already exists, re-issue the request
    if(it == _watchList.end()) {
        _pHandle = _pOMMConsumer->registerClient(&_eventQueue, &intSpec, _client);
        if(interactionType == (rfa::message::ReqMsg::InitialImageFlag | rfa::message::ReqMsg::InterestAfterRefreshFlag)) {
            ret = _watchList.insert(  pair<rfa::common::Handle*,std::string>(_pHandle,itemName+"."+_serviceName) );
            if(ret.second) {
                if(_debug) {
                    _log = "[MarketByPriceHandler::sendRequest] Add item subscription for: ";
                    _log.append((itemName+"."+_serviceName).c_str());
                }
            } else {
                _log = "[MarketByPriceHandler::sendRequest] Watchlist insertion failed.";
                _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error,_log.c_str());
                return;
            }
            if(_debug) {
                _log += ". Watchlist size: ";
                _log.append((int)_watchList.size());
                _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
            }
        }
    } else {
        if(_debug) {
            _log = "[MarketByPriceHandler::sendRequest] Item is already in the watchlist. Re-issuing for: ";
            _log.append((itemName+"."+_serviceName).c_str());
            _log += ". Watchlist size: ";
            _log.append((int)_watchList.size());
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
        }
        _pOMMConsumer->reissueClient(it->first, &intSpec);
    }
}

void MarketByPriceHandler::closeRequest(const std::string &itemName){
    std::map<rfa::common::Handle*,std::string>::iterator it;
    it = _watchList.find(getHandle(itemName));
    if(it != _watchList.end()) {
        if(_debug) {
            _log = "[MarketByPriceHandler::closeRequest] Close item subscription for: ";
            _log.append(it->second.c_str());
        }
        _pOMMConsumer->unregisterClient(it->first);
        _watchList.erase(it);
        if(_debug) {
            _log += ". Watchlist size: ";
            _log.append((int)_watchList.size());
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
        }
    }
}

void MarketByPriceHandler::closeAllRequest(){
    _pOMMConsumer->unregisterClient();
    _watchList.clear();
    if(_debug) {
        _log = "[MarketByPriceHandler::closeAllRequest] Close all item subscription.";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
    }
}

void MarketByPriceHandler::processResponse(const rfa::message::RespMsg& respMsg, rfa::common::Handle* handle, boost::python::tuple& out) {
    std::string itemName = "";
    std::string itemServiceName = "";
    itemName = getItemName(handle);
    if (itemName.empty()) {
        itemName = respMsg.getAttribInfo().getName().c_str();
    }
    itemServiceName = getItemServiceName(handle);
    if (itemServiceName.empty()) {
        itemServiceName = _serviceName;
    }

    switch (respMsg.getRespType()){
        case rfa::message::RespMsg::RefreshEnum:
            if(_debug)
                cout << "[MarketByPriceHandler::processResponse] MarketByPrice Refresh: " << itemName << "." << itemServiceName << endl;

            // Notify that this is a refresh
            if(_refreshCount == 0) {
                dict preempt;
                preempt["RIC"] = itemName;
                preempt["SERVICE"] = itemServiceName;
                preempt["MTYPE"] = "REFRESH";
                out += boost::python::make_tuple(preempt);
            }

            if (respMsg.getHintMask() & rfa::message::RespMsg::PayloadFlag) {
                decodeMarketByPrice(respMsg.getPayload(), out, itemName, itemServiceName, "IMAGE");
            } else {
                if(_debug) {
                    _log = "[MarketByPriceHandler::processResponse] Empty Refresh.";
                    _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
                }
            }

            // if refresh complete
            if(respMsg.getIndicationMask() & rfa::message::RespMsg::RefreshCompleteFlag) {
                if(_debug) {
                    _log = "[MarketByPriceHandler::processResponse] Refresh Complete \n";
                    _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
                }
                _refreshCount = 0;
            } else {
                _refreshCount++;
            }
            break;

        case rfa::message::RespMsg::UpdateEnum:
            if(_debug)
                cout << "[MarketByPriceHandler::processResponse] MarketByPrice Update: " << itemName << "." << itemServiceName << endl;

            if (respMsg.getHintMask() & rfa::message::RespMsg::PayloadFlag) {
                decodeMarketByPrice(respMsg.getPayload(), out, itemName, itemServiceName, "UPDATE");
            } else {
                if(_debug) {
                    _log = "[MarketByPriceHandler::processResponse] Empty Update.";
                    _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
                }
            }
            break;

        case rfa::message::RespMsg::StatusEnum:
            dict d;
            d["RIC"] = itemName;
            d["SERVICE"] = itemServiceName;
            d["MTYPE"] = "STATUS";
            d["TEXT"] = respMsg.getRespStatus().getStatusText().c_str();
            d["DATA_STATE"] = RDMUtils::dataStateToString(respMsg.getRespStatus().getDataState()).c_str();
            d["STREAM_STATE"] = RDMUtils::streamStateToString(respMsg.getRespStatus().getStreamState()).c_str();
            d["STATUS_CODE"] = RDMUtils::statusCodeToString(respMsg.getRespStatus().getStatusCode()).c_str();
            out += boost::python::make_tuple(d);
            if(_debug)
                cout << "[MarketByPriceHandler::processResponse] MarketByPrice Status: "<< respMsg.getRespStatus().getStatusText().c_str() << endl;
            _log = "[MarketByPriceHandler::processResponse] MarketByPrice Status: " + respMsg.getRespStatus().getStatusText();
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Warning,_log.c_str());
            break;
    }

    // display subscription status
    if (respMsg.getHintMask() & rfa::message::RespMsg::RespStatusFlag) {
        const rfa::common::RespStatus& status = respMsg.getRespStatus();

        // item status details
        _log = " \n\tStatus :";
        _log += " \n\tdataState=\"";
        _log.append(RDMUtils::dataStateToString(status.getDataState()).c_str());
        _log += "\"";
        _log += " \n\tstreamState=\"";
        _log.append(RDMUtils::streamStateToString(status.getStreamState()).c_str());
        _log += "\" \n\tstatusCode=\"";
        _log.append(RDMUtils::statusCodeToString(status.getStatusCode()).c_str());
        _log += "\" \n\tstatusText=\"";
        _log += status.getStatusText();
        _log += "\"";

        // close item if streamState is closed
        if(status.getStreamState() == rfa::common::RespStatus::ClosedEnum) {
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error,_log.c_str());
            closeRequest(itemName);
        }

        // Unspecified dataState
        if(status.getDataState() == rfa::common::RespStatus::UnspecifiedEnum) {
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error,_log.c_str());
        }
    }

    if(_debug && (out != boost::python::tuple()))
        prettyPrint(out);
}

void MarketByPriceHandler::prettyPrint(boost::python::tuple& inputTuple) {
    string out = "";
    out.append("(");
    for ( int i = 0 ; i < len(inputTuple) ; i++ ) {
        extract<dict>dictElement(inputTuple[i]);
        //check whether the converted data is python dictionary
        if (dictElement.check()) {
            out.append("{");
            dict dictElement = extract<dict>(inputTuple[i]);
            boost::python::list keys = (boost::python::list)dictElement.keys();

            for (int j = 0; j < len(keys); j++) {
                string key = extract<string>(keys[j]);
                string value = "";

                //string
                extract<string>toString(dictElement[keys[j]]);
                if(toString.check()) {
                    value = extract<string>(dictElement[keys[j]]);
                    value = "'" + value + "'";
                }
                //double
                extract<double>toDouble(dictElement[keys[j]]);
                if(toDouble.check()) {
                    double edouble = extract<double>(dictElement[keys[j]]);
                    value = boost::lexical_cast<string>(edouble);
                }
                //int
                extract<int>toInt(dictElement[keys[j]]);
                if(toInt.check()) {
                    int eint = extract<int>(dictElement[keys[j]]);
                    value = boost::lexical_cast<string>(eint);
                }
                //long
                extract<long>toLong(dictElement[keys[j]]);
                if(toLong.check()) {
                    value = extract<string>(str(dictElement[keys[j]]));
                }

                out.append("'"+key+"'"+":"+value);
                if (j != len(keys) - 1) {
                    out.append(",");
                }
            }
            out.append("}");
            if (i != len(inputTuple) - 1) {
                out.append(",");
            }
        }
    }
    out.append(")");
    cout << out << endl;
}

void MarketByPriceHandler::decodeMarketByPrice(const rfa::common::Data& data, boost::python::tuple &out, const std::string &itemName, const std::string &serviceName, const std::string &mtype) {
    dict d;
    const rfa::data::Map& mapData = static_cast<const rfa::data::Map&>(data);
    // data is RFA update message
    if(mapData.getIndicationMask() & rfa::data::Map::EntriesFlag) {
        rfa::data::MapReadIterator mri;
        mri.start(mapData);
        const rfa::data::MapEntry & entryInit = mri.value();
        const rfa::common::Data & valueDataInit = entryInit.getData();
        const rfa::data::FieldList & EntryFieldList = static_cast<const rfa::data::FieldList &>(valueDataInit);
        for(mri.start(mapData); !mri.off(); mri.forth()) {
            const rfa::data::MapEntry & entry = mri.value();
            const rfa::common::Data & keyData = entry.getKeyData();
            const rfa::common::Data & valueData = entry.getData();
            if (keyData.getDataType() != rfa::data::DataBufferEnum) {
                cout << "[MarketByPriceHandler::decodeMarketByPrice] Expected key datatype of DataBuffer" << endl;
                return;
            }
            const rfa::data::DataBuffer & keyBuffer = static_cast<const rfa::data::DataBuffer &>(keyData);
            if (keyBuffer.getDataBufferType() != rfa::data::DataBuffer::BufferEnum){
                cout << "[MarketByPriceHandler::decodeMarketByPrice] Expected key buffer data buffer type of Buffer" << endl;
                return;
            }

            rfa::data::FieldListReadIterator flri;
            switch (entry.getAction()) {
                case rfa::data::MapEntry::Add:
                    d["SERVICE"] = serviceName.c_str();
                    d["RIC"] = itemName.c_str();
                    d["MTYPE"]= mtype.c_str();
                    d["ACTION"] = "ADD";
                    d["KEY"] = static_cast<const rfa::data::DataBuffer&> (keyData).getAsString().c_str();

                    for(flri.start(EntryFieldList); !flri.off(); flri.forth()) {
                        const rfa::data::FieldEntry& field = flri.value();
                        const rfa::common::Int16 fieldID = field.getFieldID();
                        const RDMFieldDef* fieldDef = _pDict->getFieldDef(fieldID);
                        rfa::common::RFA_String fieldValue;
                        // if field name does not exist, use the field ID instead
                        // if no field definition then no dict then no dataType
                        if(!fieldDef) {
                            const rfa::common::UInt8 dataType = (rfa::common::UInt8)rfa::data::DataBuffer::UnknownDataBufferEnum;
                            const rfa::common::Data& fieldData = field.getData(dataType);
                            const rfa::data::DataBuffer& dataBuffer = static_cast<const rfa::data::DataBuffer&>(fieldData);
                            rfa::common::RFA_String fieldValue;
                            fieldValue = RDMUtils::dataBufferToString(dataBuffer).c_str();
                            d[fieldID] = fieldValue.trimWhitespace().c_str();

                        } else {
                            // convert to DataBuffer
                            const rfa::common::UInt8 dataType = fieldDef->getDataType();
                            const rfa::common::Data& fieldData = field.getData(dataType);
                            const rfa::data::DataBuffer& dataBuffer = static_cast<const rfa::data::DataBuffer&>(fieldData);
                            const rfa::common::UInt8 dataBufferType = dataBuffer.getDataBufferType();
                            const RDMEnumDef* enumDef = 0;

                            //Note: somehow RDNDISPLAY has dataType=2 but dataBufferType=4
                            //check and decode according to data buffer type
                            switch(dataBufferType) {
                                case rfa::data::DataBuffer::EnumerationEnum:
                                    enumDef = fieldDef->getEnumDef();
                                    fieldValue = RDMUtils::dataBufferToString(dataBuffer,enumDef).c_str();
                                    d[fieldDef->getName().c_str()] = fieldValue.c_str();
                                    break;
                                case rfa::data::DataBuffer::FloatEnum:
                                case rfa::data::DataBuffer::DoubleEnum:
                                case rfa::data::DataBuffer::Real32Enum:
                                case rfa::data::DataBuffer::Real64Enum:
                                    fieldValue = RDMUtils::dataBufferToString(dataBuffer).c_str();
                                    if(fieldValue.empty()) {
                                        //d[fieldDef->getName().c_str()] = boost::python::object();
                                        d[fieldDef->getName().c_str()] = "";
                                    } else {
                                        d[fieldDef->getName().c_str()] = RDMUtils::dataBufferToDouble(dataBuffer);
                                    }
                                    break;
                                case rfa::data::DataBuffer::Int32Enum:
                                case rfa::data::DataBuffer::UInt32Enum:
                                    fieldValue = RDMUtils::dataBufferToString(dataBuffer).c_str();
                                    if(fieldValue.empty()) {
                                        d[fieldDef->getName().c_str()] = "";
                                    } else {
                                        d[fieldDef->getName().c_str()] = RDMUtils::dataBufferToInt(dataBuffer);
                                    }
                                    break;
                                case rfa::data::DataBuffer::Int64Enum:
                                case rfa::data::DataBuffer::UInt64Enum:
                                    fieldValue = RDMUtils::dataBufferToString(dataBuffer).c_str();
                                    if(fieldValue.empty()) {
                                        d[fieldDef->getName().c_str()] = "";
                                    } else {
                                        d[fieldDef->getName().c_str()] = RDMUtils::dataBufferToLong(dataBuffer);
                                    }
                                    break;
                                default:
                                    fieldValue = RDMUtils::dataBufferToString(dataBuffer).c_str();
                                    d[fieldDef->getName().c_str()] = fieldValue.trimWhitespace().c_str();
                                    break;
                            }
                        }
                    }
                    break;

                case rfa::data::MapEntry::Update:
                    if (valueData.getDataType() != rfa::data::FieldListEnum) {
                        cout << "[MarketByPriceHandler::decodeMarketByPrice] Expected data datatype of FieldList" << endl;
                        return;
                    }
                    d["SERVICE"] = serviceName.c_str();
                    d["RIC"] = itemName.c_str();
                    d["MTYPE"]= mtype.c_str();
                    d["ACTION"] = "UPDATE";
                    d["KEY"] = static_cast<const rfa::data::DataBuffer&> (keyData).getAsString().c_str();
                    for(flri.start(EntryFieldList); !flri.off(); flri.forth()) {
                        const rfa::data::FieldEntry& field = flri.value();
                        const rfa::common::Int16 fieldID = field.getFieldID();
                        const RDMFieldDef* fieldDef = _pDict->getFieldDef(fieldID);
                        rfa::common::RFA_String fieldValue;

                        // if field name does not exist, use the field ID instead
                        // if no field definition then no dict then no dataType
                        if(!fieldDef) {
                            const rfa::common::UInt8 dataType = (rfa::common::UInt8)rfa::data::DataBuffer::UnknownDataBufferEnum;
                            const rfa::common::Data& fieldData = field.getData(dataType);
                            const rfa::data::DataBuffer& dataBuffer = static_cast<const rfa::data::DataBuffer&>(fieldData);
                            rfa::common::RFA_String fieldValue;
                            fieldValue = RDMUtils::dataBufferToString(dataBuffer).c_str();
                            d[fieldID] = fieldValue.trimWhitespace().c_str();

                        } else {
                        // convert to DataBuffer
                        const rfa::common::UInt8 dataType = fieldDef->getDataType();
                        const rfa::common::Data& fieldData = field.getData(dataType);
                        const rfa::data::DataBuffer& dataBuffer = static_cast<const rfa::data::DataBuffer&>(fieldData);
                        const rfa::common::UInt8 dataBufferType = dataBuffer.getDataBufferType();
                        const RDMEnumDef* enumDef = 0;

                        //Note: somehow RDNDISPLAY has dataType=2 but dataBufferType=4
                        //check and decode according to data buffer type
                        switch(dataBufferType) {
                            case rfa::data::DataBuffer::EnumerationEnum:
                                enumDef = fieldDef->getEnumDef();
                                fieldValue = RDMUtils::dataBufferToString(dataBuffer,enumDef).c_str();
                                d[fieldDef->getName().c_str()] = fieldValue.c_str();
                                break;
                            case rfa::data::DataBuffer::FloatEnum:
                            case rfa::data::DataBuffer::DoubleEnum:
                            case rfa::data::DataBuffer::Real32Enum:
                            case rfa::data::DataBuffer::Real64Enum:
                                d[fieldDef->getName().c_str()] = RDMUtils::dataBufferToDouble(dataBuffer);
                                break;
                            case rfa::data::DataBuffer::Int32Enum:
                            case rfa::data::DataBuffer::UInt32Enum:
                                d[fieldDef->getName().c_str()] = RDMUtils::dataBufferToInt(dataBuffer);
                                break;
                            case rfa::data::DataBuffer::Int64Enum:
                            case rfa::data::DataBuffer::UInt64Enum:
                                d[fieldDef->getName().c_str()] = RDMUtils::dataBufferToLong(dataBuffer);
                            break;
                            default:
                                fieldValue = RDMUtils::dataBufferToString(dataBuffer).c_str();
                                d[fieldDef->getName().c_str()] = fieldValue.trimWhitespace().c_str();
                                break;
                            }

                        }
                    }

                    break;

                case rfa::data::MapEntry::Delete:
                    d["SERVICE"] = serviceName.c_str();
                    d["RIC"] = itemName.c_str();
                    d["MTYPE"]= mtype.c_str();
                    d["ACTION"] = "DELETE";
                    d["KEY"] = static_cast<const rfa::data::DataBuffer&> (keyData).getAsString().c_str();
                    break;
            }
            out += boost::python::make_tuple(d);
            d = dict();
        }
    }
}

void MarketByPriceHandler::setDebugMode(const bool &debug) {
    _debug = debug;
}

std::string MarketByPriceHandler::getItemName(rfa::common::Handle* handle) {
    std::map<rfa::common::Handle*,std::string>::iterator it;
    it = _watchList.find(handle);
    std::string itemName = "";
    if(it != _watchList.end()) {
        std::vector<std::string> strs;
        boost::split(strs, it->second, boost::is_any_of("."));
        if (strs.size() > 2) {
            std::vector<std::string>::iterator strsit = strs.begin();
            itemName = *strsit;
            strsit++;
            for (std::vector<std::string>::size_type i=1; i < strs.size()-1; i++) {
                itemName = itemName + "." + *strsit;
                strsit++;
            }
        } else {
            itemName = strs[0];
        }
    }
    return itemName;
}

std::string MarketByPriceHandler::getItemServiceName(rfa::common::Handle* handle) {
    std::map<rfa::common::Handle*,std::string>::iterator it;
    it = _watchList.find(handle);
    std::string itemServiceName = "";
    if(it != _watchList.end()) {
        std::vector<std::string> strs;
        boost::split(strs, it->second, boost::is_any_of("."));
        itemServiceName = strs.back();
    }
    return itemServiceName;
}

rfa::common::Handle* MarketByPriceHandler::getHandle(const std::string &itemName) {
    std::map<rfa::common::Handle*,std::string>::iterator it;
    for(it = _watchList.begin(); it != _watchList.end(); it++) {
        if(it->second == itemName+"."+_serviceName) {
            return it->first;
        }
    }
    return NULL;
}

std::map<rfa::common::Handle*,std::string> &MarketByPriceHandler::getWatchList() {
    return _watchList;
}
