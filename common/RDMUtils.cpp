#include "common/RDMUtils.h"
#include "common/RDMDictDef.h"
#include <boost/lexical_cast.hpp>

#include <sstream>
#include <iomanip>

double RDMUtils::dataBufferToDouble(const rfa::data::DataBuffer& dataBuffer) {
    double ret = -1;
    //double scale = 0.0000000001;
    switch(dataBuffer.getDataBufferType())
    {
    case rfa::data::DataBuffer::FloatEnum:
        ret = dataBuffer.getFloat();
        break;
    case rfa::data::DataBuffer::DoubleEnum:
        ret = dataBuffer.getDouble();
        break;
    case rfa::data::DataBuffer::Real32Enum:
    case rfa::data::DataBuffer::Real64Enum:
        // Workaround: this may causes,for example, 4.6 tp become 4.6000000000000005 in python
        // use scale to limit decimal point
        ret = dataBuffer.getDouble();
        //ret = floor(ret / scale) * scale;
        break;
    default:
        ret = -1;
        break;
    }
    return ret;
}

int RDMUtils::dataBufferToInt(const rfa::data::DataBuffer& dataBuffer) {
    int ret = -1;
    switch(dataBuffer.getDataBufferType())
    {
    case rfa::data::DataBuffer::Int32Enum:
        ret = (int)dataBuffer.getInt();
        break;
    case rfa::data::DataBuffer::UInt32Enum:
        ret = (int)dataBuffer.getUInt();
        break;
    default:
        ret = -1;
        break;
    }
    return ret;
}

long RDMUtils::dataBufferToLong(const rfa::data::DataBuffer& dataBuffer) {
    long ret = -1;
    switch(dataBuffer.getDataBufferType())
    {
    case rfa::data::DataBuffer::Int64Enum:
        ret = (long)dataBuffer.getInt();
        break;
    case rfa::data::DataBuffer::UInt64Enum:
        ret = (long)dataBuffer.getUInt();
        break;
    default:
        ret = -1;
        break;
    }
    return ret;
}

std::string RDMUtils::dataBufferToString(const rfa::data::DataBuffer& dataBuffer,
                                         const RDMEnumDef* enumDef)
{
    rfa::common::RMTESConverter converter;
    rfa::data::PartialUpdateReadIterator partialReadIt;
    
    ostringstream ost;
    ost << setfill('0');

    switch(dataBuffer.getDataBufferType())
    {
    case rfa::data::DataBuffer::FloatEnum:
    case rfa::data::DataBuffer::DoubleEnum:
    case rfa::data::DataBuffer::Real32Enum:
    case rfa::data::DataBuffer::Real64Enum:
    case rfa::data::DataBuffer::Int32Enum:
    case rfa::data::DataBuffer::UInt32Enum:
    case rfa::data::DataBuffer::Int64Enum:
    case rfa::data::DataBuffer::UInt64Enum:
    case rfa::data::DataBuffer::TimeEnum:
    case rfa::data::DataBuffer::DateEnum:
    case rfa::data::DataBuffer::DateTimeEnum:
        ost << dataBuffer.getAsString().c_str();
        break;

    case rfa::data::DataBuffer::EnumerationEnum:
        if (enumDef)
            ost << enumDef->findEnumVal(dataBuffer.getEnumeration()).c_str(); 
        else
            ost<< " EnumDef not found, enum value=" << dataBuffer.getAsString().c_str();
        break;

    case rfa::data::DataBuffer::StringAsciiEnum:
    case rfa::data::DataBuffer::StringRMTESEnum:
    case rfa::data::DataBuffer::StringUTF8Enum:
        if (dataBuffer.isPartialUpdates()) {
            partialReadIt.start(dataBuffer.getBuffer());
            while (!partialReadIt.off()) {
                const rfa::common::Buffer& fieldUpdatedBuf = partialReadIt.value();
                rfa::common::Int16 offset = partialReadIt.offset();
                converter.setBuffer(fieldUpdatedBuf, offset);
                partialReadIt.forth();
            }
            // For update msg display
            ost << converter.getAsCharString().c_str();
        } else {
            converter.setBuffer(dataBuffer.getBuffer());
            // For refresh msg display
            ost << converter.getAsCharString().c_str();
        }
        break;
    case rfa::data::DataBuffer::BufferEnum:
        ost<<string((char*)dataBuffer.getBuffer().c_buf(),dataBuffer.getBuffer().size());
        break;

    case rfa::data::DataBuffer::QualityOfServiceInfoEnum:
        {
            const rfa::common::QualityOfService& qos = dataBuffer.getQualityOfServiceInfo().getQualityOfService();
            ost<< "Rate: " << qos.getRate() << ", Timeliness: " << qos.getTimeliness();
            break;
        }
    case rfa::data::DataBuffer::RespStatusEnum:
        {
            const rfa::common::RespStatus& status = dataBuffer.getRespStatus();

            ost << " dataState=\"" << dataStateToString (status.getDataState()) <<"\"";
            ost << " streamState=\"" << streamStateToString(status.getStreamState()) <<"\"";
            ost << " statusCode=\"" << statusCodeToString(status.getStatusCode()) <<"\"";
            ost << " statusText=\"" << status.getStatusText().c_str() <<"\"";
            break;
        }
    case rfa::data::DataBuffer::NoDataBufferEnum:
        ost<<"No Data DataBuffer";
        break;

    case rfa::data::DataBuffer::UnknownDataBufferEnum:
        ost<<"Unknown DataBuffer";
        break;

    default:
        ost<<"UNKNOWN DATA TYPE";
        break;
    }

    return ost.str();
}

std::string RDMUtils::dataBufferToStringNoCoversion(const rfa::data::DataBuffer& dataBuffer)
{
    ostringstream ost;
    ost << setfill('0');
    ost << string((char*)dataBuffer.getBuffer().c_buf(),dataBuffer.getBuffer().size());
    return ost.str();
}

string RDMUtils::streamStateToString(const rfa::common::RespStatus::StreamState& streamState)    {
    string utilString;
    switch (streamState)
    {
    case rfa::common::RespStatus::UnspecifiedStreamStateEnum:
        utilString = "UnspecifiedStreamState";
        break;
    case rfa::common::RespStatus::OpenEnum:
        utilString = "Open";
        break;
    case rfa::common::RespStatus::NonStreamingEnum:
        utilString = "NonStreaming";
        break;
    case rfa::common::RespStatus::ClosedRecoverEnum:
        utilString = "ClosedRecoverable";
        break;
    case rfa::common::RespStatus::ClosedEnum:
        utilString = "Closed";
        break;
    case rfa::common::RespStatus::RedirectedEnum:
        utilString = "Redirected";
        break;
    default:
        utilString = "Unknown StreamState";
        break;
    }
    return utilString;
}

string RDMUtils::dataStateToString(const rfa::common::RespStatus::DataState& dataState)
{
    string utilString;
    switch (dataState)
    {
    case rfa::common::RespStatus::OkEnum:
        utilString = "Ok";
        break;
    case rfa::common::RespStatus::SuspectEnum:
        utilString = "Suspect";
        break;
    case rfa::common::RespStatus::UnspecifiedEnum:
        utilString = "Unspecified";
        break;
    default:
        utilString = "Unknown DataState";
        break;
    }
    return utilString;
}



string RDMUtils::statusCodeToString(const rfa::common::UInt8 statusCode)
{
    string utilString;
    switch (statusCode)
    {
    case rfa::common::RespStatus::NoneEnum:
        utilString = "None";
        break;
    case rfa::common::RespStatus::NotFoundEnum:
        utilString = "NotFound";
        break;
    case rfa::common::RespStatus::TimeoutEnum:
        utilString = "Timeout";
        break;
    case rfa::common::RespStatus::NotAuthorizedEnum:
        utilString = "NotAuthorized";
        break;
    case rfa::common::RespStatus::InvalidArgumentEnum:
        utilString = "InvalidArgument";
        break;
    case rfa::common::RespStatus::UsageErrorEnum:
        utilString = "UsageError";
        break;
    case rfa::common::RespStatus::PreemptedEnum:
        utilString = "Preempted";
        break;
    case rfa::common::RespStatus::JustInTimeFilteringStartedEnum:
        utilString = "JustInTimeFilteringStarted";
        break;
    case rfa::common::RespStatus::TickByTickResumedEnum:
        utilString = "TickByTickResumed";
        break;
    case rfa::common::RespStatus::FailoverStartedEnum:
        utilString = "FailoverStarted";
        break;
    case rfa::common::RespStatus::FailoverCompletedEnum:
        utilString = "FailoverCompleted";
        break;
    case rfa::common::RespStatus::GapDetectedEnum:
        utilString = "GapDetected";
        break;
    default:
        utilString = "Unknown RespCode";
        break;
    }
    return utilString;
}


string RDMUtils::msgModelTypeToString(rfa::common::UInt16 msgType)
{
    rfa::common::RFA_String tmpString;
    switch (msgType)
    {
        case rfa::rdm::MMT_MARKET_PRICE:
            tmpString = "MarketPrice";
            break;
        case rfa::rdm::MMT_MARKET_BY_ORDER:
            tmpString = "MarketByOrder";
            break;
        case rfa::rdm::MMT_MARKET_BY_PRICE:
            tmpString = "MarketByPrice";
            break;
        case rfa::rdm::MMT_MARKET_MAKER:
            tmpString = "MarketMaker";
            break;
        case rfa::rdm::MMT_SYMBOL_LIST:
            tmpString = "SymbolList";
            break;
        //case rfa::rdm::MMT_RANKED_SYMBOL_LIST:
        //    tmpString = "RankedSymbolList";
        //    break;
        case rfa::rdm::MMT_HISTORY:
            tmpString = "History";
            break;
        case rfa::rdm::MMT_LOGIN:
            tmpString = "Login";
            break;
        case rfa::rdm::MMT_DIRECTORY:
            tmpString = "Directory";
            break;
        case rfa::rdm::MMT_DICTIONARY:
            tmpString = "Dictionary";
            break;
        default:
            char tmpstr[100];
            sprintf(tmpstr,"Unknown MsgType[%d]",msgType);
            tmpString = tmpstr;
            break;
    }
    return tmpString.c_str();
}


string RDMUtils::msgRespTypeToString(rfa::common::UInt32 respType)
{
    switch (respType)
    {
        case rfa::message::RespMsg::RefreshEnum:
            return "Refresh";
        case rfa::message::RespMsg::StatusEnum:
            return "Status";
        case rfa::message::RespMsg::UpdateEnum:
            return "Update";
        default:
            return "Invalid Response Type";
    }
}
