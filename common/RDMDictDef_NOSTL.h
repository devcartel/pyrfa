#ifndef __SESSION_LAYER_COMMON__RDMFieldDef__H__
#define __SESSION_LAYER_COMMON__RDMFieldDef__H__

// RFA Includes
#include "Data/DataBuffer.h"
#include "HashMapSample.h"
#include "Common/RFA_Vector.h"

using namespace rfa::common;
using namespace rfa::data;

typedef HashMapSample<UInt16, rfa::common::RFA_String > EnumValues;

// Enum Definitiona Declaration
class RDMEnumDef
{
public:
		RDMEnumDef();
		~RDMEnumDef();
		void setDisplayName( rfa::common::RFA_String& s ) { _enumString = s; }

		void setEnumVal( UInt16 val ) { _enumVal = val; }

		static unsigned int IntHashFunction(const unsigned short& key);
		void insertEnumVal()
		{
			_enumValMap.put(_enumVal,_enumString);
		}	

		void insertEnumVal( UInt16 enumVal, const rfa::common::RFA_String& enumString )
		{
			_enumString = enumString;
			_enumValMap.put(enumVal,_enumString);
		}
		
		rfa::common::RFA_String findEnumVal( Int16 enumVal)
		{
			RFA_String val;
			bool fnd = _enumValMap.get(enumVal,val);
			return( fnd ? val : _emptyEnumString);
		}

		static const rfa::common::RFA_String& getEmptyEnumString() { return _emptyEnumString; }

		const EnumValues values() const
		{
			return _enumValMap;
		}

		const RFA_Vector<Int16> fids() const
		{
			return _fids;
		}
		void addFid( Int16 fid ) 
		{ 
			_fids.push_back(fid);
		}

		void dumpEnumDef() const;

protected:

		EnumValues				_enumValMap;
		rfa::common::RFA_Vector<Int16> _fids;

		static rfa::common::RFA_String		_emptyEnumString;
		rfa::common::RFA_String				_enumString;
		UInt16								_enumVal;
};


// Field Definitition Declaration
class RDMFieldDef
{
public:
		RDMFieldDef();
		RDMFieldDef( const rfa::common::RFA_String & name, 
					  DataBuffer::DataBufferEnumeration dataType,
					  const rfa::common::RFA_String & displayName );


		void	setFieldId( Int fieldId ) { _fieldId = fieldId; }
		const	Int getFieldId() const { return _fieldId; }

		void	setName( const rfa::common::RFA_String & name ) { _name = name; }
		const	rfa::common::RFA_String & getName() const { return _name; }

        void    setDisplayName( const rfa::common::RFA_String & displayName ) { _displayName = displayName; }
        const   rfa::common::RFA_String & getDisplayName() const { return _displayName; }

        void    setRipplesToFieldName( const rfa::common::RFA_String & rippleFieldName ) { _rippleFieldName = rippleFieldName; }
        void    setRipplesToFid( Int rippleFieldFid ) { _rippleFieldFid = rippleFieldFid; }
        const   rfa::common::RFA_String & getRipplesToFieldName() const { return _rippleFieldName; }
        const   Int getRipplesToFid() const { return _rippleFieldFid; }

        void    setMFFieldTypeName( const rfa::common::RFA_String & mfFieldTypeName); 
        void    setMFFieldType( Int fieldType) { _mfFieldType = fieldType; }
        const   Int	getMFFieldType() const { return _mfFieldType; }

        void    setMFFieldLength( Int mfFieldLength) { _mfFieldLength = mfFieldLength; }
        const   Int  getMFFieldLength() const { return _mfFieldLength; }

        void    setMFEnumLength( Int mfEnumLength) { _mfEnumLength = mfEnumLength; }
        const   Int  getMFEnumLength() const { return _mfEnumLength; }

        UInt	getDataType() const { return _dataType; }

		void	setDataType( const rfa::common::RFA_String & dataTypeStr, int length = 0 );
		void	setDataType( UInt dataType ) { _dataType = dataType; };

        void    setMaxFieldLength( UInt16  maxFieldLength) { _maxFieldLength = maxFieldLength; }
        const   UInt16  getMaxFieldLength() const { return _maxFieldLength; }

		// EnumDef is just referenced so it's not included into "constness"
		void	 setEnumDef( const RDMEnumDef* pEnumDef ) { _pEnumDef = pEnumDef; }
		void	 setEnumDef( const RDMEnumDef* pEnumDef ) const { _pEnumDef = pEnumDef; }
		const	 RDMEnumDef* getEnumDef() const { return _pEnumDef; }		

		void	getEnumString ( rfa::common::Int16 enumVal, rfa::common::RFA_String & enumStr ) const;

		void dumpFieldDef() const;

private:
		rfa::common::RFA_String				_name;
		Int									_fieldId;
		UInt								_dataType;
		rfa::common::RFA_String				_displayName;
		rfa::common::RFA_String				_rippleFieldName;
		Int								_rippleFieldFid;
        Int								_mfFieldType;
        Int								_mfFieldLength;
        Int								_mfEnumLength;
        UInt16								_maxFieldLength;
        
		mutable const RDMEnumDef			*_pEnumDef;
};

#endif
