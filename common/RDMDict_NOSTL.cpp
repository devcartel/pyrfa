#ifdef WIN32
#pragma warning(disable:  4786)
#endif

#include "RDMDict_NOSTL.h"
#include "RDM/RDM.h"
#include "Data/Series.h"
#include "Data/Array.h"

#include <stdio.h>
#include <stdlib.h>
#include <new>

using namespace rfa::rdm;

RDMDict::RDMDict() :
	_isComplete( false ),
	_dictId( 0 ),
	_dictType( DICTIONARY_UNSPECIFIED )
{
}

RDMFieldDict::RDMFieldDict() :
	_count(0),
	_maxPosFieldId(0),
	_minNegFieldId(0),
	_posFieldDefinitionsSize(0),
	_negFieldDefinitionsSize(0),
	_posFieldDefinitions(0),
	_negFieldDefinitions(0),
	_maxFieldLength(0)
{
	resizePosFieldDefinitions(4000);
	_dictType = DICTIONARY_FIELD_DEFINITIONS;
	
	// Point the hash function to be used by HashTableSample
	_fidByNameMap.setHashFunction(&StrHashFunction);
}

RDMFieldDict::~RDMFieldDict()
{
	for( UInt16 i = 1; i < _posFieldDefinitionsSize; i++ )
	{
		if ( _posFieldDefinitions[i] )
			delete _posFieldDefinitions[i];
	}

	for( UInt16 j = 1; j < _negFieldDefinitionsSize; j++)
	{
		if ( _negFieldDefinitions[j] )
			delete _negFieldDefinitions[j];
	}

	_fidByNameMap.clear();

	if ( _posFieldDefinitions ) delete [] _posFieldDefinitions;
	if ( _negFieldDefinitions ) delete [] _negFieldDefinitions;
}

void RDMFieldDict::putFieldDef( const RDMFieldDef *pFieldDef )
{
	Int fieldId = pFieldDef->getFieldId();
	
	// map name to fieldId
	_fidByNameMap.put( pFieldDef->getName(), fieldId );
	
	if ( fieldId > 0 )
	{
		// need to resize the array
		if ( fieldId >= _posFieldDefinitionsSize ) resizePosFieldDefinitions( fieldId );

		// keep track of the largest "positive" fieldId
		if ( _maxPosFieldId < fieldId )	_maxPosFieldId = fieldId;

		// count number of unique field ids
		if ( _posFieldDefinitions[fieldId] == 0 ) _count++;

		_posFieldDefinitions[fieldId] = pFieldDef;
	}
	else
	{
		Int posFieldId = -fieldId;

		if ( posFieldId >= _negFieldDefinitionsSize )
		{
			resizeNegFieldDefinitions( posFieldId );
		}

		// keep track of the largest "negative" fieldId
		if ( _minNegFieldId > fieldId ) _minNegFieldId = fieldId;

		// count number of unique field ids
		if ( _negFieldDefinitions[posFieldId] == 0 ) _count++;

		_negFieldDefinitions[posFieldId] = pFieldDef;
	}
	if ( _maxFieldLength < pFieldDef->getMaxFieldLength() )
        _maxFieldLength = pFieldDef->getMaxFieldLength();
}

void RDMFieldDict::resizePosFieldDefinitions( Int fieldId )
{
	Int newDefSize = fieldId + 1;
	if ( newDefSize < _posFieldDefinitionsSize * 2 )
	{
		newDefSize = _posFieldDefinitionsSize * 2;
	}

	const RDMFieldDef** newDefs = new const RDMFieldDef*[(unsigned int)newDefSize];
	Int32 i = 1;
	for ( ; i < _posFieldDefinitionsSize; i++ )
		newDefs[i] = _posFieldDefinitions[i];

	for ( ; i < newDefSize; i++ )
		newDefs[i] = 0;

	newDefs[0] = 0;

	if ( _posFieldDefinitions ) delete [] _posFieldDefinitions;

	_posFieldDefinitions = newDefs;
	_posFieldDefinitionsSize = newDefSize;
}

void RDMFieldDict::resizeNegFieldDefinitions( Int posFieldId )
{
	Int newDefSize = posFieldId + 1;
	if ( newDefSize < _negFieldDefinitionsSize * 2 )
	{
		newDefSize = _negFieldDefinitionsSize * 2;
	}

	const RDMFieldDef ** newDefs = new const RDMFieldDef*[(unsigned int)newDefSize];
	Int i = 1;
	for (; i < _negFieldDefinitionsSize; i++)
		newDefs[i] = _negFieldDefinitions[i];
	for (; i < newDefSize; i++)
		newDefs[i] = 0;
	newDefs[0] = 0;

	if (_negFieldDefinitions ) delete [] _negFieldDefinitions;

	_negFieldDefinitions = newDefs;
	_negFieldDefinitionsSize = newDefSize;
}

void	RDMFieldDict::associateEnumDict(  )
{
	UInt16 i;

	for( i = 1; i <= _maxPosFieldId; i++)
	{
		const RDMFieldDef * def = _posFieldDefinitions[i];
		if (!def)
			continue;
		if (def->getDataType() == DataBuffer::EnumerationEnum )
			def->setEnumDef( _enumDict.findEnumDef( i ) );
	}

	Int16 j;

	for( j = -1; j > _minNegFieldId; j--)
	{
		const RDMFieldDef * def = _negFieldDefinitions[-j];
		if (!def)
			continue;
		if (def->getDataType() == DataBuffer::EnumerationEnum )
			def->setEnumDef( _enumDict.findEnumDef( j ) );
	}

	if (Trace & 0x2)
		printf("enumeration tables associated\n");

	_isComplete = true;
}

unsigned int RDMFieldDict::StrHashFunction(const rfa::common::RFA_String &key)
{
	unsigned long result = 0;
	int magic = 8388593;			// the greatest prime lower than 2^23

	const char *s = key.c_str();
	int n = key.length();
	while (n--)
		result = ((result % magic) << 8) + (unsigned int) *s++;
	return (unsigned int)result;
}

const RDMFieldDef * RDMFieldDict::getFieldDef( const RFA_String & fieldName ) 
{
	Int tmpId;
	bool fnd = _fidByNameMap.get(fieldName,tmpId);
	 
	Int fieldId = (fnd ? tmpId : 0);

	if ( fieldId < 0 ) {
		const RDMFieldDef * fieldDef = _negFieldDefinitions[ -fieldId ];
		return fieldDef;
	}
	else {
		const RDMFieldDef * fieldDef = _posFieldDefinitions[ fieldId ];
		return fieldDef;
	}

}

void	RDMFieldDict::fixRipple() 
{
	RDMFieldDef* def;

	for( UInt16 i = 1; i <= _maxPosFieldId; i++)
    {
		def = const_cast<RDMFieldDef*>(_posFieldDefinitions[i]);
		if (!def)
			continue;
		if (def->getRipplesToFid() == 0)
		{
			if (def->getRipplesToFieldName() == "")
			{
				continue;
			}
			else if (def->getRipplesToFieldName()=="NULL")
			{
				def->setRipplesToFid(0);
				def->setRipplesToFieldName(RFA_String("", 0 ,false));
			}
			else
			{
				const RDMFieldDef * rippleDef = getFieldDef(def->getRipplesToFieldName());
				if (rippleDef)
					def->setRipplesToFid(rippleDef->getFieldId());
			}
		}
		else
		{
			const RDMFieldDef * rippleDef = getFieldDef(def->getRipplesToFid());
			if (rippleDef)
				def->setRipplesToFieldName(rippleDef->getName());
		}
	}

	for( Int16 i = -1; i >= _minNegFieldId; i--)
    {
		def = const_cast<RDMFieldDef*>(_negFieldDefinitions[-i]);
		if (!def)
			continue;
		if (def->getRipplesToFid() == 0)
		{
			if (def->getRipplesToFieldName() == "")
			{
				continue;
			}
			else if (def->getRipplesToFieldName()=="NULL")
			{
				def->setRipplesToFid(0);
				def->setRipplesToFieldName(RFA_String("", 0, false));
			}
			else
			{
				const RDMFieldDef * rippleDef = getFieldDef(def->getRipplesToFieldName());
				if (rippleDef)
					def->setRipplesToFid(rippleDef->getFieldId());
			}
		}
		else
		{
			const RDMFieldDef * rippleDef = getFieldDef(def->getRipplesToFid());
			if (rippleDef)
				def->setRipplesToFieldName(rippleDef->getName());
		}
	}
	if (Trace & 0x2)
		printf("ripples associated\n");
}

RDMEnumDict::RDMEnumDict()
{
	_dictType = DICTIONARY_ENUM_TABLES;
	// this point to hash function to be used by HashtableSample
	_enumDefMap.setHashFunction(&IntHashFunction);
}

RDMEnumDict::~RDMEnumDict() 
{
	for( unsigned int i=0; i < _enumDefList.size(); ++i)
	{
		delete _enumDefList[i];
	}
	
	_enumDefMap.clear();
}

void RDMEnumDict::insertEnumDef( const RDMEnumDef * enumDef )
{	

	const RFA_Vector<Int16>  &fidIds = enumDef->fids();
	int sz = fidIds.size();

	for(int i=0; i < sz; ++i)
	{
		_enumDefMap.put(fidIds[i],enumDef);
	}
	_enumDefList.push_back( enumDef );
}

unsigned int RDMEnumDict::IntHashFunction(const unsigned short &key)
{
	return(key % 101 + 1);
}

rfa::common::RFA_String RDMEnumDef::_emptyEnumString = RFA_String( "Not provided", 12, false );
UInt32 RDMDict::Trace = 0;
