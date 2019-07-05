#ifndef __SESSION_LAYER_COMMON__RDMDict__H__
#define __SESSION_LAYER_COMMON__RDMDict__H__

/**
	\class RDMDict RDMDict.h 
	\brief
	Encapsultes data dictionaries and can populate from file or network.

*/

// RFA Includes
#include "Common/DataDef.h"
#include "Data/DataBuffer.h"
#include "Data/ElementList.h"
#include "RDMDictDef_NOSTL.h"
#include "HashMapSample.h"

// Forward declarations
class RDMFieldDict;
class RDMEnumDict;

typedef HashMapSample<unsigned short, const RDMEnumDef *> EnumTableMap;
typedef rfa::common::RFA_Vector<const RDMEnumDef*> EnumTables;

// Decoder
typedef HashMapSample<rfa::common::RFA_String, Int> FidMap;

using namespace rfa::common;
using namespace rfa::data;

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

		void		setDictId( Int dictId ) { _dictId = dictId; };
		Int			getDictId() const { return _dictId; }
		UInt16		getDictType() const { return _dictType; }

		bool		isComplete() const { return _isComplete; }

		void		setVersion( const rfa::common::RFA_String& version) { _version = version; };
		rfa::common::RFA_String getVersion() const {return _version;};

		static UInt32 Trace;

protected:
		bool					_isComplete;
		Int						_dictId;
		UInt16					_dictType;
		rfa::common::RFA_String _version;
};

// Enum Dictionary Declaration
class RDMEnumDict : public RDMDict
{
public:
		RDMEnumDict();
		virtual ~RDMEnumDict();
		void	insertEnumDef( const RDMEnumDef * enumDef );
		static unsigned int IntHashFunction(const unsigned short& key);
		const	RDMEnumDef	* findEnumDef( Int16 fieldId ) 
		{
			const RDMEnumDef* val;
			bool fnd = _enumDefMap.get(fieldId,val);
			return( fnd ? val : 0);
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

	const RDMFieldDef * getFieldDef( Int fieldId ) const
	{
		if ( fieldId > 0 )
			return (fieldId > _maxPosFieldId) ? 0 : _posFieldDefinitions[fieldId];
		else if (fieldId < 0)
			return (fieldId < _minNegFieldId) ? 0 : _negFieldDefinitions[-fieldId];
		else
			return 0;
	}

	const RDMFieldDef * getFieldDef( const RFA_String & fieldName ) ;

	RDMEnumDict & enumDict() { return _enumDict; }
	UInt16 maxFieldLength() const { return _maxFieldLength; }
	Int maxPositiveFieldId() const { return _maxPosFieldId; }
	Int minNegativeFieldId() const { return _minNegFieldId; }
	UInt16 count() const { return _count; }
	static unsigned int StrHashFunction(const rfa::common::RFA_String& key);

protected:
	friend class RDMFileDictionaryDecoder;
	friend class RDMNetworkDictionaryDecoder;
	void fixRipple();
	void associateEnumDict();

	void resizePosFieldDefinitions( Int fieldId );
	void resizeNegFieldDefinitions( Int posFieldId );

	UInt16					_count;
	Int					_maxPosFieldId;
	Int					_minNegFieldId;
	Int					_posFieldDefinitionsSize;
	Int					_negFieldDefinitionsSize;
	const RDMFieldDef **	_posFieldDefinitions;
	const RDMFieldDef **	_negFieldDefinitions;
	RDMEnumDict 			_enumDict;
	UInt16					_maxFieldLength;
	FidMap					_fidByNameMap;

private:
	RDMFieldDict( const RDMFieldDict & );
	RDMFieldDict operator=( const RDMFieldDict & );
};

#endif // __SESSION_LAYER_COMMON__RDMDict__H__
