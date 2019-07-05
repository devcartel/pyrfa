#ifndef _RFA_EXAMPLE_RDMDictionaryDecoder_h
#define _RFA_EXAMPLE_RDMDictionaryDecoder_h

#include "Data/ElementListReadIterator.h"
#include "../common/RDMDict.h"
#include "RDM/RDM.h"

/**
	\class RDMNetworkDictionaryDecoder RDMDictionaryDecoder.h 
	\brief
	RDMNetworkDictionaryDecoder is an class to decodes RWFFld Field Dictionary 
	and RWFEnum Enum Tables Dictionary that were downloaded from a network.
*/

class RDMNetworkDictionaryDecoder
{
public:
	RDMNetworkDictionaryDecoder(RDMFieldDict & dictionary, rfa::common::UInt32 dictDataMask = rfa::rdm::DICTIONARY_NORMAL);
	virtual ~RDMNetworkDictionaryDecoder();
	
	void	loadAppendix_A(const rfa::common::Data& data, bool moreFragments = false);
	void	loadEnumTypeDef(const rfa::common::Data& data, bool moreFragments = false);

	rfa::common::UInt32	getDictVerbosityType();

protected:
	bool appendixALoaded;
	bool enumTypeDefLoaded;

	void	associate();
	virtual bool decodeFirstFieldDef( const rfa::data::ElementList& elementList, rfa::data::ElementListReadIterator& it, RDMFieldDef* def );
	virtual bool decodeOtherFieldDef( const rfa::data::ElementList& elementList, rfa::data::ElementListReadIterator& it, RDMFieldDef* def );
	virtual void decodeFieldDefDictionary( const rfa::common::Data& data );
	void decodeFieldDefDictionarySummaryData( const rfa::data::ElementList& elementList, rfa::data::ElementListReadIterator& it );
	void decodeFieldDefDictionaryRow( const rfa::data::ElementList& elementList, rfa::data::ElementListReadIterator& it );

	void decodeEnumDefTables( const rfa::common::Data& data );
	void decodeEnumDefTablesSummaryData( const rfa::data::ElementList& elementList, rfa::data::ElementListReadIterator& it );
	void decodeEnumDefTable( const rfa::data::ElementList& elementList, rfa::data::ElementListReadIterator& it1, rfa::data::ElementListReadIterator& it2 );

	RDMFieldDict&			_dictionary;
	rfa::common::UInt32		_dictVerbosityType;
	rfa::common::DataDef*	_def;
	RDMEnumDict*			_enumDict;
	bool					_first;

	RDMEnumDef*				pEnumDef;

private:
	RDMNetworkDictionaryDecoder( const RDMNetworkDictionaryDecoder & );
	RDMNetworkDictionaryDecoder operator=( const RDMNetworkDictionaryDecoder & );
};

/**
	\class RDMFileDictionaryDecoder RDMDictionaryDecoder.h 
	\brief
	RDMFileDictionaryDecoder is an class to decodes RWFFld Field Dictionary 
	and RWFEnum Enum Tables Dictionary from files.
*/

class RDMFileDictionaryDecoder
{
public:
	RDMFileDictionaryDecoder(RDMFieldDict & dictionary);
	virtual ~RDMFileDictionaryDecoder();
	bool load( const rfa::common::RFA_String& appendix_a, const rfa::common::RFA_String& enumTypeDef );

   static rfa::common::RFA_String _DictVersion;
   static rfa::common::RFA_String _EnumVersion;
   static rfa::common::Int32      _DictId;
   static rfa::common::Int32      _EnumId;
protected:

	void	associate();
	bool    loadAppendix_A( const rfa::common::RFA_String& appendix_a );
	bool    loadEnumTypeDef( const rfa::common::RFA_String& enumTypeDef,  RDMEnumDict *pEnumDict );

	RDMEnumDict*	_enumDict;
	RDMFieldDict & _dictionary;

   

private:
	RDMFileDictionaryDecoder( const RDMFileDictionaryDecoder & );
	RDMFileDictionaryDecoder operator=( const RDMFileDictionaryDecoder & );
};

#endif
