#ifndef _RFA_EXAMPLE_RDMDictionaryEncoder_h
#define _RFA_EXAMPLE_RDMDictionaryEncoder_h

#include "RDMDict.h"
#include "Data/SeriesWriteIterator.h"
#include "Data/ElementListDef.h"

/**
	\class RDMDictionaryEncoder RDMDictionaryEncoder.h 
	\brief
	RDMDictionaryEncoder is an class to encodes RWFFld Field Dictionary 
	and RWFEnum Enum Tables Dictionary for sending over a network.
*/

class RDMDictionaryEncoder
{
public:

	enum OptimizeEnum
	{
		Speed = 0,
		Size = 1,
		Fragment = 2
	};
	
	RDMDictionaryEncoder();
	virtual ~RDMDictionaryEncoder();

	void encodeFieldDictionary(rfa::data::Series & series, const RDMFieldDict & dictionary,
		rfa::common::UInt32 verbosity, OptimizeEnum optimize = Size, long maxFragmentSize = 200000 );
	void encodeEnumDictionary(rfa::data::Series & series, const RDMEnumDict & dictionary, 
		rfa::common::UInt32 verbosity, OptimizeEnum optimize = Size, long maxFragmentSize = 200000);
	
	void continueEncoding(rfa::data::Series & series);

	bool isComplete() const { return _isComplete; }

protected:

	const RDMFieldDict * _fieldDict;
	const RDMEnumDict * _enumDict;

	// state used for fragmentation
	rfa::data::ElementList * _elementList;
	rfa::data::ElementListDef * _elementListDef;
	EnumTables::const_iterator _enumIter;
	bool _isComplete;
	rfa::common::UInt32 _verbosity;
	rfa::common::UInt32 _type;
	rfa::common::Int16 _fieldId;
	rfa::common::UInt16 _tableIndex;

	long _maxFragmentSize;
	int _maxSeriesEntrySize;
	OptimizeEnum _optimize;

	void encodeFieldDictionary( const RDMFieldDict & dictionary, rfa::common::UInt32 verbosity, rfa::data::Series & series );

	void encodeFieldDictionaryDataDef( rfa::common::Int maxLength,	// for "LENGTH"
								rfa::common::Int maxFieldLength,		// for "RWFLEN"
								rfa::common::Int maxEnumLength,		// for "ENUMLENGTH"
								rfa::common::UInt32 verbosity,
								rfa::data::ElementListDef& eld,
								rfa::data::Series& series );

	void encodeFieldDictionaryDataDef( const rfa::data::ElementListDef& eld, rfa::data::Series& series );

	void encodeFieldDictionarySummaryData( const RDMFieldDict & dictionary, rfa::data::Series & series );

	void encodeFieldDictionaryFields( rfa::common::Int16 startFid,
			rfa::data::SeriesWriteIterator & switer, 
			rfa::data::SeriesEntry & sentry, 
			const RDMFieldDict & dictionary, 
			rfa::common::UInt32 verbosity,
			rfa::data::ElementList & elementList, 
			const rfa::data::ElementListDef & eld, 
			rfa::data::Series & series);
	void encodeFieldDictionaryFidDef(rfa::data::DataBuffer::DataBufferEncodedEnumeration flen,
			rfa::data::DataBuffer::DataBufferEncodedEnumeration len,
			rfa::data::DataBuffer::DataBufferEncodedEnumeration elen,
			rfa::common::UInt32 verbosity, const rfa::data::ElementListDef & eld, 
			const RDMFieldDef *, rfa::data::ElementList & fidDef);

	void encodeEnumDictionary(const RDMEnumDict & dictionary, rfa::common::UInt32 verbosity, rfa::data::Series & series);
	void encodeEnumDictionaryDataDef(rfa::data::ElementListDef & eld, rfa::data::Series & series);
	void encodeEnumDictionaryDataDef(const rfa::data::ElementListDef & eld, rfa::data::Series & series);
	void encodeEnumDictionaryTables(EnumTables::const_iterator & iter, 
			rfa::data::SeriesWriteIterator & switer, 
			rfa::data::SeriesEntry & sentry, 
			const RDMEnumDict & dictionary,
			rfa::data::ElementList & elementList, 
			const rfa::data::ElementListDef & eld,
			rfa::data::Series & series);
	void encodeEnumDictionaryEnumTable(const EnumTables::const_iterator & iter, 
			const rfa::data::ElementListDef & eld,	rfa::data::ElementList & enumTable);

	void encodeDictionarySummaryData(const RDMDict & dictionary, 
		rfa::common::UInt32 type, rfa::data::Series & series, rfa::data::ElementList & list);


private:

};

#endif
