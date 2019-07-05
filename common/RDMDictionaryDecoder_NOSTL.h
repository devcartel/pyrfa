#ifndef _RFA_EXAMPLE_RDMDictionaryDecoder_h
#define _RFA_EXAMPLE_RDMDictionaryDecoder_h

#include "Data/ElementListReadIterator.h"
#include "../Common/RDMDict_NOSTL.h"
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
	RDMNetworkDictionaryDecoder(RDMFieldDict & dictionary, UInt32 dictDataMask = rfa::rdm::DICTIONARY_NORMAL);
	virtual ~RDMNetworkDictionaryDecoder();
	
	void	loadAppendix_A(const Data& data, bool moreFragments = false);
	void	loadEnumTypeDef(const Data& data, bool moreFragments = false);

	UInt32	getDictVerbosityType();

protected:
	bool appendixALoaded;
	bool enumTypeDefLoaded;

	void	associate();
	virtual bool decodeFirstFieldDef( const ElementList& elementList, ElementListReadIterator& it, RDMFieldDef* def );
	virtual bool decodeOtherFieldDef( const ElementList& elementList, ElementListReadIterator& it, RDMFieldDef* def );
	virtual void decodeFieldDefDictionary( const Data& data );
	void decodeFieldDefDictionarySummaryData( const ElementList& elementList, ElementListReadIterator& it );
	void decodeFieldDefDictionaryRow( const ElementList& elementList, ElementListReadIterator& it );

	void decodeEnumDefTables( const Data& data );
	void decodeEnumDefTablesSummaryData( const ElementList& elementList, ElementListReadIterator& it );
	void decodeEnumDefTable( const ElementList& elementList, ElementListReadIterator& it1, ElementListReadIterator& it2 );

	RDMFieldDict & _dictionary;
	UInt16			_defId;
	UInt32			_dictVerbosityType;
	DataDef*			_def;
	DataDef*			_defTemp;
	RDMEnumDict*	_enumDict;
	bool			_first;

	RDMEnumDef *pEnumDef;

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
	bool load( const RFA_String& appendix_a, const RFA_String& enumTypeDef );

protected:

	void	associate();
	bool    loadAppendix_A( const RFA_String& appendix_a );
	bool    loadEnumTypeDef( const RFA_String& enumTypeDef,  RDMEnumDict *pEnumDict );
	static int getCopyQuotedStr( char * line, int curpos, char *copyToStr );
	static int getCopyUntilSpace( char * line, int curpos, char *copyToStr );
	static int getCopyPoundOrQuoteStr( bool& charQuote, char * line, int curpos, char *copyToStr );

	RDMEnumDict*	_enumDict;
	RDMFieldDict & _dictionary;

private:
	RDMFileDictionaryDecoder( const RDMFileDictionaryDecoder & );
	RDMFileDictionaryDecoder operator=( const RDMFileDictionaryDecoder & );
};

#endif
