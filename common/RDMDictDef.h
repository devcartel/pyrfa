#ifndef __SESSION_LAYER_COMMON__RDMFieldDef__H__
#define __SESSION_LAYER_COMMON__RDMFieldDef__H__

// RFA Includes
#include "Data/DataBuffer.h"

#include <list>
#include <map>

typedef std::map< rfa::common::UInt16, rfa::common::RFA_String > EnumValues;
typedef rfa::common::Int16	FID;
typedef std::list< FID >	FIDList;

// Enum Definitiona Declaration
class RDMEnumDef
{
public:

	void setDisplayName( rfa::common::RFA_String& s ) { _enumString = s; }

	void setEnumVal( rfa::common::UInt16 val ) { _enumVal = val; }

	void insertEnumVal()
	{
		_enumValMap.insert( EnumValues::value_type( _enumVal, _enumString ) );
	}	

	void insertEnumVal( rfa::common::UInt16 enumVal, const rfa::common::RFA_String& enumString )
	{
		_enumString = enumString;
		_enumValMap.insert( EnumValues::value_type( enumVal, _enumString ) );
	}

	const rfa::common::RFA_String& findEnumVal( rfa::common::UInt16 enumVal ) const
	{
		EnumValues::const_iterator cit = _enumValMap.find( enumVal );

		return ( cit != _enumValMap.end() ) ? cit->second : _emptyEnumString;
	}

	static const rfa::common::RFA_String& getEmptyEnumString() { return _emptyEnumString; }

	const EnumValues& values() const { return _enumValMap; }

	const FIDList& fids() const { return _fids; }

	void addFid( FID fid ) { _fids.push_back( fid ); }

	void dumpEnumDef() const;

protected:

	EnumValues							_enumValMap;
	FIDList								_fids;

	static rfa::common::RFA_String		_emptyEnumString;
	rfa::common::RFA_String				_enumString;
	rfa::common::UInt16					_enumVal;
};


// Field Definitition Declaration
class RDMFieldDef
{
public:
		RDMFieldDef();
		RDMFieldDef( const rfa::common::RFA_String & name, 
					rfa::data::DataBuffer::DataBufferEnumeration dataType,
					const rfa::common::RFA_String & displayName );


		void	setFieldId( rfa::common::Int fieldId ) { _fieldId = fieldId; }
		const	rfa::common::Int getFieldId() const { return _fieldId; }

		void	setName( const rfa::common::RFA_String & name ) { _name = name; }
		const	rfa::common::RFA_String & getName() const { return _name; }

        void    setDisplayName( const rfa::common::RFA_String & displayName ) { _displayName = displayName; }
        const   rfa::common::RFA_String & getDisplayName() const { return _displayName; }

        void    setRipplesToFieldName( const rfa::common::RFA_String & rippleFieldName ) { _rippleFieldName = rippleFieldName; }
        void    setRipplesToFid( rfa::common::Int rippleFieldFid ) { _rippleFieldFid = rippleFieldFid; }
        const   rfa::common::RFA_String & getRipplesToFieldName() const { return _rippleFieldName; }
        const   rfa::common::Int getRipplesToFid() const { return _rippleFieldFid; }

        void    setMFFieldTypeName( const rfa::common::RFA_String & mfFieldTypeName); 
        void    setMFFieldType( rfa::common::Int fieldType) { _mfFieldType = fieldType; }
        const   rfa::common::Int	getMFFieldType() const { return _mfFieldType; }

        void    setMFFieldLength( rfa::common::Int mfFieldLength) { _mfFieldLength = mfFieldLength; }
        const   rfa::common::Int  getMFFieldLength() const { return _mfFieldLength; }

        void    setMFEnumLength( rfa::common::Int mfEnumLength) { _mfEnumLength = mfEnumLength; }
        const   rfa::common::Int  getMFEnumLength() const { return _mfEnumLength; }

        rfa::common::UInt	getDataType() const { return _dataType; }

		void	setDataType( const rfa::common::RFA_String & dataTypeStr, int length = 0 );
		void	setDataType( rfa::common::UInt dataType );

        void    setMaxFieldLength( rfa::common::UInt16  maxFieldLength) { _maxFieldLength = maxFieldLength; }
        const   rfa::common::UInt16  getMaxFieldLength() const { return _maxFieldLength; }

		// EnumDef is just referenced so it's not included into "constness"
		void	 setEnumDef( const RDMEnumDef* pEnumDef ) { _pEnumDef = pEnumDef; }
		void	 setEnumDef( const RDMEnumDef* pEnumDef ) const { _pEnumDef = pEnumDef; }
		const	 RDMEnumDef* getEnumDef() const { return _pEnumDef; }		

		void	getEnumString ( rfa::common::Int16 enumVal, rfa::common::RFA_String & enumStr ) const;

		void dumpFieldDef() const;

private:
		rfa::common::RFA_String				_name;
		rfa::common::Int					_fieldId;
		rfa::common::UInt					_dataType;
		rfa::common::RFA_String				_displayName;
		rfa::common::RFA_String				_rippleFieldName;
		rfa::common::Int					_rippleFieldFid;
        rfa::common::Int					_mfFieldType;
        rfa::common::Int					_mfFieldLength;
        rfa::common::Int					_mfEnumLength;
        rfa::common::UInt16					_maxFieldLength;
        
		mutable const RDMEnumDef			*_pEnumDef;
};

#endif
