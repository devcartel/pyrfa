//
//|---------------------------------------------------------------
//|				StandardOut										--
//|---------------------------------------------------------------
//

#ifdef WIN32
#pragma warning(disable : 4786)
#endif
#include "StandardOut.h"
#include <stdarg.h>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
/// Constructor
///
StandardOut::StandardOut() : m_indents(0)
{}

StandardOut::~StandardOut()
{}

/// write FieldList
void StandardOut::outBegin(const FieldList& input)
{
	m_indents++;
	printf("\n");
	out("FieldList:\n");
	out("standard data count = %d\n", input.getStandardDataCount());
}

void StandardOut::outEnd(const FieldList& input)
{
	m_indents--;
}

void StandardOut::outBegin(const FieldEntry& input, const char* fieldName)
{
	m_indents++;
	if (strlen(fieldName))
		out("FieldEntry: %s", fieldName);
}

void StandardOut::outEnd(const FieldEntry& input, const char* fieldName)
{
	m_indents--;
	printf("\n");
}


/// write Series
void StandardOut::outBegin(const Series& input)
{
	m_indents++;
	printf("\n");
}

void StandardOut::outEnd(const Series& input)
{
	m_indents--;
	printf("\n");
}

void StandardOut::outBegin(const SeriesEntry& input)
{
	encodeIndents();
}

void StandardOut::outEnd(const SeriesEntry& input)
{
	printf("\n");
}


//write ElementListDef
void StandardOut::outBegin(const ElementListDef& input)
{
	m_indents++;
}

void StandardOut::outEnd(const ElementListDef& input)
{
	m_indents--;
	printf("\n");
}

void StandardOut::out(const ElementEntryDef& input)
{
	printf("ElementEntryDef name= %s dataType=%u encodedDataType=%u\n",
		input.getName().c_str(),
		(UInt16)input.getType(),
		(UInt16)input.getEncodedType());
}

//write FieldListDef
void StandardOut::outBegin(const FieldListDef& input)
{
	m_indents++;
}

void StandardOut::outEnd(const FieldListDef& input)
{
	m_indents--;
}

void StandardOut::out(const FieldEntryDef& input)
{}

/// write ElementList
void StandardOut::outBegin(const ElementList& input)
{
	//increase indent so entries are tabbed
	m_indents++;
	//end this line nesting is preserved
	printf("\n");
}

void StandardOut::outEnd(const ElementList& input)
{
	m_indents--;
	printf("\n");
}

void StandardOut::outBegin(const ElementEntry& input)
{
	encodeIndents();
	printf("%s ",input.getName().c_str());
}

void StandardOut::outEnd(const ElementEntry& input)
{
	printf("\n");
}

/// write Vector
void StandardOut::outBegin(const Vector& input)
{
	m_indents++;
}

void StandardOut::outEnd(const Vector& input)
{
	m_indents--;
}

void StandardOut::outBegin(const VectorEntry& input)
{
	encodeIndents();
}

void StandardOut::outEnd(const VectorEntry& input)
{
	printf("\n");
}

/// write FilterList
void StandardOut::outBegin(const FilterList& input)
{
	m_indents++;
	printf("\n");
}

void StandardOut::outEnd(const FilterList& input)
{
	m_indents--;
	printf("\n");
}

void StandardOut::outBegin(const FilterEntry& input)
{
	encodeIndents();
}

void StandardOut::outEnd(const FilterEntry& input)
{
	printf("\n");
}

/// write Map
void StandardOut::outBegin(const Map& input)
{
	m_indents++;
	printf("\n");
}

void StandardOut::outEnd(const Map& input)
{
	m_indents--;
	printf("\n");
}

void StandardOut::outBegin(const MapEntry& input)
{
	encodeIndents();
}

void StandardOut::outEnd(const MapEntry& input)
{
	printf("\n");
}

/// write key
void StandardOut::outKeyBegin(const Data& input)
{
	m_indents++;
}

void StandardOut::outKeyEnd(const Data& input)
{
	m_indents--;
}


/// write Array
void StandardOut::outBegin(const Array& input)
{
	m_indents++;
}

void StandardOut::outEnd(const Array& input)
{
	m_indents--;
}

void StandardOut::outArrayEntryBegin(const ArrayEntry& input)
{
	printf("\n");
	encodeIndents();
}

void StandardOut::outArrayEntryEnd(const ArrayEntry& input)
{
}

/// write Msg
void StandardOut::outBegin(const Msg& input)
{
	m_indents++;
	printf("\n");
}

void StandardOut::outEnd(const Msg& input)
{
	m_indents--;
}

void StandardOut::outAttrib(const AttribInfo& attribInfo)
{
	UInt8 hint = attribInfo.getHintMask();

	m_indents++;
	if (hint & AttribInfo::DataMaskFlag)
		out("data mask: %d\n", attribInfo.getDataMask());
	if (hint & AttribInfo::NameFlag)
		out("name: %s\n", attribInfo.getName().c_str());
	if (hint & AttribInfo::NameTypeFlag)
		out("name type: %d\n", attribInfo.getNameType());
	if (hint & AttribInfo::ServiceNameFlag)
		out("service name: %s\n", attribInfo.getServiceName().c_str());
	if (hint & AttribInfo::IDFlag)
		out("ID: %d\n", attribInfo.getID());
	if (hint & AttribInfo::AttribFlag)
		out("attrib: true (not decoded)\n");
	m_indents--;
}

void StandardOut::outBegin(const ReqMsg& input)
{
	UInt8 hint = input.getHintMask();

	out("Req Message:\n");
	out("AttribInfo: ");
	if (hint & ReqMsg::AttribInfoFlag)
	{
		printf("\n");
		outAttrib(input.getAttribInfo());
	}
	else
		printf("(none)\n");

	out("ReqMsg::model: %d\n", input.getMsgModelType());
	out("ReqMsg::interaction: %d\n", input.getInteractionType());
	if (hint && ReqMsg::QualityOfServiceReqFlag)
	{
		QualityOfServiceRequest qosr = input.getRequestedQualityOfService();
		out("ReqMsg::req QoS: %ld %ld\n", qosr.getBestRate(), qosr.getBestTimeliness());
	}
	if (input.getIndicationMask() & rfa::privateStream::PrivateStreamFlag )
		out("ReqMsg::PrivateStreamFlag: true\n");
	if (hint && ReqMsg::PayloadFlag)
		out("ReqMsg::payload:\n");
}

void StandardOut::outEnd(const ReqMsg& input)
{
}

void StandardOut::outBegin(const RespMsg& input)
{
	UInt8 hint = input.getHintMask();

	out("Resp Message:\n");
	out("AttribInfo: ");
	if (hint & RespMsg::AttribInfoFlag)
	{
		printf("\n");
		outAttrib(input.getAttribInfo());
	}
	else
		printf("(none)\n");

	out("RespMsg::Resp type: %d\n", input.getRespType());
	if (hint & RespMsg::QualityOfServiceFlag)
	{
		out("RespMsg::QoS: ");
		QualityOfService qos = input.getQualityOfService();
		QualityOfServiceInfo qosi(qos);
		out(qosi);
		printf("\n");
	}
	if( hint & RespMsg::RespStatusFlag)
	{
		out("RespMsg::Resp status: ");
		out(input.getRespStatus());
		printf("\n");
	}
	if( hint & RespMsg::RespTypeNumFlag)
		out("RespMsg::Resp type num: %d\n", input.getRespTypeNum());
	if (input.getIndicationMask() & rfa::privateStream::PrivateStreamFlag )
		out("RespMsg::PrivateStreamFlag: true\n");
	if (hint && RespMsg::PayloadFlag)
		out("RespMsg::payload:\n");
}

void StandardOut::outEnd(const RespMsg& input)
{
}

void StandardOut::outBegin(const GenericMsg& input)
{
	UInt8 hint = input.getHintMask();

	out("Generic Message ");
	out("AttribInfo: ");
	if (hint & GenericMsg::AttribInfoFlag)
	{
		printf("\n");
		outAttrib(input.getAttribInfo());
	}
	else
		printf("(none)\n");

	if (hint & GenericMsg::SeqFlag)
		out("GenericMsg::seq: %d\n", input.getSeqNum());
	if (hint & GenericMsg::SecondarySeqFlag)
		out("GenericMsg::2nd seq: %d\n", input.getSecondarySeqNum());
	if (input.getIndicationMask() & rfa::privateStream::PrivateStreamFlag )
		out("GenericMsg::PrivateStreamFlag: true\n");
	if (hint && GenericMsg::PayloadFlag)
		out("GenericMsg::payload:\n");
	else
		out("GenericMsg: (no payload)\n");
}

void StandardOut::outEnd(const GenericMsg& input)
{
}

void StandardOut::outBegin(const PostMsg& input)
{
	UInt8 hint = input.getHintMask();

	out("PostMsg:\n");
	out("AttribInfo: ");
	if (hint & PostMsg::AttribInfoFlag)
	{
		printf("\n");
		outAttrib(input.getAttribInfo());
	}
	else
		printf("(none)\n");

	if (hint & PostMsg::PostIdFlag)
		out("PostMsg::post ID: %d\n", input.getPostID());
	if (hint & PostMsg::SeqFlag)
		out("PostMsg::seq num: %d\n", input.getSeqNum());
	if (hint && PostMsg::PayloadFlag)
		out("PostMsg::payload:\n");
	else
		out("PostMsg: (no payload)\n");
}

void StandardOut::outEnd(const PostMsg& input)
{
}

void StandardOut::outBegin(const AckMsg& input)
{
	UInt8 hint = input.getHintMask();

	out("AckMsg:\n");
	out("AttribInfo: ");
	if (hint & AckMsg::AttribInfoFlag)
	{
		printf("\n");
		outAttrib(input.getAttribInfo());
	}
	else
		printf("(none)\n");

	out("AckMsg::ack ID: %d\n", input.getAckID());
	if (hint & AckMsg::SeqFlag)
		out("AckMsg::seq num: %d\n", input.getSeqNum());
	if (hint & AckMsg::NackCodeFlag)
		out("AckMsg::nack code: %d\n", input.getNackCode());
	if (hint & AckMsg::TextFlag)
		out("AckMsg::text flag: %s\n", input.getText().c_str());
	if (hint && AckMsg::PayloadFlag)
		out("AckMsg::payload:\n");
}

void StandardOut::outEnd(const AckMsg& input)
{
}

/// write DataBuffer
void StandardOut::out(const DataBuffer& dataBuffer)
{
	char* sData;

	UInt8 dataBufferType = dataBuffer.getDataBufferType();

	switch ( dataBufferType )
	{
		case DataBuffer::EnumerationEnum:
			printf("%u", dataBuffer.getEnumeration());
			break;
		case DataBuffer::FloatEnum:
			printf("%f", dataBuffer.getFloat());
			break;
		case DataBuffer::DoubleEnum:
			printf("%e", dataBuffer.getDouble());
			break;
		case DataBuffer::IntEnum:
			// Need to make sure ...
			printf("%lld", dataBuffer.getInt());
			break;
		case DataBuffer::UIntEnum:
			// Need to make sure ...
			printf("%llu", dataBuffer.getUInt());
			break;
		case DataBuffer::RealEnum:
			printf("%s", dataBuffer.getAsString().c_str());
			break;
		case DataBuffer::TimeEnum:
			out(dataBuffer.getTime());
			break;
		case DataBuffer::DateEnum:
			out(dataBuffer.getDate());
			break;
		case DataBuffer::DateTimeEnum:
			out(dataBuffer.getDateTime());
			break;
		case DataBuffer::QualityOfServiceInfoEnum:
			out(dataBuffer.getQualityOfServiceInfo());
			break;
		case DataBuffer::StringAsciiEnum:
		case DataBuffer::StringUTF8Enum:
		case DataBuffer::StringRMTESEnum:
		{
			const Buffer& buf = dataBuffer.getBuffer();
			sData = new char[buf.size() + 1];

			strncpy( sData, (char*)buf.c_buf(), buf.size() );
			sData[ buf.size() ] = '\0';
			printf( "\"%s\"", sData );

			delete[] sData;
		}
		break;
		case DataBuffer::BufferEnum:
		{
			const Buffer& buf = dataBuffer.getBuffer();
			sData = new char[buf.size() + 1];

			strncpy( sData, (char*)buf.c_buf(), buf.size() );
			sData[ buf.size() ] = '\0';
			printf( "%s", sData );

			delete[] sData;
		}
		break;
		case DataBuffer::UnknownDataBufferEnum:
			printf( "%s", "UnknownDataBuffer" );
			break;
		default:
			printf("StandardOut::out() - Unsupported or Not-Yet-Impl'd DataBuffer Type\n");
			break;
	}
}

/// write primitives
void StandardOut::out(const QualityOfServiceInfo& input)
{
	printf("%ld %ld", input.getQualityOfService().getRate(), input.getQualityOfService().getTimeliness());
}


void StandardOut::out(const Date& input)
{
	//must cast out of UInt8s return types,they are interpreted as unsigned chars
	printf("%u/%u/%u", (UInt16) input.getMonth(),
		(UInt16) input.getDay(),
		(UInt16) input.getYear());
}

void StandardOut::out(const Time& input)
{
	printf("%u:%u:%u", (UInt16) input.getHour(),
		(UInt16) input.getMinute(),
		(UInt16) input.getSecond());
}

void StandardOut::out(const DateTime& input)
{
	printf("%u/%u/%u %u:%u:%u",
		(UInt16) input.getMonth(),
		(UInt16) input.getDay(),
		(UInt16) input.getYear(),
		(UInt16) input.getHour(),
		(UInt16) input.getMinute(),
		(UInt16) input.getSecond());
}

void StandardOut::out(const RespStatus& input)
{
	char* _utilString;
	switch (input.getStreamState())
	{
		case RespStatus::UnspecifiedStreamStateEnum:
			_utilString = const_cast<char *>("UnspecifiedStreamState");
			break;
		case RespStatus::OpenEnum:
			_utilString = const_cast<char *>("Open");
			break;
		case RespStatus::NonStreamingEnum:
			_utilString = const_cast<char *>("NonStreaming");
			break;
		case RespStatus::ClosedRecoverEnum:
			_utilString = const_cast<char *>("ClosedRecoverable");
			break;
		case RespStatus::ClosedEnum:
			_utilString = const_cast<char *>("Closed");
			break;
		case RespStatus::RedirectedEnum:
			_utilString = const_cast<char *>("Redirected");
			break;
		default:
			_utilString = const_cast<char *>("Unknown StreamState");
			break;
	}
	printf("%s ",_utilString);

	switch (input.getDataState())
	{
		case RespStatus::OkEnum:
			_utilString = const_cast<char *>("Ok");
			break;
		case RespStatus::SuspectEnum:
			_utilString = const_cast<char *>("Suspect");
			break;
		case RespStatus::NoChangeEnum:
			_utilString = const_cast<char *>("NoChange");
			break;
		default:
			_utilString = const_cast<char *>("Unknown DataState");
			break;
	}
	printf("%s ",_utilString);

	switch (input.getStatusCode())
	{
		case RespStatus::NoneEnum:
			_utilString = const_cast<char *>("None");
			break;
		case RespStatus::NotFoundEnum:
			_utilString = const_cast<char *>("NotFound");
			break;
		case RespStatus::TimeoutEnum:
			_utilString = const_cast<char *>("Timeout");
			break;
		case RespStatus::NotAuthorizedEnum:
			_utilString = const_cast<char *>("NotAuthorized");
			break;
		case RespStatus::InvalidArgumentEnum:
			_utilString = const_cast<char *>("InvalidArgument");
			break;
		case RespStatus::UsageErrorEnum:
			_utilString = const_cast<char *>("UsageError");
			break;
		case RespStatus::PreemptedEnum:
			_utilString = const_cast<char *>("Preempted");
			break;
		case RespStatus::JustInTimeFilteringStartedEnum:
			_utilString = const_cast<char *>("JustInTimeFilteringStarted");
			break;
		case RespStatus::TickByTickResumedEnum:
			_utilString = const_cast<char *>("TickByTickResumed");
			break;
		case RespStatus::FailoverStartedEnum:
			_utilString = const_cast<char *>("FailoverStarted");
			break;
		case RespStatus::FailoverCompletedEnum:
			_utilString = const_cast<char *>("FailoverCompleted");
			break;
		case RespStatus::GapDetectedEnum:
			_utilString = const_cast<char *>("GapDetected");
			break;
		default:
			_utilString = const_cast<char *>("Unknown RespCode");
			break;
	}
	printf("%s ",_utilString);

	//show status text
	printf("%s",input.getStatusText().c_str());
}

void StandardOut::out(UInt16 input)
{
	printf("%d",input);
}

void StandardOut::out(Float input)
{
	printf("%f",input);
}

void StandardOut::out(Double input)
{
	printf("%e",input);
}

void StandardOut::out(const std::string & input)
{
	printf("%s",input.c_str());
}

void StandardOut::out(const char *input, ...)
{
	va_list arglist;
	assert(input != NULL);
	va_start(arglist, input);
	encodeIndents();
	vprintf(input, arglist);
	va_end(arglist);
}


/// write formatting
void StandardOut::encodeIndents()
{
	for (int i = 0; i < m_indents; i++)
		printf("    "); //4 spaces
}

/// write before or after decoding
void StandardOut::writeDataBodyBegin()
{}

void StandardOut::writeDataBodyEnd()
{}
