#ifndef __SESSION_LAYER_COMMON__RDMDict__H__
#define __SESSION_LAYER_COMMON__RDMDict__H__

/**
	\class RDMDict RDMDict.h 
	\brief
	Encapsultes data dictionaries and can populate from file or network.

*/

// STL Includes
#include <map>
#include <list>
#include <vector>
#include <new>

// RFA Includes
#include "Common/DataDef.h"
#include "Data/DataBuffer.h"
#include "Data/ElementList.h"
#include "RDMDictDef.h"

// Forward declarations
class RDMFieldDict;
class RDMEnumDict;

typedef std::map<rfa::common::Int16, const RDMEnumDef *>	EnumTableMap;
typedef std::list<const RDMEnumDef*> EnumTables;

// Decoder
typedef std::map< rfa::common::RFA_String, rfa::common::Int >	FidMap;

// Auxilary class to prevent copying
class __NO_COPY
{
protected:
	__NO_COPY() {}
	virtual ~__NO_COPY() {}
private:
	__NO_COPY( const __NO_COPY & );
	__NO_COPY operator=( const __NO_COPY & );
};

// Base dict class
class RDMDict : private __NO_COPY
{
	public:
		RDMDict();

		void				setDictId( rfa::common::Int dictId ) { _dictId = dictId; };
		rfa::common::Int	getDictId() const { return _dictId; }
		rfa::common::UInt16	getDictType() const { return _dictType; }

		bool		isComplete() const { return _isComplete; }

		void		setVersion( const rfa::common::RFA_String& version) { _version = version; };
		rfa::common::RFA_String getVersion() const {return _version;};

		static rfa::common::UInt32 Trace;

protected:
		bool					_isComplete;
		rfa::common::Int		_dictId;
		rfa::common::UInt16		_dictType;
		rfa::common::RFA_String _version;
};

// Enum Dictionary Declaration
class RDMEnumDict : public RDMDict
{
public:
		RDMEnumDict();
		virtual ~RDMEnumDict();
		void	insertEnumDef( const RDMEnumDef * enumDef );

		const	RDMEnumDef	* findEnumDef( rfa::common::Int16 fieldId ) const
		{
			EnumTableMap::const_iterator cit = _enumDefMap.find( fieldId );
			return cit != _enumDefMap.end() ? cit->second : 0;
		}

		const EnumTableMap & enums() const
		{
			return _enumDefMap;
		}
		const EnumTables & enumList() const
		{
			return _enumDefList;
		}

protected:
		
		EnumTableMap		_enumDefMap;
		EnumTables			_enumDefList;

private:
	RDMEnumDict( const RDMEnumDict & );
	RDMEnumDict operator=( const RDMEnumDict & );
};

// Field Data Dictionary Declaration
class RDMFieldDict : public RDMDict
{
public:
	RDMFieldDict();
	virtual ~RDMFieldDict();
	void putFieldDef( const RDMFieldDef *pFieldDef );

	const RDMFieldDef * getFieldDef( rfa::common::Int fieldId ) const
	{
		if ( fieldId >= 0 )
			return (fieldId > _maxPosFieldId) ? 0 : _posFieldDefinitions[fieldId];
		else if (fieldId < 0)
			return (fieldId < _minNegFieldId) ? 0 : _negFieldDefinitions[-fieldId];
		else
			return 0;
	}

	const RDMFieldDef * getFieldDef( const rfa::common::RFA_String & fieldName ) const;

	RDMEnumDict& enumDict() { return _enumDict; }
	rfa::common::Int maxFieldLength() const { return _maxFieldLength; }
	rfa::common::Int maxLength() const { return _maxLength; }
	rfa::common::Int maxEnumLength() const { return _maxEnumLength; }

	rfa::common::Int maxPositiveFieldId() const { return _maxPosFieldId; }
	rfa::common::Int minNegativeFieldId() const { return _minNegFieldId; }
	rfa::common::UInt16 count() const { return _count; }

protected:

	friend class RDMFileDictionaryDecoder;
	friend class RDMNetworkDictionaryDecoder;
	void fixRipple();
	void associateEnumDict();

	void resizePosFieldDefinitions( rfa::common::Int fieldId );
	void resizeNegFieldDefinitions( rfa::common::Int posFieldId );

	rfa::common::UInt16		_count;
	rfa::common::Int		_maxPosFieldId;
	rfa::common::Int		_minNegFieldId;
	rfa::common::Int		_posFieldDefinitionsSize;
	rfa::common::Int		_negFieldDefinitionsSize;
	const RDMFieldDef **	_posFieldDefinitions;
	const RDMFieldDef **	_negFieldDefinitions;
	RDMEnumDict 			_enumDict;
	rfa::common::UInt16		_maxFieldLength;
	rfa::common::Int		_maxLength;
	rfa::common::Int		_maxEnumLength;
	FidMap					_fidByNameMap;

private:

	RDMFieldDict( const RDMFieldDict & );
	RDMFieldDict operator=( const RDMFieldDict & );
};

#endif // __SESSION_LAYER_COMMON__RDMDict__H__
