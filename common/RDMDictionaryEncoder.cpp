#include "RDMDictionaryEncoder.h"

#include <iostream>
#include <assert.h>

#include "Data/DataBuffer.h"

#include "Data/SeriesEntry.h"

#include "Data/ArrayEntry.h"
#include "Data/ArrayWriteIterator.h"

#include "Data/ElementListWriteIterator.h"
#include "Data/ElementListDefWriteIterator.h"

#include "Data/DataDefWriteIterator.h"

#include "RDM/RDM.h"


using namespace std;
using namespace rfa::common;
using namespace rfa::data;
using namespace rfa::rdm;


RDMDictionaryEncoder::RDMDictionaryEncoder()
{
	_isComplete = false;
	_elementList = new ElementList();
	_elementListDef = new ElementListDef();
}

RDMDictionaryEncoder::~RDMDictionaryEncoder()
{
	delete _elementListDef;
	delete _elementList;
}

void RDMDictionaryEncoder::encodeFieldDictionary( Series& series, const RDMFieldDict& dictionary,
											UInt32 verbosity, OptimizeEnum optimize, long maxFragmentSize )
{
	_isComplete = false;
	_elementList->clear();
	_elementList->setAssociatedMetaInfo( series.getMajorVersion(), series.getMinorVersion() );
	_elementListDef->clear();

	_fieldDict = &dictionary;
	_type = DICTIONARY_FIELD_DEFINITIONS;
	_verbosity = verbosity;
	_optimize = optimize;
		// maxFragmentSize is ignored unless optimize == Fragment
	if (optimize == Fragment)
		_maxFragmentSize= maxFragmentSize;

	encodeFieldDictionary(dictionary, verbosity, series);
}

void RDMDictionaryEncoder::encodeEnumDictionary(Series & series, const RDMEnumDict & dictionary, UInt32 verbosity,
												 OptimizeEnum optimize, long maxFragmentSize )
{
	_tableIndex = 0;
	_isComplete = false;
	_elementList->clear();
	_elementList->setAssociatedMetaInfo(series.getMajorVersion(), series.getMinorVersion());
	_elementListDef->clear();

	_enumDict = &dictionary;
	_type = DICTIONARY_ENUM_TABLES;
	_verbosity = verbosity;
	_optimize = optimize;
		// maxFragmentSize is ignored unless optimize == Fragment
	if (optimize == Fragment)
		_maxFragmentSize= maxFragmentSize;

	encodeEnumDictionary(dictionary, verbosity, series);
}


void RDMDictionaryEncoder::continueEncoding(Series & series)
{
	assert(_optimize == Fragment);
	assert(!_isComplete);
	//should not call clear after reseting version	
	//series.clear();
	series.setIndicationMask(Series::DataDefFlag | Series::EntriesFlag);
	encodeFieldDictionaryDataDef(*_elementListDef, series);

		// write ElementList saved from previous fragment
	SeriesWriteIterator switer;
	switer.start(series);
	SeriesEntry sentry;
	sentry.setData(*_elementList);
	switer.bind(sentry);
	if (_type == DICTIONARY_ENUM_TABLES)
	{
		if (RDMDict::Trace & 0x4)
		{
			cout << "encoding saved table " << _tableIndex << ": ";
			int enumTableLength = _elementList->getEncodedBuffer().size();
			cout << enumTableLength << " bytes" << endl;
		}
		_enumIter++;
		_tableIndex++;
	}
	_elementList->clear();
	_elementList->setAssociatedMetaInfo(series.getMajorVersion(), series.getMinorVersion());
		// continue encoding
	if (_type == DICTIONARY_FIELD_DEFINITIONS)
		encodeFieldDictionaryFields(_fieldId, switer, sentry, *_fieldDict, _verbosity, *_elementList, *_elementListDef, series);
	else // _type == DICTIONARY_ENUM_TABLES
		encodeEnumDictionaryTables(_enumIter, switer, sentry, *_enumDict, *_elementList, *_elementListDef, series);

}




void RDMDictionaryEncoder::encodeFieldDictionaryDataDef( const ElementListDef& eld, Series& series )
{
	DataDefWriteIterator ddwiter;
	ddwiter.start( series );
	ddwiter.bind( eld );
	ddwiter.complete();
}


void RDMDictionaryEncoder::encodeFieldDictionaryDataDef(
	Int maxLength,		// for "LENGTH"
	Int maxFieldLength,	// for "RWFLEN"
	Int maxEnumLength,	// for "ENUMLENGTH"
	UInt32 verbosity,
	ElementListDef& eld,
	Series& series )
{
	DataDefWriteIterator ddwiter;
	eld.setDataDefID(0);
	ElementListDefWriteIterator elditer;
	ElementEntryDef def;

	ddwiter.start( series );

	RFA_String name( "NAME", 4, false );
	RFA_String fid( "FID", 3, false );
	RFA_String rippleto( "RIPPLETO", 8, false );
	RFA_String type( "TYPE", 4, false) ;
	RFA_String length( "LENGTH", 6, false );
	RFA_String rwftype( "RWFTYPE", 7, false );
	RFA_String rwflen( "RWFLEN", 6, false) ;
	RFA_String enumlength( "ENUMLENGTH", 10, false );
	RFA_String longname( "LONGNAME", 8, false );

	DataBuffer::DataBufferEncodedEnumeration lenghtSize = DataBuffer::Bit8ValueEnum;		// for "LENGTH"
	DataBuffer::DataBufferEncodedEnumeration rwflenSize = DataBuffer::Bit8ValueEnum;		// for "RWFLEN"
	DataBuffer::DataBufferEncodedEnumeration enumlengthSize = DataBuffer::Bit8ValueEnum;	// for "ENUMLENGTH"

	elditer.start(eld);
	
	def.setName( name );
	def.setType( DataBuffer::StringAsciiEnum );
	elditer.bind( def );

	def.setName( fid );
	def.setType( DataBuffer::Int32Enum, DataBuffer::Bit16ValueEnum );
	elditer.bind( def );

	def.setName( rippleto );
	def.setType( DataBuffer::Int32Enum, DataBuffer::Bit16ValueEnum );
	elditer.bind( def );

	def.setName( type );
	def.setType( DataBuffer::Int32Enum, DataBuffer::Bit8ValueEnum) ;
	elditer.bind( def );

	def.setName( length );
	if ( maxLength > 255 )
		lenghtSize = DataBuffer::Bit16ValueEnum;
	def.setType( DataBuffer::UInt32Enum, lenghtSize );
	elditer.bind( def );

	def.setName( rwftype );
	def.setType( DataBuffer::UInt32Enum, DataBuffer::Bit8ValueEnum );
	elditer.bind( def );

	def.setName( rwflen );
	if ( maxFieldLength > 255 )
		rwflenSize = DataBuffer::Bit16ValueEnum;
	def.setType( DataBuffer::UInt32Enum, rwflenSize );
	elditer.bind( def );

	if ( verbosity > DICTIONARY_MINIMAL )
	{
		def.setName( enumlength );
		if ( maxEnumLength > 255 )
			enumlengthSize = DataBuffer::Bit16ValueEnum;
		def.setType( DataBuffer::UInt32Enum, enumlengthSize );
		elditer.bind( def );

		def.setName( longname );
		def.setType( DataBuffer::StringAsciiEnum );
		elditer.bind( def );
	}

	elditer.complete();

	ddwiter.bind( eld );
	ddwiter.complete();
}

void RDMDictionaryEncoder::encodeFieldDictionaryFidDef(
	DataBuffer::DataBufferEncodedEnumeration flen,
	DataBuffer::DataBufferEncodedEnumeration len,
	DataBuffer::DataBufferEncodedEnumeration elen,
	UInt32 verbosity,
	const ElementListDef & eld,
	const RDMFieldDef * def,
	ElementList & definition)
{
	ElementListWriteIterator eliter;
	DataBuffer dataBuffer;
	RFA_String s;

	eliter.start(definition, DefinedDataFlag, &eld);

	s.set(def->getName().c_str());
	dataBuffer.setFromString(s, DataBuffer::StringAsciiEnum);
	eliter.bind(dataBuffer);
	Int t = def->getFieldId();
	dataBuffer.setInt(def->getFieldId(),DataBuffer::Bit16ValueEnum);
	eliter.bind(dataBuffer);
	t = def->getRipplesToFid();
	dataBuffer.setInt((Int&)t,DataBuffer::Bit16ValueEnum);
	eliter.bind(dataBuffer);	
	t = def->getMFFieldType();
	dataBuffer.setInt((Int&)t,DataBuffer::Bit8ValueEnum);
	eliter.bind(dataBuffer);
	UInt ut = def->getMFFieldLength();
	dataBuffer.setUInt(ut, len);
	eliter.bind(dataBuffer);
	ut = def->getDataType();
	dataBuffer.setUInt(ut,DataBuffer::Bit8ValueEnum);
	eliter.bind(dataBuffer);
	ut = def->getMaxFieldLength();
	dataBuffer.setUInt(ut, flen);
	eliter.bind(dataBuffer);
	if (verbosity > DICTIONARY_MINIMAL)
	{
		ut = def->getMFEnumLength();
		dataBuffer.setUInt(ut, elen);
		eliter.bind(dataBuffer);
		s.set(def->getDisplayName().c_str());
		dataBuffer.clear();
		dataBuffer.setFromString(s, DataBuffer::StringAsciiEnum);
		eliter.bind(dataBuffer);
	}
	eliter.complete();
}

void RDMDictionaryEncoder::encodeFieldDictionaryFields(Int16 startFid,
													   SeriesWriteIterator & switer,
													   SeriesEntry & sentry,
													   const RDMFieldDict & dictionary,
													   UInt32 verbosity, 
													   ElementList & elementList,
													   const ElementListDef & eld,
													   Series & series)
{
	Int32 maxCheck = startFid;

	for ( _fieldId = startFid; _fieldId <= dictionary.maxPositiveFieldId(); _fieldId++ )
	{
		if ( maxCheck > 0x7FFF ) break;
		maxCheck++;

		const RDMFieldDef* def = dictionary.getFieldDef(_fieldId);
		if ( !def ) continue;

		if ( _optimize == Speed )
		{
			sentry.setData( elementList );
			switer.bind( sentry );
		}

		DataBuffer::DataBufferEncodedEnumeration maxFieldLength = 
			(dictionary.maxFieldLength() > 255) ? DataBuffer::Bit16ValueEnum : DataBuffer::Bit8ValueEnum;

		DataBuffer::DataBufferEncodedEnumeration maxLength = 
			(dictionary.maxLength() > 255) ? DataBuffer::Bit16ValueEnum : DataBuffer::Bit8ValueEnum;

		DataBuffer::DataBufferEncodedEnumeration maxEnumLength = 
			(dictionary.maxEnumLength() > 255) ? DataBuffer::Bit16ValueEnum : DataBuffer::Bit8ValueEnum;

		encodeFieldDictionaryFidDef( maxFieldLength, maxLength, maxEnumLength , verbosity, eld, def, elementList );

		if (_optimize == Fragment)
		{
			long currentLength = series.getEncodedBuffer().size();
			long fidDefLen = elementList.getEncodedBuffer().size();
			if ( currentLength + fidDefLen + 20 > _maxFragmentSize )
			{
				switer.complete();
				if (RDMDict::Trace & 0x1)
				{
					cout << "encodeFieldDictionary fragment of size " 
						<< currentLength << " complete" << endl;
				}
				_isComplete = false;
				return;
			}
		}

		if (_optimize != Speed)
		{
			sentry.setData(elementList);
			switer.bind(sentry);
		}
		elementList.clear();
		sentry.clear();
	}
	switer.complete();
	if (RDMDict::Trace & 0x1)
	{
		cout << "encodeFieldDictionary fragment of size " 
			<< series.getEncodedBuffer().size() << " complete" << endl;
	}
	_isComplete = true;
}


void RDMDictionaryEncoder::encodeFieldDictionary( const RDMFieldDict& dictionary, UInt32 verbosity, Series& series )
{
	if (RDMDict::Trace & 0x1)
		cout << "encodeFieldDictionary" << endl;

	series.setTotalCountHint(dictionary.count());
	UInt8 indicationMask = Series::SummaryDataFlag;
	if (verbosity > DICTIONARY_INFO)
		indicationMask |=  Series::DataDefFlag | Series::EntriesFlag;
	series.setIndicationMask(indicationMask);
	
	ElementList summaryel;
	summaryel.setAssociatedMetaInfo( series.getMajorVersion(), series.getMinorVersion() );

	if ( _optimize != Speed )
		encodeDictionarySummaryData( dictionary, DICTIONARY_FIELD_DEFINITIONS, series, summaryel );

	if ( verbosity > DICTIONARY_INFO )
		encodeFieldDictionaryDataDef( dictionary.maxLength(), dictionary.maxFieldLength(), dictionary.maxEnumLength(), verbosity, *_elementListDef, series );

	if ( _optimize == Speed )
		encodeDictionarySummaryData(dictionary, DICTIONARY_FIELD_DEFINITIONS, series, summaryel);

	if ( verbosity > DICTIONARY_INFO )
	{
		SeriesWriteIterator switer;
		switer.start(series, dictionary.count());
		SeriesEntry sentry;
		encodeFieldDictionaryFields(static_cast<rfa::common::Int16>(dictionary.minNegativeFieldId()), switer, sentry, 
			dictionary, verbosity, *_elementList, *_elementListDef, series);
	}
		
	if (RDMDict::Trace & 0x1)
		cout << "encodeFieldDictionary complete" << endl;
}


void RDMDictionaryEncoder::encodeEnumDictionaryDataDef(const ElementListDef & eld, Series & series)
{
	DataDefWriteIterator ddwiter;
	ElementListDefWriteIterator elditer;
	ElementEntryDef def;
	
	ddwiter.start(series);
	ddwiter.bind(eld);

	ddwiter.complete();
}


void RDMDictionaryEncoder::encodeEnumDictionaryDataDef(ElementListDef & eld, Series & series)
{
	DataDefWriteIterator ddwiter;
	eld.setDataDefID(0);
	ElementListDefWriteIterator elditer;
	ElementEntryDef def;
	
	ddwiter.start(series);

	RFA_String fids, values, displays;
	fids.set("FIDS", 4, false);
	values.set("VALUES", 6, false);
	displays.set("DISPLAYS", 8, false);

	elditer.start(eld);
	def.setName(fids);
	def.setType(ArrayEnum);
	elditer.bind(def);
	def.setName(values);
	def.setType(ArrayEnum);
	elditer.bind(def);
	def.setName(displays);
	def.setType(ArrayEnum);
	elditer.bind(def);
	elditer.complete();
	ddwiter.bind(eld);

	ddwiter.complete();
}

void RDMDictionaryEncoder::encodeDictionarySummaryData(const RDMDict & dictionary, UInt32 type,
													   Series & series, ElementList & summary)
{
	ElementEntry element;
	DataBuffer elementData;
	ElementListWriteIterator eiter;
	if (_optimize == Speed)
	{
		series.setSummaryData(summary);
	}

	eiter.start(summary);
	element.setName(rfa::rdm::ENAME_TYPE);
	elementData.setUInt(type);
	element.setData(elementData);
	eiter.bind(element);
	element.setName(rfa::rdm::ENAME_DICTIONARY_ID);
	elementData.setUInt(dictionary.getDictId());
	element.setData(elementData);
	eiter.bind(element);
	element.setName(rfa::rdm::ENAME_VERSION);
	string ver = dictionary.getVersion().c_str();
	const unsigned char * version = (const unsigned char*) ver.c_str();
	Buffer b;
	b.setFrom(version, static_cast<int>( ver.length() ));
	elementData.setBuffer(b, DataBuffer::BufferEnum);
	element.setData(elementData);
	eiter.bind(element);
	eiter.complete();

	if (_optimize != Speed)
	{
		series.setSummaryData(summary);
	}

}

void RDMDictionaryEncoder::encodeEnumDictionaryEnumTable( const EnumTables::const_iterator & iter,
														 const ElementListDef& eld,
														 ElementList& etable )
{
	ElementListWriteIterator eiter;

	eiter.start( etable, DefinedDataFlag, &eld );

	Array array;
	ArrayEntry aentry;
	DataBuffer d;
	ArrayWriteIterator awiter;

	if ( _optimize == Speed )
		eiter.bind( array );

	array.setWidth( 2 );
	
	awiter.start( array );
	
	if ( RDMDict::Trace & 0x8 )
		cout << "FIDS: ";
	
	const FIDList& fields = (*iter)->fids();
	for ( FIDList::const_iterator fiter = fields.begin();
		fiter != fields.end(); fiter++ )
	{
		int fid = *fiter;
		if ( RDMDict::Trace & 0x8 )
			cout << fid << ", ";

		d.setInt( fid, Bit16Value );
		aentry.setData( d );
		awiter.bind( aentry );
	}

	awiter.complete();
	
	if ( _optimize != Speed )
		eiter.bind( array );

	if ( RDMDict::Trace & 0x8 )
		cout << endl;
	
	const EnumValues& values = (*iter)->values();

	d.clear();
	array.clear();

	if ( _optimize == Speed )
		eiter.bind( array );

	array.setWidth(2);

	awiter.start( array, (UInt16)values.size() );
	
	if ( RDMDict::Trace & 0x8 )
		cout << "VALUES: ";

	for ( EnumValues::const_iterator viter = values.begin();
		viter != values.end();
		viter++)
	{
		if (RDMDict::Trace & 0x8)
			cout << viter->first << ", ";
		d.setEnumeration( viter->first );
		aentry.setData( d );
		awiter.bind( aentry );
	}
	if (RDMDict::Trace & 0x8)
		cout << endl;
	awiter.complete();
	if (_optimize != Speed)
		eiter.bind(array);
	
	d.clear();
	array.clear();

	if (_optimize == Speed)
		eiter.bind(array);
	EnumValues::const_iterator diter = values.begin();
	array.setWidth((UInt16)diter->second.length());
	awiter.start(array, (UInt16) values.size());
	if (RDMDict::Trace & 0x8)
		cout << "DISPLAYS: ";
	for (; diter != values.end(); diter++)
	{
		RFA_String s;
		const std::string & enumDisp = diter->second.c_str();
		if (RDMDict::Trace & 0x8)
			cout << enumDisp.c_str() << ", ";
		s.set(enumDisp.c_str());
		d.setFromString(s, DataBuffer::StringAsciiEnum);
		aentry.setData(d);
		awiter.bind(aentry);
	}
	if (RDMDict::Trace & 0x8)
		cout << endl;
	awiter.complete();
	if (_optimize != Speed)
		eiter.bind(array);

	eiter.complete();
}

void RDMDictionaryEncoder::encodeEnumDictionaryTables(EnumTables::const_iterator & iter, 
			SeriesWriteIterator & switer, SeriesEntry & sentry, const RDMEnumDict & dictionary,
			ElementList & elementList,
			const ElementListDef & eld,	Series & series)
{
	_maxSeriesEntrySize = 0;

	const EnumTables & enumList = dictionary.enumList();
	for ( ;iter != enumList.end(); iter++, _tableIndex++ )
	{
		if ( RDMDict::Trace & 0x4 )
			cout << "encoding table " << _tableIndex << ": ";
		if ( _optimize == Speed )
		{
			sentry.setData(elementList);
			switer.bind(sentry);
		}

		encodeEnumDictionaryEnumTable(iter, eld, elementList);

		if (RDMDict::Trace & 0x4)
		{
			int enumTableLength = elementList.getEncodedBuffer().size();
			cout << enumTableLength << " bytes" << endl;
		}


		if (_optimize == Fragment)
		{
			int currentLength = series.getEncodedBuffer().size();
			int enumTableLength = elementList.getEncodedBuffer().size();
			if (currentLength + enumTableLength + 20 > _maxFragmentSize)
			{
				switer.complete();
				if (RDMDict::Trace & 0x4)
				{
					cout << "saving table " << _tableIndex << " for next fragment" << endl;
				}

				if (RDMDict::Trace & 0x1)
				{
					cout << "encodeEnumDictionary fragment of size " 
						<< currentLength << " complete" << endl;
				}
				_isComplete = false;
				return;
			}
		}

		if (_optimize != Speed)
		{
			sentry.setData(elementList);
			switer.bind(sentry);
		}
		elementList.clear();
		sentry.clear();
	}
	switer.complete();
	if (RDMDict::Trace & 0x1)
	{
		cout << "encodeEnumDictionary fragment of size " 
			<< series.getEncodedBuffer().size() << " complete" << endl;
	}
	_isComplete = true;
}



void RDMDictionaryEncoder::encodeEnumDictionary(const RDMEnumDict & dictionary, UInt32 verbosity, Series & series)
{
	if (RDMDict::Trace & 0x1)
		cout << "encodeEnumDictionary" << endl;

	const EnumTables & enumList = dictionary.enumList();

	series.setTotalCountHint((UInt32)enumList.size());
	UInt8 indicationMask = Series::SummaryDataFlag;
	if (verbosity > DICTIONARY_INFO)
       indicationMask |= Series::DataDefFlag | Series::EntriesFlag;
	series.setIndicationMask(indicationMask);

	ElementList summaryel;
	summaryel.setAssociatedMetaInfo(series.getMajorVersion(), series.getMinorVersion());
	if (_optimize != Speed)
		encodeDictionarySummaryData(dictionary, DICTIONARY_ENUM_TABLES,  series, summaryel);

	if (verbosity > DICTIONARY_INFO)
		encodeEnumDictionaryDataDef(*_elementListDef, series);
	if (_optimize == Speed)
		encodeDictionarySummaryData(dictionary, DICTIONARY_ENUM_TABLES, series, summaryel);

	if (verbosity > DICTIONARY_INFO)
	{
		SeriesWriteIterator switer;
		SeriesEntry sentry;
		switer.start(series, (UInt16)enumList.size());
		_enumIter = enumList.begin();
		encodeEnumDictionaryTables(_enumIter, switer, sentry, *_enumDict, *_elementList, *_elementListDef, series);
	}
	if (RDMDict::Trace & 0x1)
		cout << "encodeEnumDictionary complete" << endl;
}
