#ifdef WIN32
#pragma warning(disable:  4786)
#endif

#include "RDMDict.h"
#include "RDM/RDM.h"
#include "Data/Series.h"
#include "Data/Array.h"

#include <iostream>
#include <stdlib.h>
#include <new>

using namespace std;
using namespace rfa::rdm;
using namespace rfa::common;
using namespace rfa::data;

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
	_maxFieldLength(0),
	_maxLength(0),
	_maxEnumLength(0)
{
	resizePosFieldDefinitions(4000);
	_dictType = DICTIONARY_FIELD_DEFINITIONS;
}

RDMFieldDict::~RDMFieldDict()
{
	for ( UInt16 i = 0; i < _posFieldDefinitionsSize; i++ )
	{
		if ( _posFieldDefinitions[i] ) 
			delete _posFieldDefinitions[i];
	}

	for ( UInt16 j = 1; j < _negFieldDefinitionsSize; j++)
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
	_fidByNameMap.insert( FidMap::value_type( pFieldDef->getName(), fieldId ) );

	if ( fieldId >= 0 )
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

	if ( _maxLength < pFieldDef->getMFFieldLength() )
        _maxLength = pFieldDef->getMFFieldLength();

	if ( _maxEnumLength < pFieldDef->getMFEnumLength() )
        _maxEnumLength = pFieldDef->getMFEnumLength();
}

void RDMFieldDict::resizePosFieldDefinitions( Int fieldId )
{
	Int newDefSize = fieldId + 1;
	if ( newDefSize < _posFieldDefinitionsSize * 2 )
	{
		newDefSize = _posFieldDefinitionsSize * 2;
	}

	const RDMFieldDef** newDefs = new const RDMFieldDef*[(unsigned int)newDefSize];
	Int i = 0;
	for ( ; i < _posFieldDefinitionsSize; i++ )
		newDefs[i] = _posFieldDefinitions[i];

	for ( ; i < newDefSize; i++ )
		newDefs[i] = 0;

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
	for ( ; i < _negFieldDefinitionsSize; i++ )
		newDefs[i] = _negFieldDefinitions[i];

	for ( ; i < newDefSize; i++ )
		newDefs[i] = 0;

	if ( _negFieldDefinitions ) delete [] _negFieldDefinitions;

	_negFieldDefinitions = newDefs;
	_negFieldDefinitionsSize = newDefSize;
}

void RDMFieldDict::associateEnumDict(  )
{
	UInt16 i;

	for( i = 0; i <= _maxPosFieldId; i++)
	{
		const RDMFieldDef * def = _posFieldDefinitions[i];
		if (!def)
			continue;
		if (def->getDataType() == DataBuffer::EnumerationEnum )
			def->setEnumDef( _enumDict.findEnumDef( i ) );
	}

	for(UInt16 j = 1; j <= -_minNegFieldId; j++)
	{
		const RDMFieldDef * def = _negFieldDefinitions[j];
		if (!def)
			continue;
		if (def->getDataType() == DataBuffer::EnumerationEnum )
			def->setEnumDef( _enumDict.findEnumDef( j ) );
	}

	if (Trace & 0x2)
		cout << "enumeration tables associated" << endl;
	_isComplete = true;
}

const RDMFieldDef* RDMFieldDict::getFieldDef( const RFA_String & fieldName ) const
{
	FidMap::const_iterator cit = _fidByNameMap.find( fieldName );

	Int fieldId = ( cit != _fidByNameMap.end() ? cit->second : 0 );

	if ( fieldId < 0 ) {
		const RDMFieldDef * fieldDef = _negFieldDefinitions[ -fieldId ];
		return fieldDef;
	}
	else {
		const RDMFieldDef * fieldDef = _posFieldDefinitions[ fieldId ];
		return fieldDef;
	}

}

void RDMFieldDict::fixRipple() 
{
	RDMFieldDef* def;

	for( UInt16 i = 0; i <= _maxPosFieldId; i++)
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

	for(UInt16 j = 1; j <= -_minNegFieldId; j++)
    {
		def = const_cast<RDMFieldDef*>(_negFieldDefinitions[j]);
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
		cout << "ripples associated" << endl;
}

RDMEnumDict::RDMEnumDict()
{
	_dictType = DICTIONARY_ENUM_TABLES;
}

RDMEnumDict::~RDMEnumDict() 
{
	for( EnumTables::iterator it = _enumDefList.begin();
		 it != _enumDefList.end();
		 delete *it, ++it );

}

void RDMEnumDict::insertEnumDef( const RDMEnumDef* enumDef )
{		
	const FIDList& fidIds = enumDef->fids();
	for ( FIDList::const_iterator cit = fidIds.begin();
		 cit != fidIds.end();
		 ++cit )
		 _enumDefMap.insert( EnumTableMap::value_type( *cit, enumDef ) );
	
	_enumDefList.push_back( enumDef );
}

RFA_String RDMEnumDef::_emptyEnumString = RFA_String( "Not provided", 13, false );
UInt32 RDMDict::Trace = 0;
