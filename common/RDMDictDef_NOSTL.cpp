#include "RDMDictDef_NOSTL.h"
#include <stdio.h>

RDMEnumDef::RDMEnumDef()
{
	_enumValMap.setHashFunction(&IntHashFunction);
}
RDMEnumDef::~RDMEnumDef()
{	
	_enumValMap.clear();
	this->_fids.clear();
}

void RDMEnumDef::dumpEnumDef() const
{

	RFA_Vector<Int16> _fidslist = fids();
	printf("Enum fid list: ");
	
	int sz = _fidslist.size();

	for(int i=0; i < sz; ++i)
	{
		printf("%d, ",_fidslist[i]);
	} 
	printf("\n");
	
	EnumValues _valsMap = values();
	int elmtSz = _valsMap.getSize();

	rfa::common::RFA_Vector<rfa::common::UInt16> aKey;
	rfa::common::RFA_Vector<RFA_String> aVal;

	printf("Enum value pairs: ");
	
	_valsMap.getKeyValues(aKey,aVal);

	for(int i=0; i < elmtSz; ++i)
	{
		printf("%d,%s ",aKey[i],aVal[i].c_str());
	}
	printf("\n");
}

unsigned int RDMEnumDef::IntHashFunction(const unsigned short &key)
{
	return(key % 101 + 1);
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
	RDMEnumDef* tmp = const_cast<RDMEnumDef*>(_pEnumDef);
	enumStr = tmp ? tmp->findEnumVal(enumVal) : RDMEnumDef::getEmptyEnumString();
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
	TRY_DATA_TYPE_AND_RETURN_EX(INT32,Int32);
	TRY_DATA_TYPE_AND_RETURN_EX(UINT32,UInt32);
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

	TRY_DATA_TYPE_AND_RETURN_EX(REAL32 ,Real32);
	TRY_DATA_TYPE_AND_RETURN_EX(REAL64 ,Real64);

	// try new data type REAL with length
	TRY_DATA_TYPE_AND_RETURN_EX(REAL,Real64);
	
	TRY_DATA_TYPE_AND_RETURN_EX(DATE,Date);
	TRY_DATA_TYPE_AND_RETURN_EX(DATETIME,DateTime);

	TRY_DATA_TYPE_AND_RETURN_EX(BUFFER, Buffer);

	TRY_DATA_TYPE_AND_RETURN_EX(ASCII_STRING, StringAscii);
	TRY_DATA_TYPE_AND_RETURN_EX(RMTES_STRING, StringRMTES);
	
	TRY_DATA_TYPE_AND_RETURN_EX(UTF8_STRING, StringUTF8);

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
	printf("%d    %s   %s   %s   %d   %d   %d   %d   %d\n",
		getFieldId(),getName().c_str(),getDisplayName().c_str(),
		getRipplesToFieldName().c_str(),getMFFieldType(),
		getMFFieldLength(),getMFEnumLength(),getMaxFieldLength(),
		getDataType());
}
