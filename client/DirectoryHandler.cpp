#include "StdAfx.h"
#include "common/RDMUtils.h"
#include "common/AppUtil.h"

#include "DirectoryHandler.h"
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

DirectoryHandler::DirectoryHandler(rfa::sessionLayer::OMMConsumer* pOMMConsumer, 
                                   rfa::common::EventQueue& eventQueue,
                                   rfa::common::Client& client,
                                   std::string& serviceName,
                                   rfa::logger::ComponentLogger& componentLogger):
_pOMMConsumer(pOMMConsumer),
_eventQueue(eventQueue),
_client(client),
_serviceName(serviceName),
_isServiceUp(false),
_debug(false),
_componentLogger(componentLogger),
_log("")
{
}


DirectoryHandler::~DirectoryHandler(void)
{
}

void DirectoryHandler::sendRequest(){
    rfa::message::ReqMsg reqMsg;    
    rfa::message::AttribInfo attribInfo ;

    reqMsg.setMsgModelType(rfa::rdm::MMT_DIRECTORY);
    reqMsg.setInteractionType( rfa::message::ReqMsg::InitialImageFlag | rfa::message::ReqMsg::InterestAfterRefreshFlag );
    attribInfo.setDataMask( rfa::rdm::SERVICE_INFO_FILTER | rfa::rdm::SERVICE_STATE_FILTER);
    reqMsg.setAttribInfo(attribInfo);

    rfa::sessionLayer::OMMItemIntSpec intSpec;
    intSpec.setMsg( &reqMsg );
    _pHandle = _pOMMConsumer->registerClient(&_eventQueue, &intSpec, _client);
}

void DirectoryHandler::processResponse(const rfa::message::RespMsg& respMsg, boost::python::tuple& out){

    switch (respMsg.getRespType())
    {
    case rfa::message::RespMsg::RefreshEnum:
        if(_debug) {
            _log = "[DirectoryHandler::processResponse] Directory Refresh.";
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
        }
        decodeDirectory(respMsg.getPayload(), out);
        break;
    case rfa::message::RespMsg::UpdateEnum:
        if(_debug) {
            _log = "[DirectoryHandler::processResponse] Directory Update.";
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
        }
        decodeDirectory(respMsg.getPayload(), out);
        break;
    case rfa::message::RespMsg::StatusEnum:
        _log = "[DirectoryHandler::processResponse] Directory Status.";
        break; 
    }

    // log directory information
    if(_debug) {
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
    }
    
    /*if (respMsg.getIndicationMask() & rfa::message::RespMsg::RespStatusFlag){
        const rfa::common::RespStatus& status = respMsg.getRespStatus();
        _log += " \n\tStatus :";
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
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Warning,_log.c_str());
    }*/
}

void DirectoryHandler::decodeDirectory(const rfa::common::Data& directory, boost::python::tuple& out) {
    const rfa::data::Map& map = static_cast<const rfa::data::Map&>(directory);
    rfa::data::MapReadIterator mri;
    for (mri.start(map); !mri.off(); mri.forth()) { 
        const rfa::data::MapEntry&  mapEntry = mri.value();
        const rfa::common::Data& keyData = mapEntry.getKeyData();
        const rfa::data::DataBuffer& keyDb = static_cast<const rfa::data::DataBuffer&>(keyData);
        const rfa::common::Data& data = mapEntry.getData();
        const std::string serviceName = keyDb.getAsString().c_str();
        dict d;
        if(_debug) {
            _log += "\nService: ";
            _log.append(serviceName.c_str());
            _log += "\n";
        }
        const rfa::data::FilterList& filterList = static_cast<const rfa::data::FilterList&>(data);
        decodeFilterList(filterList, serviceName, d);
        d["MTYPE"] = "DIRECTORY";
        d["SERVICE"] = serviceName;
        out += boost::python::make_tuple(d);
    }
}

bool DirectoryHandler::isServiceUp() const {
    return _isServiceUp;
}

void DirectoryHandler::decodeFilterList(const rfa::data::FilterList& filterList, const std::string& serviceName, dict &d) {
    rfa::data::FilterListReadIterator flri;
    for (flri.start(filterList); !flri.off(); flri.forth()) {
        const rfa::data::FilterEntry& filterEntry = flri.value();
        const rfa::common::Data& filterEntryData = filterEntry.getData();
        const rfa::data::ElementList& elementList = static_cast<const rfa::data::ElementList&>(filterEntryData);
        switch (filterEntry.getFilterID()){
            case rfa::rdm::SERVICE_INFO_ID:
                if(_debug)
                    _log += "Service Info: \n";
                break;
            case rfa::rdm::SERVICE_STATE_ID:
                if(_debug)
                    _log += "Service State: \n";
                break;
            case rfa::rdm::SERVICE_LOAD_ID:
                if(_debug)
                    _log += "Service Load: \n";
                break;
            default:
                _log += "DirectoryHandler: Unexpected FilterID ";
                _log.append((int)filterEntry.getFilterID());
                _log += "\n";
                break;
        }
        decodeElementList(elementList, serviceName, d);
    }
}
void DirectoryHandler::decodeElementList(const rfa::data::ElementList& elementList, const std::string& serviceName, dict &d){
    rfa::data::ElementListReadIterator elri;
    for (elri.start(elementList); !elri.off(); elri.forth()) {
        const rfa::data::ElementEntry& entry = elri.value();
        const rfa::common::Data& elementData = entry.getData();
        std::string entryName = entry.getName().c_str();
        // split camel strings e.g. ServiceState to SERVICE_STATE
        if (entryName == "QoS") {
            boost::algorithm::to_upper(entryName);
        }
        const boost::regex re("(?<!^)([A-Z][a-z]|(?<=[a-z])[A-Z])");
        entryName = boost::regex_replace(entryName, re, "_$1");
        boost::algorithm::to_upper(entryName);
        if(_debug) {
            _log += "\t";
            _log.append(entryName.c_str());
            _log += ": ";
        }
        switch (elementData.getDataType()) {
            case rfa::data::DataBufferEnum: 
                {
                    const rfa::data::DataBuffer& db = static_cast<const rfa::data::DataBuffer&>(elementData);
                    if (entryName != "STATUS") {
                        d[entryName.c_str()] = RDMUtils::dataBufferToString(db).c_str();
                    }
                    if(_debug) {
                        _log.append(RDMUtils::dataBufferToString(db).c_str());
                    }
                    if ((entryName == "SERVICE_STATE") &&
                        (!serviceName.compare(_serviceName) )) {
                        if (db.getUInt32()==1)
                            _isServiceUp= true;
                        else
                            _isServiceUp = false;  
                    }
                    break;              
                }
            case rfa::data::ArrayEnum:
                {
                    const rfa::data::Array& array = static_cast<const rfa::data::Array&>(elementData);
                    if (entryName != "QOS") {
                        d[entryName.c_str()] = decodeArray(array).trimWhitespace().c_str();      
                    }
                    break;
                }
            default:
                {
                    if(_debug)
                        cout <<"DirectoryHandler: Unexpected type in ElementEntry" << endl;
                    _log += "DirectoryHandler: Unexpected type in ElementEntry";
                }
        }
        if(_debug) {
            _log += "\n";
        }
    }
}

rfa::common::RFA_String DirectoryHandler::decodeArray(const rfa::data::Array& array) {
    rfa::common::RFA_String decoded;
    rfa::data::ArrayReadIterator ari;
    for (ari.start(array); !ari.off(); ari.forth()) {
        const rfa::data::ArrayEntry& entry = ari.value();
        const rfa::common::Data& data = entry.getData();
        const rfa::data::DataBuffer& db = static_cast<const rfa::data::DataBuffer&>(data);
        if(_debug) {
            _log.append(RDMUtils::dataBufferToString(db).c_str());
            _log += " ";
        }
        decoded.append(RDMUtils::dataBufferToString(db).c_str());
        decoded += " ";
    }
    return decoded;
}

void DirectoryHandler::setDebugMode(const bool &debug) {
    _debug = debug;
}
