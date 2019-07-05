#include "common/RDMUtils.h"
#include "common/RDMDict.h"

#include "TimeSeriesHandler.h"
#include <boost/algorithm/string.hpp>

using namespace rfa::data;

TimeSeriesHandler::TimeSeriesHandler(rfa::sessionLayer::OMMConsumer* pOMMConsumer,
                                       rfa::common::EventQueue& eventQueue,
                                       rfa::common::Client& client,
                                       const std::string& serviceName,
                                       const RDMFieldDict* dict,
                                       rfa::logger::ComponentLogger& componentLogger,
                                       rfa::ts1::TS1DefDb& TS1DictDb):
_pOMMConsumer(pOMMConsumer),
_componentLogger(componentLogger),
_eventQueue(eventQueue),
_client(client),
_serviceName(serviceName),
_pDict(dict),
_debug(false),
_TS1DictDb(TS1DictDb),
_pTimeSeries(0),
_timeSeries(0),
_numberOfDictParsed(0),
_numberOfTimeSeriesRefreshParsed(0),
_isTimeSeriesParseComplete(true),
_log("")
{
}


TimeSeriesHandler::~TimeSeriesHandler(void){}


void TimeSeriesHandler::sendRequest(const std::string &itemName){
    rfa::message::ReqMsg reqMsg;
    rfa::message::AttribInfo attribInfo(true) ;

    //
    // request _itemName from _serviceName
    //
    attribInfo.setName(itemName.c_str());
    attribInfo.setNameType(rfa::rdm::INSTRUMENT_NAME_RIC);
    attribInfo.setServiceName(_serviceName.c_str());
    reqMsg.setAttribInfo(attribInfo);

    reqMsg.setMsgModelType(rfa::rdm::MMT_MARKET_PRICE);
    reqMsg.setInteractionType( rfa::message::ReqMsg::InitialImageFlag );

    rfa::sessionLayer::OMMItemIntSpec intSpec;
    intSpec.setMsg( &reqMsg );

    std::map<rfa::common::Handle*,std::string>::iterator it;
    std::pair<map<rfa::common::Handle*,std::string>::iterator,bool> ret;

    it = _watchList.find(getHandle(itemName));

    // If item already exists, re-issue the request
    if(it == _watchList.end()) {
        _pHandle = _pOMMConsumer->registerClient(&_eventQueue, &intSpec, _client);
        ret = _watchList.insert(  pair<rfa::common::Handle*,std::string>(_pHandle,itemName+"."+_serviceName) );
        if(ret.second) {
            if(_debug) {
                _log = "[TimeSeriesHandler::sendRequest] Add item subscription for: ";
                _log.append((itemName+"."+_serviceName).c_str());
            }
        } else {
            _log = "[TimeSeriesHandler::sendRequest] Watchlist insertion failed.";
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error,_log.c_str());
            _isTimeSeriesParseComplete = true;
            return;
        }
        if(_debug) {
            _log += ". Watchlist size: ";
            _log.append((int)_watchList.size());
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
        }
    } else {
        if(_debug) {
            _log = "[TimeSeriesHandler::sendRequest] Item is already in the watchlist. Re-issuing for: ";
            _log.append((itemName+"."+_serviceName).c_str());
            _log += ". Watchlist size: ";
            _log.append((int)_watchList.size());
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
        }
        _pOMMConsumer->reissueClient(it->first, &intSpec);
    }
}

void TimeSeriesHandler::closeRequest(const std::string &itemName){
    std::map<rfa::common::Handle*,std::string>::iterator it;
    it = _watchList.find(getHandle(itemName));
    if(it != _watchList.end()) {
        if(_debug) {
            _log = "[TimeSeriesHandler::closeRequest] Close item subscription for: ";
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
    _isTimeSeriesParseComplete = true;
    _numberOfTimeSeriesRefreshParsed = 0;
}

void TimeSeriesHandler::closeAllRequest(){
    _pOMMConsumer->unregisterClient();
    _watchList.clear();
    if(_debug) {
        _log = "[TimeSeriesHandler::closeAllRequest] Close all item subscription.";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
    }
    _isTimeSeriesParseComplete = true;
    _numberOfTimeSeriesRefreshParsed = 0;
}

void TimeSeriesHandler::processResponse( const rfa::message::RespMsg& respMsg, rfa::common::Handle* handle, boost::python::tuple& out){
    dict d;
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
                cout << "[TimeSeriesHandler::processResponse] TimeSeries Refresh: " << itemName << "." << itemServiceName << endl;

            if (respMsg.getHintMask() & rfa::message::RespMsg::PayloadFlag) {
                if (_TS1DictDb.hasTS1DbRics(respMsg.getAttribInfo().getName().c_str())) {
                    decodeMarketPrice(respMsg.getPayload(), d);
                    _numberOfDictParsed++;

                } else {
                    decodeTimeSeries(respMsg, d);
                    _numberOfTimeSeriesRefreshParsed++;
                }
                out += boost::python::make_tuple(respMsg.getAttribInfo().getServiceName().c_str(), respMsg.getAttribInfo().getName().c_str(), boost::python::make_tuple(d));
            } else {
                if(_debug) {
                    _log = "[TimeSeriesHandler::processResponse] Empty Refresh.";
                    _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
                }
                _isTimeSeriesParseComplete = true;
            }
            break;

        case rfa::message::RespMsg::UpdateEnum:
            if(_debug)
                cout << "[TimeSeriesHandler::processResponse] TimeSeries Update: " << itemName << "." << itemServiceName << endl;

            if (respMsg.getHintMask() & rfa::message::RespMsg::PayloadFlag) {
                if (_TS1DictDb.hasTS1DbRics(respMsg.getAttribInfo().getName().c_str())) {
                    decodeMarketPrice(respMsg.getPayload(), d);
                } else {
                    decodeTimeSeries(respMsg, d);
                }
            } else {
                if(_debug) {
                    _log = "[TimeSeriesHandler::processResponse] Empty Update.";
                    _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
                }
                _isTimeSeriesParseComplete = true;
            }
            out += boost::python::make_tuple(respMsg.getAttribInfo().getServiceName().c_str(),respMsg.getAttribInfo().getName().c_str(), boost::python::make_tuple(d));
            break;

        case rfa::message::RespMsg::StatusEnum:
            if(_debug)
                cout << "[TimeSeriesHandler::processResponse] TimeSeries Status: "<< respMsg.getRespStatus().getStatusText().c_str() << endl;
            _log = "[TimeSeriesHandler::processResponse] TimeSeries Status: " + respMsg.getRespStatus().getStatusText();
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

void TimeSeriesHandler::prettyPrint(boost::python::tuple& inputTuple) {
    bool _isFirstElement = true;
    string out = "";
    out.append("(");
    for ( int i = 0 ; i < len(inputTuple) ; i++ ) {
        //cout << "out = " << out;
        if (_isFirstElement) {
            out.append("(");
            _isFirstElement = false;
        }
        extract<string>strElement(inputTuple[i]);

        if (!strElement.check()) {
            boost::python::tuple tupElement = extract<boost::python::tuple>(inputTuple[i]);
            out.append("(");

            for ( int ii = 0 ; ii < len(tupElement) ; ii++ ) {
                extract<string>nestedstrElement(tupElement[ii]);
                //check whether the converted data is python dictionary
                if (!nestedstrElement.check()) {
                    out.append("{");
                    dict dictElement = extract<dict>(tupElement[ii]);
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
        } else {
            out.append("'");
            out.append(nestedstrElement);
            out.append("'");
        }
        if (ii != len(tupElement) - 1) {
            out.append(",");
        }
                }
            out.append("))");
            _isFirstElement = true;
        } else {
            out.append("'");
            out.append(strElement);
            out.append("'");
        }
        //check whether the converted data is python dictionary

        if (i != len(inputTuple) - 1) {
            out.append(",");
        }
    }
    out.append(")");
    cout << out << endl;;
}

void TimeSeriesHandler::decodeMarketPrice(const rfa::common::Data& data, dict &d){

    // iterate thru field entries
    const rfa::data::FieldList& fieldlist = static_cast<const rfa::data::FieldList&>(data);

    rfa::data::FieldListReadIterator flri;
    for (flri.start(fieldlist); !flri.off(); flri.forth())  {

        const rfa::data::FieldEntry& field = flri.value();
        const rfa::common::Int16 fieldID = field.getFieldID();
        const RDMFieldDef* fieldDef = _pDict->getFieldDef(fieldID);
        rfa::common::RFA_String fieldValue;

        // if no field definition then no dict then no dataType
        if(!fieldDef) {
            const rfa::common::UInt8 dataType = (rfa::common::UInt8)rfa::data::DataBuffer::UnknownDataBufferEnum;
            const rfa::common::Data& fieldData = field.getData(dataType);
            const rfa::data::DataBuffer& dataBuffer = static_cast<const rfa::data::DataBuffer&>(fieldData);
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


            // The "_decoder" stores 14 pages of data, out of which 2 thru 10 indexes
            // correspond to ROW64_3 thru ROW64_11 each defining a TS1Def.
            // or get 'ROW64_12' that has next page code that needs to be requested:
            if( fieldID >= (_pDict->getFieldDef(RFA_String("ROW64_3", 0, false)))->getFieldId() &&  fieldID <= (_pDict->getFieldDef(RFA_String("ROW64_11", 0, false)))->getFieldId() ) {
                _TS1DictDb.add(fieldValue.c_str());
            } else if( fieldID == (_pDict->getFieldDef(RFA_String("ROW64_12", 0, false)))->getFieldId() ) {
                RFA_String nextStr = fieldValue;
                const char *next = nextStr.c_str();
                int len = nextStr.length();
                char nextRic[65];
                int j = len - 1;
                int n = 0; // Remove '+' and Spaces.
                for(; j >= 0; j--)
                {
                    if(next[j] != ' ' )
                        break;
                }
                n = 0;
                int i = 0;
                for(; i <= j; i++)
                {
                    if(next[i] != '+')
                    {
                        nextRic[n] = next[i];
                        n++;
                    }
                }
                nextRic[n] = '\0';

                // if addDbRics is successful (returns TRUE), meaning it wasn't in TS1DictDb
                // then send a subscription request on that db ric.
                if(_TS1DictDb.addDbRics(nextRic)) {
                    sendRequest(nextRic);
                }
            }
        }

}

void TimeSeriesHandler::decodeTimeSeries(const rfa::message::RespMsg& respMsg, dict &d) {
    rfa::common::RFA_String timeSeriesData("", 0, false);

    // iterate thru field entries
    const rfa::data::FieldList& fieldlist = static_cast<const rfa::data::FieldList&>(respMsg.getPayload());

    rfa::data::FieldListReadIterator flri;
    for (flri.start(fieldlist); !flri.off(); flri.forth())  {

        const rfa::data::FieldEntry& field = flri.value();
        const rfa::common::Int16 fieldID = field.getFieldID();

        const RDMFieldDef* fieldDef = _pDict->getFieldDef(fieldID);


        // if no field definition then no dict then no dataType
        if(!fieldDef) {
            const rfa::common::UInt8 dataType = (rfa::common::UInt8)rfa::data::DataBuffer::UnknownDataBufferEnum;
            const rfa::common::Data& fieldData = field.getData(dataType);
            const rfa::data::DataBuffer& dataBuffer = static_cast<const rfa::data::DataBuffer&>(fieldData);
            rfa::common::RFA_String fieldValue;
            fieldValue = RDMUtils::dataBufferToString(dataBuffer).c_str();
            d[fieldDef->getName().c_str()] = fieldValue.trimWhitespace().c_str();
        } else {
            // convert to DataBuffer
            const rfa::common::UInt8 dataType = fieldDef->getDataType();
            const rfa::common::Data& fieldData = field.getData(dataType);
            const rfa::data::DataBuffer& dataBuffer = static_cast<const rfa::data::DataBuffer&>(fieldData);

            rfa::common::RFA_String fieldValue;

            //check if enum
            if(fieldDef->getDataType() == rfa::data::DataBuffer::EnumerationEnum) {
                const RDMEnumDef* enumDef = fieldDef->getEnumDef();
                fieldValue = RDMUtils::dataBufferToString(dataBuffer,enumDef).c_str();
            } else {
                fieldValue = RDMUtils::dataBufferToStringNoCoversion(dataBuffer).c_str();
            }

            // Filter only the ROW64_1 to ROW64_14 fields
            if( fieldID >= (_pDict->getFieldDef(RFA_String("ROW64_1", 0, false)))->getFieldId() &&  fieldID <= (_pDict->getFieldDef(RFA_String("ROW64_14", 0, false)))->getFieldId() ) {
                timeSeriesData.append(fieldValue);
                d[fieldDef->getName().c_str()] = "<encoded timeseries data>";
            } else {
                d[fieldDef->getName().c_str()] = fieldValue.trimWhitespace().c_str();
            }
        }
    }

    if(!timeSeriesData.empty()) {
        if (_pTimeSeries->decode(respMsg.getAttribInfo().getName().c_str(),timeSeriesData.c_str()) == false) {
            _log = "[TimeSeriesHandler::decodeTimeSeries] Timeseries decoding failed for ";
            _log.append(respMsg.getAttribInfo().getName().c_str());
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Error,_log.c_str());
            _isTimeSeriesParseComplete = true;
            return;
        }

        // subscribe to secondary RICs e.g. dC.Nd1, dC.Nd2,...
        // 0 = primary
        // 1...N = secondary where N is from getRicCounts
        if(respMsg.getAttribInfo().getName() == _pTimeSeries->getPrimaryRic()) {
            for(int i=1; i < _pTimeSeries->getRicCounts(); ++i) {
                sendRequest(RFA_String(_pTimeSeries->getRicName(i), 0, false).c_str());
            }
        }
    }
}

bool TimeSeriesHandler::isTimeSeriesParseComplete() {
    // determine if all TS1 data is parsed completely
    if( _numberOfDictParsed == _TS1DictDb.getNumberDbRics() && _numberOfTimeSeriesRefreshParsed == _pTimeSeries->getRicCounts() && _numberOfTimeSeriesRefreshParsed > 0 ) {
        _isTimeSeriesParseComplete = true;
    }
    return _isTimeSeriesParseComplete;
}

void TimeSeriesHandler::setDebugMode(const bool &debug) {
    _debug = debug;
}

std::string TimeSeriesHandler::getItemName(rfa::common::Handle* handle) {
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

std::string TimeSeriesHandler::getItemServiceName(rfa::common::Handle* handle) {
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


rfa::common::Handle* TimeSeriesHandler::getHandle(const std::string &itemName) {
    std::map<rfa::common::Handle*,std::string>::iterator it;
    for(it = _watchList.begin(); it != _watchList.end(); it++) {
        if(it->second == itemName+"."+_serviceName) {
            return it->first;
        }
    }
    return NULL;
}

std::map<rfa::common::Handle*,std::string> &TimeSeriesHandler::getWatchList() {
    return _watchList;
}

const std::list<std::string>* TimeSeriesHandler::getTimeSeries(const int &maxRecords) {
    rfa::common::RFA_String timeSeriesRecord;
    rfa::ts1::TS1SampleIterator iterator;

    if(_debug) {
        _log = "[TimeSeriesHandler::getTimeSeries] Timeseries parsing statistics:\n";
        _log += "Number Of DB RICs = ";
        _log.append(_TS1DictDb.getNumberDbRics());
        _log += "\n";
        _log += "Number Of RICs parsed = ";
        _log.append(_pTimeSeries->getRicCounts());
        _log += "\n";
        _log += "Number Of FIDs = ";
        _log.append(_pTimeSeries->getFactCount());
        _log += "\n";
        _log += "Number Of Samples parsed = ";
        _log.append(_pTimeSeries->getNumberOfSamples());
        _log += "\n";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
    }

    int count=0;
    struct tm * time_tm;
    char time_str[256];

    // Header record consists of FID definition
    if(_pTimeSeries->getNumberOfSamples() > 0) {
        timeSeriesRecord = "DATE";
        for (int i=0; i < _pTimeSeries->getFactCount(); i++) {
            timeSeriesRecord += ",";
            timeSeriesRecord.append( _TS1DictDb.getDefByFid(_pTimeSeries->getFact(i))->getLongName() );
        }
        _timeSeries.push_back(timeSeriesRecord.c_str());

        // Timeseries records
        for (iterator.start(_pTimeSeries); (!iterator.off() && (count++ < maxRecords)); iterator.forth()) {
            const rfa::ts1::TS1Sample* sample = iterator.getSample();

            time_t time = sample->getDate();
            time_tm = localtime(&time);
            sprintf(time_str,"%4ld/%02ld/%02ld",(long int)(1900+time_tm->tm_year),(long int)(time_tm->tm_mon+1),(long int)time_tm->tm_mday);
            timeSeriesRecord = time_str;

            if (!sample->isValid()) {
                timeSeriesRecord += ",";
                timeSeriesRecord.append(sample->text());
            } else {
                for (int i=0; i < sample->getNumberOfPoints(); i++) {
                    const rfa::ts1::TS1Point* point = (*sample)[i];
                    if (!point->isValid()) {
                        timeSeriesRecord += ",";
                        timeSeriesRecord.append(point->getText());
                    } else {
                        switch (point->getDataType()) {
                            case rfa::ts1::TS1Point::Double:
                                sprintf(time_str,"%g",point->toDouble());
                                timeSeriesRecord += ",";
                                timeSeriesRecord.append(time_str);
                                break;
                            case rfa::ts1::TS1Point::Integer:
                                sprintf(time_str,"%d",point->toInt());
                                timeSeriesRecord += ",";
                                timeSeriesRecord.append(time_str);
                                break;
                            case rfa::ts1::TS1Point::String:
                                sprintf(time_str,"%s",point->toString());
                                timeSeriesRecord += ",";
                                timeSeriesRecord.append(time_str);
                                break;
                            default:
                                timeSeriesRecord += ",";
                                timeSeriesRecord.append("datatype not supported");
                                break;
                        }
                    }
                }
            }
            _timeSeries.push_back(timeSeriesRecord.c_str());
        }
    }
    return &_timeSeries;
}

void TimeSeriesHandler::initTimeSeries(rfa::ts1::TS1Series* pTimeSeries) {
    _pTimeSeries = pTimeSeries;
    _isTimeSeriesParseComplete = false;
    _numberOfTimeSeriesRefreshParsed = 0;
    _timeSeries.clear();
}
