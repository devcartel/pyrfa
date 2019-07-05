#ifdef WIN32
#pragma warning(disable : 4786)
#endif

#include <new>
#include <stdlib.h>
#include <stdio.h>
#include "SingleIterDecoder_NOSTL.h"

using namespace rfa::common;
using namespace rfa::data;

///////////////////////////////////////////////////////////////////////////////
/// Constructor
///
SingleIterDecoder::SingleIterDecoder(Out* out):
_out(out),
_indents(0),
_rdmFieldDict(),
_dictionaryDecoder(_rdmFieldDict),
_bDictionaryLoaded(false),
_appendixAComplete(false),
_enumComplete(false),
_def(0)
{
}

///////////////////////////////////////////////////////////////////////////////
/// Constructor
///
SingleIterDecoder::SingleIterDecoder(Out* out, const char* a_path, const char* e_path):
_out(out),
_indents(0),
_rdmFieldDict(),
_dictionaryDecoder(_rdmFieldDict),
_bDictionaryLoaded(false),
_appendix_a_path(a_path),
_appendixAComplete(false),
_enumType_def_path(e_path),
_enumComplete(false),
_def(0)
{
	_dictionaryDecoder.load( RFA_String(_appendix_a_path, 0, false), RFA_String(_enumType_def_path, 0, false) );
	setRWFEnumComplete();
	setRWFAppendixAComplete();
}

///////////////////////////////////////////////////////////////////////////////
/// Destructor
///
SingleIterDecoder::~SingleIterDecoder()
{
	if ( _def )
		delete _def;
}
///////////////////////////////////////////////////////////////////////////////
///	Map
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/// 
///
void SingleIterDecoder::decodeMap(const Map& input)
{
	_singleReadIter.start(static_cast<const Map&>(input));

	_out->outBegin(input);

	if(input.getIndicationMask() & Map::SummaryDataFlag)
		decodeData(input.getSummaryData());

	if(input.getDataDefCount() > 0) // contains data definitions
		decodeDataDef(input.getDataDef());

	while(!_singleReadIter.off())
	{
		decodeMapEntry(static_cast<const MapEntry&>(_singleReadIter.value()));
		_singleReadIter.forth();
	}
	_out->outEnd(input);

}

///////////////////////////////////////////////////////////////////////////////
/// 
///
void SingleIterDecoder::decodeMapEntry(const MapEntry& input)
{
	_out->outBegin(input);

	decodeKey(input.getKeyData());
	decodeData(input.getData());

	_out->outEnd(input);
}

///////////////////////////////////////////////////////////////////////////////
/// 
///
void SingleIterDecoder::decodeKey(const Data& input)
{
	_out->outKeyBegin(input);
	decodeData(input);
	_out->outKeyEnd(input);
}


///////////////////////////////////////////////////////////////////////////////
///	ElementList
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/// 
///
void SingleIterDecoder::decodeElementList(const ElementList& input)	
{
	_singleReadIter.start(input);

	_out->outBegin(input);
	while(!_singleReadIter.off())
	{
		const ElementEntry& data = static_cast<const ElementEntry&>(_singleReadIter.value());
		decodeElementEntry(data);
		_singleReadIter.forth();
	}
	_out->outEnd(input);
}

///////////////////////////////////////////////////////////////////////////////
/// 
///
void SingleIterDecoder::decodeElementEntry(const ElementEntry& input)	
{
	_out->outBegin(input);
	decodeData(input.getData());
	_out->outEnd(input);
}



///////////////////////////////////////////////////////////////////////////////
///	FieldList
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/// 
///



void SingleIterDecoder::decodeFieldList(const FieldList& input)	
{
	// determine mask to be used
	//```````````````````````````````````````````//
	UInt8 mask = rfa::data::DefinedDataFlag | rfa::data::StandardDataFlag;	

	//	 contain defined data content, requiring global data definitions for decoding;
	//	 since global definitions are not available, process only entries containing standard data content
	if((input.getContentMask() & FieldList::DefinedDataFlag) && 
      !(input.getContentMask() & FieldList::LocalDataDefFlag))
	{
		// error - global data defintions not supported yet, so skip the defined data
		mask = StandardDataFlag;		
	}

	// iterate though the fieldlist
	//```````````````````````````````````````````//
	_out->outBegin(input);

	_singleReadIter.start(input);

	while(!_singleReadIter.off())
	{
		decodeFieldEntry(static_cast<const FieldEntry&>(_singleReadIter.value()));
		_singleReadIter.forth();
	}

	_out->outEnd(input);
}
///////////////////////////////////////////////////////////////////////////////
/// 
///
void SingleIterDecoder::decodeFieldEntry(const FieldEntry& input)	
{	

	if (input.getContentMask() & FieldEntry::DefinedDataFlag)
	{
		decodeData(input.getData());
	}
	else
	{
		RFA_String fieldName;
		const RDMFieldDef* fieldDef = _rdmFieldDict.getFieldDef( input.getFieldID() );

		if (fieldDef)
		{
			//check if enum
			if(fieldDef->getDataType() == DataBuffer::EnumerationEnum)
				_currentEnumDef = fieldDef->getEnumDef();

			fieldName = fieldDef->getName();
			_out->outBegin(input, fieldName.c_str());
			printf("\t");
			decodeData(input.getData((UInt8)(fieldDef->getDataType())));
		}
		else
		{
			_out->outBegin(input, fieldName.c_str());
			_out->out("field ID %d does not exist in data dictionary", input.getFieldID());
		}

		_out->outEnd(input, fieldName.c_str());
	}	
}

///////////////////////////////////////////////////////////////////////////////
///	Definitions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/// 
///
void SingleIterDecoder::decodeDataDef(const RFA_Vector<DataDef*>& input)
{
	UInt32 defCount = input.size();
	for ( UInt32 i = 0; i < defCount; ++i)
	{
		const DataDef &def = *input[i];
		if(def.getDefType() == DataDef::ElementListDefEnum)
			decodeElementListDef(static_cast<const ElementListDef&>(def));
		else 
			decodeFieldListDef(static_cast<const FieldListDef&>(def));
	}
}

///////////////////////////////////////////////////////////////////////////////
/// 
///
void SingleIterDecoder::decodeElementListDef(const ElementListDef& elementListDef)
{

	ElementListDefReadIterator it;
	it.start(elementListDef);

	_out->outBegin(elementListDef);

	while(!it.off())
	{		
		decodeElementEntryDef(it.value());
		it.forth();
	}
	_out->outEnd(elementListDef);

}

///////////////////////////////////////////////////////////////////////////////
/// 
///
void SingleIterDecoder::decodeElementEntryDef(const ElementEntryDef& def)
{
	_out->out(def);
}

///////////////////////////////////////////////////////////////////////////////
/// 
///
void SingleIterDecoder::decodeFieldListDef(const FieldListDef& fieldListDef)
{
	FieldListDefReadIterator it;
	it.start(fieldListDef);

	_out->outBegin(fieldListDef);
	while(!it.off())
	{
		decodeFieldEntryDef(it.value());
		it.forth();
	}
	_out->outEnd(fieldListDef);
}

///////////////////////////////////////////////////////////////////////////////
/// 
///
void SingleIterDecoder::decodeFieldEntryDef(const FieldEntryDef& def)
{
	_out->out(def);
}

///////////////////////////////////////////////////////////////////////////////
///	FilterList
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/// 
///
void SingleIterDecoder::decodeFilterList(const FilterList& input)
{
	_singleReadIter.start(static_cast<const Data&>(input));

	_out->outBegin(input);
	while(!_singleReadIter.off())
	{
		decodeFilterEntry((const FilterEntry&)_singleReadIter.value());	
		_singleReadIter.forth();
	}
	_out->outEnd(input);
}

///////////////////////////////////////////////////////////////////////////////
/// 
///
void SingleIterDecoder::decodeFilterEntry(const FilterEntry& input)
{
	_out->outBegin(input);
	const Data& data = input.getData();
	decodeData(data);
	_out->outEnd(input);
}

///////////////////////////////////////////////////////////////////////////////
///	Dictionary
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/// 
///
void SingleIterDecoder::setRWFAppendixAComplete()
{
	_appendixAComplete = true;
}
void SingleIterDecoder::loadDictionaryFromFile( const RFA_String& appendix_a_path, const RFA_String& enumtype_def_path )
{
	if ( _bDictionaryLoaded ) return;

	if ( _dictionaryDecoder.load( appendix_a_path, enumtype_def_path ) )
	{
		RFA_String version("1.1", 4, false);
		_rdmFieldDict.setVersion(version);
		_rdmFieldDict.setDictId(1);
		_rdmFieldDict.enumDict().setVersion(version);
		_rdmFieldDict.enumDict().setDictId(1);
		_bDictionaryLoaded = true;
	}
	else
	{
		printf( "initDictionary failed to load dictionary\n" );
		assert(0);
	}
}
///////////////////////////////////////////////////////////////////////////////
/// 
///
void SingleIterDecoder::setRWFEnumComplete()
{
	_enumComplete = true;
}



///////////////////////////////////////////////////////////////////////////////
///	Vector
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/// 
///
void SingleIterDecoder::decodeVector(const Vector& input)
{
	_singleReadIter.start(static_cast<const Data&>(input));

	_out->outBegin(input);
	while(!_singleReadIter.off())
	{
		decodeVectorEntry((const VectorEntry&)_singleReadIter.value());	
		_singleReadIter.forth();
	}
	_out->outEnd(input);

}

///////////////////////////////////////////////////////////////////////////////
/// 
///
void SingleIterDecoder::decodeVectorEntry(const VectorEntry& input)
{
	_out->outBegin(input);
	decodeData(input.getData());
	_out->outEnd(input);
}

///////////////////////////////////////////////////////////////////////////////
///	Array
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/// 
///
void SingleIterDecoder::decodeArray(const Array& input)
{
	_singleReadIter.start(static_cast<const Data&>(input));

	_out->outBegin(input);
	while(!_singleReadIter.off())
	{
		decodeArrayEntry((const ArrayEntry&)_singleReadIter.value());	
		_singleReadIter.forth();
	}
	_out->outEnd(input);
}

///////////////////////////////////////////////////////////////////////////////
/// 
///
void SingleIterDecoder::decodeArrayEntry(const ArrayEntry& input)
{
	_out->outArrayEntryBegin(input);
	decodeData(input.getData());
	_out->outArrayEntryEnd(input);
}

///////////////////////////////////////////////////////////////////////////////
///	Series
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/// 
///
void SingleIterDecoder::decodeSeries(const Series& input)	
{
	_singleReadIter.start(input);

	_out->outBegin(input);

	if(input.getHintMask() & Map::SummaryDataFlag)
		decodeData(input.getSummaryData());

	if(input.getDataDefCount() > 0) // contains data definitions
		decodeDataDef(input.getDataDef());

	while(!_singleReadIter.off())
	{
		decodeSeriesEntry((const SeriesEntry&)_singleReadIter.value());	
		_singleReadIter.forth();
	}
	_out->outEnd(input);
}

void SingleIterDecoder::decodeSeriesEntry(const SeriesEntry& input)
{
	_out->outBegin(input);

	decodeData(input.getData());

	_out->outEnd(input);
}

///////////////////////////////////////////////////////////////////////////////
///	DataBuffer
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/// 
///
void SingleIterDecoder::decodeDataBuffer(const DataBuffer& dataBuffer)	
{

	switch(dataBuffer.getDataBufferType())
	{
	case DataBuffer::RespStatusEnum:
		_out->out(dataBuffer.getRespStatus());
		break;

	case DataBuffer::IntEnum:
	case DataBuffer::UIntEnum:
	case DataBuffer::FloatEnum:
	case DataBuffer::DoubleEnum:
	case DataBuffer::BufferEnum:
	case DataBuffer::RealEnum:
	case DataBuffer::StringAsciiEnum:
	case DataBuffer::StringUTF8Enum:
	case DataBuffer::StringRMTESEnum:
		_out->out(dataBuffer);
		break;

	case DataBuffer::DateEnum:
		_out->out(dataBuffer.getDate());
		break;

	case DataBuffer::TimeEnum:
		_out->out(dataBuffer.getTime());
		break;

	case DataBuffer::DateTimeEnum:
		_out->out(dataBuffer.getDateTime());
		break;

	case DataBuffer::QualityOfServiceInfoEnum:
		_out->out(dataBuffer.getQualityOfServiceInfo());
		break;

	case DataBuffer::EnumerationEnum:
		if(_currentEnumDef)
		{
			RDMEnumDef* tmpEnumDef = const_cast<RDMEnumDef*>(_currentEnumDef);
			RFA_String valFnd = tmpEnumDef->findEnumVal(dataBuffer.getEnumeration());
			_out->out(valFnd);
			char tmp[20];
			sprintf(tmp, " (%d)", dataBuffer.getEnumeration());
			_out->out(tmp);
		}
		else
			printf("Enumeration definition for this field does not exist: %d\n", dataBuffer.getInt());

		break;


	case DataBuffer::NoDataBufferEnum:
	case DataBuffer::UnknownDataBufferEnum:
	default:
		printf("SingleIterDecoder::decodeDataBuffer() - Unsupported decoding DataBufferType\n");
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///	Msg
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/// 
///

void SingleIterDecoder::decodeMsg(const Msg& msg)
{
	_out->outBegin(msg);

	switch (msg.getMsgType())
	{
	case rfa::message::ReqMsgEnum:
		decodeReqMsg(static_cast<const rfa::message::ReqMsg&>(msg));
		break;
	case rfa::message::RespMsgEnum:
		decodeRespMsg(static_cast<const rfa::message::RespMsg&>(msg));
		break;
	case rfa::message::GenericMsgEnum:
		decodeGenericMsg(static_cast<const rfa::message::GenericMsg&>(msg));
		break;
	case rfa::message::PostMsgEnum:
		decodePostMsg(static_cast<const rfa::message::PostMsg&>(msg));
		break;
	case rfa::message::AckMsgEnum:
		decodeAckMsg(static_cast<const rfa::message::AckMsg&>(msg));
		break;
	default:
		_out->out("SingleIterDecoder::decodeData() - Invalid message type.\n");
	}

	_out->outEnd(msg);
}

void SingleIterDecoder::decodeReqMsg(const ReqMsg& msg)
{
	_out->outBegin(msg);

	decodeData(msg.getPayload());

	_out->outEnd(msg);
}

void SingleIterDecoder::decodeRespMsg(const RespMsg& msg)
{
	_out->outBegin(msg);

	decodeData(msg.getPayload());

	_out->outEnd(msg);
}

void SingleIterDecoder::decodeGenericMsg(const GenericMsg& msg)
{
	_out->outBegin(msg);

	decodeData(msg.getPayload());

	_out->outEnd(msg);
}

void SingleIterDecoder::decodePostMsg(const PostMsg& msg)
{
	_out->outBegin(msg);

	decodeData(msg.getPayload());

	_out->outEnd(msg);
}

void SingleIterDecoder::decodeAckMsg(const AckMsg& msg)
{
	_out->outBegin(msg);

	decodeData(msg.getPayload());

	_out->outEnd(msg);
}



///////////////////////////////////////////////////////////////////////////////
///	decode main
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/// 
///
void SingleIterDecoder::decodeData(const Data& data, bool clearIter)
{
	if ( clearIter )
		_singleReadIter.clear();

	// decode based on dataType
	if(!data.isBlank())
	{
		switch(data.getDataType())
		{
		case DataBufferEnum:
			{
				const DataBuffer& input = static_cast<const DataBuffer&>(data);
				decodeDataBuffer(input);
				break;
			}
		case VectorEnum:
			{
				const Vector& input = static_cast<const Vector&>(data);
				decodeVector(input);
				break;
			}
		case FilterListEnum:
			{
				const FilterList& input = static_cast<const FilterList&>(data);
				decodeFilterList(input);
				break;
			}
		case ElementListEnum:
			{
				const ElementList& input = static_cast<const ElementList&>(data);
				decodeElementList(input);
				break;
			}
		case FieldListEnum:
			{
				const FieldList& input = static_cast<const FieldList&>(data);
				decodeFieldList(input);
				break;
			}	
		case SeriesEnum:
			{
				const Series& input = static_cast<const Series&>(data);
				decodeSeries(input);
				break;
			}
		case MapEnum:
			{
				const Map& input = static_cast<const Map&>(data);
				decodeMap(input);
				break;
			}
		case ArrayEnum:
			{
				const Array& input = static_cast<const Array&>(data);
				decodeArray(input);
				break;
			}
		case MsgEnum:
			{
				const Msg& msg = static_cast<const Msg&>(data);
				decodeMsg(msg);
				break;
			}
		case NoDataEnum:
			{
				break;
			}	
		default:
			printf("SingleIterDecoder::decodeData() - Unsupported data format.\n");
			break;
		}
	}
	//else blank data
}


///////////////////////////////////////////////////////////////////////////////
/// end of SingleIterDecoder.cpp
///////////////////////////////////////////////////////////////////////////////
