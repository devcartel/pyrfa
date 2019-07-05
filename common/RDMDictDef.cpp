#include "RDMDictDef.h"

#include <iostream>

using namespace std;
using namespace rfa::common;
using namespace rfa::data;

void RDMEnumDef::dumpEnumDef() const
{
	const FIDList& fidslist = fids();
	FIDList::const_iterator it;

	cout << "Enum fid list: ";
	
	for ( it = fidslist.begin(); it != fidslist.end(); ++it )
	{
		cout << *it << ","; 
	}

	cout << endl;

	const EnumValues& _valsMap = values();
	EnumValues::const_iterator vIt;
	
	cout<<"Enum value pairs: ";
	
	for ( vIt = _valsMap.begin(); vIt!=_valsMap.end(); ++vIt )
	{
		cout << vIt->first << "," <<vIt->second.c_str()<<" "; // each element on a separate line
	} 

	cout << endl;
}

RDMFieldDef::RDMFieldDef() : 
	_dataType( DataBuffer::NoDataBufferEnum ),
	_rippleFieldFid(0),
	_mfFieldLength(0),
	_mfEnumLength(0),
	_maxFieldLength(0),
	_pEnumDef(0)
{
}

RDMFieldDef::RDMFieldDef( const rfa::common::RFA_String & name, 
			  rfa::data::DataBuffer::DataBufferEnumeration dataType,
			  const rfa::common::RFA_String & displayName ) :
	_name(name),
	_dataType(dataType),
	_displayName(displayName),
	_mfFieldLength(0),
	_mfEnumLength(0),
	_maxFieldLength(0),
	_pEnumDef(0)
{
}

void RDMFieldDef::getEnumString ( rfa::common::Int16 enumVal, rfa::common::RFA_String & enumStr ) const 
{
	enumStr = _pEnumDef ? _pEnumDef->findEnumVal( enumVal ) : RDMEnumDef::getEmptyEnumString(); 
}

#define TRY_FIELD_TYPE_AND_RETURN_EX(VAL,ENUM) \
		if( mfFieldTypeName == #VAL ) { _mfFieldType = ENUM; return; } else ((void)0)

void RDMFieldDef::setMFFieldTypeName( const rfa::common::RFA_String& mfFieldTypeName )
{
	TRY_FIELD_TYPE_AND_RETURN_EX(TIME_SECONDS, 0);
	TRY_FIELD_TYPE_AND_RETURN_EX(INTEGER, 1);
	TRY_FIELD_TYPE_AND_RETURN_EX(NUMERIC, 2);
	TRY_FIELD_TYPE_AND_RETURN_EX(DATE, 3);
	TRY_FIELD_TYPE_AND_RETURN_EX(PRICE, 4);
	TRY_FIELD_TYPE_AND_RETURN_EX(ALPHANUMERIC, 5);
	TRY_FIELD_TYPE_AND_RETURN_EX(ENUMERATED, 6);
	TRY_FIELD_TYPE_AND_RETURN_EX(TIME, 7);
	TRY_FIELD_TYPE_AND_RETURN_EX(BINARY, 8);
	TRY_FIELD_TYPE_AND_RETURN_EX(LONG_ALPHANUMERIC, 9);
	TRY_FIELD_TYPE_AND_RETURN_EX(OPAQUETYPE, 10);
	_mfFieldType = -1; // Unknown
}

#define TRY_DATA_TYPE_AND_RETURN_EX(VAL,ENUM) \
		if( dataTypeStr == #VAL ) { _dataType = DataBuffer:: ENUM##Enum; return; } else ((void)0)
#define TRY_DATA_FORMAT_AND_RETURN_EX(VAL,ENUM) \
		if( dataTypeStr == #VAL ) { _dataType =  ENUM##Enum; return; } else ((void)0)
#define	TRY_DATA_TYPE_AND_RETURN(VAL)  \
		if( dataTypeStr == #VAL ) { _dataType = DataBuffer:: VAL##Enum; return; } else ((void)0)

//maps from RWF string found in appendix_a to RFA DataBufferEnums
void	RDMFieldDef::setDataType( const rfa::common::RFA_String& dataTypeStr, int length )
{
	//map RWF types in 0.3 of appendix_a to RFA types
	TRY_DATA_TYPE_AND_RETURN_EX(INT32,Int);
	TRY_DATA_TYPE_AND_RETURN_EX(UINT32,UInt);
	TRY_DATA_TYPE_AND_RETURN_EX(INT64,Int64);
	TRY_DATA_TYPE_AND_RETURN_EX(UINT64,UInt64);

	// try new data type UINT, INT with length
	TRY_DATA_TYPE_AND_RETURN_EX(INT,Int64);
	TRY_DATA_TYPE_AND_RETURN_EX(UINT,UInt64);
	
	TRY_DATA_TYPE_AND_RETURN_EX(ENUM,Enumeration);
	TRY_DATA_TYPE_AND_RETURN_EX(TIME,Time);

	TRY_DATA_TYPE_AND_RETURN_EX(FLOAT,Float);
	TRY_DATA_TYPE_AND_RETURN_EX(DOUBLE,Double);
	TRY_DATA_TYPE_AND_RETURN_EX(STATE,RespStatus);
	TRY_DATA_TYPE_AND_RETURN_EX(QOS, QualityOfServiceInfo);
	TRY_DATA_TYPE_AND_RETURN_EX(ANSI_PAGE,ANSI_Page);

	TRY_DATA_TYPE_AND_RETURN_EX(REAL32 ,Real);
	TRY_DATA_TYPE_AND_RETURN_EX(REAL64 ,Real64);

	// try new data type REAL with length
	TRY_DATA_TYPE_AND_RETURN_EX(REAL,Real64);
	
	TRY_DATA_TYPE_AND_RETURN_EX(DATE,Date);
	TRY_DATA_TYPE_AND_RETURN_EX(DATETIME,DateTime);
	TRY_DATA_TYPE_AND_RETURN_EX(BUFFER, Buffer);

	TRY_DATA_TYPE_AND_RETURN_EX(ASCII_STRING, StringAscii);
	TRY_DATA_TYPE_AND_RETURN_EX(RMTES_STRING, StringRMTES);
	
	TRY_DATA_TYPE_AND_RETURN_EX(UTF8_STRING, StringUTF8);

	TRY_DATA_TYPE_AND_RETURN_EX(OPAQUE, Opaque);

	TRY_DATA_FORMAT_AND_RETURN_EX(VECTOR, Vector);
	TRY_DATA_FORMAT_AND_RETURN_EX(FILTER_LIST, FilterList);
	TRY_DATA_FORMAT_AND_RETURN_EX(ELEMENT_LIST, ElementList);
	TRY_DATA_FORMAT_AND_RETURN_EX(FIELD_LIST, FieldList);
	TRY_DATA_FORMAT_AND_RETURN_EX(MAP, Map);
	TRY_DATA_FORMAT_AND_RETURN_EX(SERIES, Series);
	TRY_DATA_FORMAT_AND_RETURN_EX(ARRAY, Array);

	//no match
	_dataType =  DataBuffer::UnknownDataBufferEnum;
}

void RDMFieldDef::dumpFieldDef() const
{
	cout<<getFieldId()<<"	"
		<<getName().c_str()<<"	"
		<<getDisplayName().c_str()<<"	"
		<<getRipplesToFieldName().c_str()<<"	"
		<<getMFFieldType()<<"	"
		<<getMFFieldLength()<<"	"
		<<getMFEnumLength()<<"	"
		<<getMaxFieldLength()<<"	"
		<<getDataType()<<"	"
		<<endl;
}

void RDMFieldDef::setDataType( rfa::common::UInt dataType )
{
		switch ( dataType )
		{
			case DataBuffer::Int32Enum:
				_dataType = DataBuffer::IntEnum;
				break;
			case DataBuffer::UInt32Enum:
				_dataType = DataBuffer::UIntEnum;
				break;
			case DataBuffer::Real32Enum:
				_dataType = DataBuffer::RealEnum;
				break;
			default:
				_dataType = dataType;
				break;
		}
}
