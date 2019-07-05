#include "RDMDictionaryDecoder.h"

#include "Data/ElementListDef.h"
#include "Data/ElementEntryDef.h"
#include "Data/ElementEntry.h"

#include "Data/Series.h"
#include "Data/SeriesReadIterator.h"
#include "Data/ArrayReadIterator.h"
#include "Data/ElementListReadIterator.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;
using namespace rfa::common;
using namespace rfa::rdm;
using namespace rfa::data;

////////////////////////////////////////////////////////////////////
/// Begin RDMFileDictionaryDecoder
////////////////////////////////////////////////////////////////////


// static variables are initialized here.
// Currently, only !tag Version for dictionary is supported, so the version is read from the file.
// The other three are assigned 1.1 or 1 until the final version of RMDS template is released.

rfa::common::RFA_String RDMFileDictionaryDecoder::_DictVersion("", 0, false);
rfa::common::RFA_String RDMFileDictionaryDecoder::_EnumVersion("1.1", 0, false);
rfa::common::Int32 RDMFileDictionaryDecoder::_DictId     = 1;
rfa::common::Int32 RDMFileDictionaryDecoder::_EnumId = 1;

RDMFileDictionaryDecoder::RDMFileDictionaryDecoder(RDMFieldDict & dictionary)
: _dictionary(dictionary)
{
	_enumDict = &dictionary.enumDict();
}

RDMFileDictionaryDecoder::~RDMFileDictionaryDecoder()
{}

bool RDMFileDictionaryDecoder::loadEnumTypeDef( const rfa::common::RFA_String& enumTypeDef, RDMEnumDict *pEnumDict )
{
	if (RDMDict::Trace & 0x1)
		cout << "File loadEnumTypeDef" << endl;

	fstream	file;
	char	read_buffer[1024];

	// open file for reading
	file.open( enumTypeDef.c_str(), ios::in);

	if(!file.is_open()){
	    cout<<endl<<"Error opening EnumDictionaryFilename: "<<enumTypeDef.c_str()<<endl;
		return false;
	}

	bool		newEnumExpected  = true;
	RDMEnumDef *pEnumDef = 0;
	
	string s, fidName, expandedValue;

	// Read line by line
	while( file.getline( read_buffer, sizeof read_buffer - 1 ) )
	{
		if( read_buffer[0] == '!' ) // Skip comments
			continue;
		
		if( read_buffer[0] ==  0) // Skip blank lines
			continue;

		s.assign( &read_buffer[0], strlen( &read_buffer[0] ) );
		istringstream ss( read_buffer );

		if ( read_buffer[0] != ' ' )
		{
			if ( newEnumExpected )
			{
				if( pEnumDef ){
					pEnumDict->insertEnumDef( pEnumDef );
					if (RDMDict::Trace & 0x10)
						pEnumDef->dumpEnumDef();
				}
				
				pEnumDef = new RDMEnumDef;

				newEnumExpected = false;
			}

			// Skip the fid name
			string::size_type index1 = s.find_first_of( " ", 1 );
			index1 = s.find_first_not_of( " ", index1 + 1 );
			// this check protects from badly formatted line in enumtype.def file
			if ( index1 == string::npos ) { continue; }

			// get the fid id
			ss.seekg( index1 );
			rfa::common::Int16	fid;
			ss >> fid;
			pEnumDef->addFid( fid );
		}
		else
		{
			newEnumExpected = true;

			string::size_type index1 = s.find_first_not_of( " ", 0 );
			if ( index1 == string::npos ) { continue; }

			// Get enum val
			ss.seekg( index1 );
			rfa::common::UInt16 enumVal;
			ss >> enumVal;

			// Get enum string
			index1 = s.find_first_of( "\"#", static_cast<unsigned int>(ss.tellg()) );
			if ( index1 == string::npos ) { continue; }

			if (s[index1]=='#')
			{
				string::size_type index2 = s.find_first_of( "\"#", index1 + 1 );
				if ( index2 == string::npos ) { continue; }
				expandedValue.clear();
				unsigned int start = (unsigned int) (index1 + 1);
				unsigned int length = (unsigned int) (index2 - index1 - 1);
				for (unsigned int i = 0; i < length; i += 2)
				{
					string tmp = s.substr(start + i, 2);
					long hexchar = strtol(tmp.c_str(), (char**) 0, 16);
					expandedValue += (char) hexchar;
				}
			}
			else
			{
				string::size_type index2 = s.find_first_of( "\"", index1 + 1 );
				if ( index2 == string::npos ) { continue; }
				expandedValue = s.substr( index1 + 1, index2 - index1 - 1 );
			}
			pEnumDef->insertEnumVal( enumVal, RFA_String(expandedValue.c_str(), 0, false) );
		}
	}

	if( pEnumDef )
	{
		pEnumDict->insertEnumDef( pEnumDef );
		if (RDMDict::Trace & 0x10)
			pEnumDef->dumpEnumDef();
	}
	file.close();
	return true;
}


void RDMFileDictionaryDecoder::associate()
{
	_dictionary.fixRipple();
	_dictionary.associateEnumDict();
}

// Read appendinx_a file
bool RDMFileDictionaryDecoder::loadAppendix_A( const rfa::common::RFA_String& appendix_a )
{
	if (RDMDict::Trace & 0x1)
		cout << "File loadAppendix_A" << endl;

	fstream	file;
	const int BufSize = 1024;
	char read_buffer[BufSize];

    file.open( appendix_a.c_str(), ios::in);
	if(!file.is_open())
	{
        cout<<endl<<"Error opening FieldDictionaryFilename: "<<appendix_a.c_str()<<endl;
        return false;
    }

	string s;
	string::size_type index1, index2, dataTypeIndex1, dataTypeIndex2;
	RDMFieldDef *pFieldDef = 0;

	// Read line by line
	while ( file.getline( &read_buffer[0], sizeof( read_buffer ) - 1, '\n' ))
	{
		if ( read_buffer[0] == '!' ) {
			if ( read_buffer[1] == ' ' )
				continue;

			// If this line contains version or id of dictionary, assign these
			// value to the corresponding static data members.
			// Ref: RDMFieldDictionary
			char tmp_str[80];

			strcpy( tmp_str, "!tag Version" );
			int len = static_cast<int>( strlen( tmp_str ));
			if ( _DictVersion.length() == 0 && strncmp(read_buffer, tmp_str, len) == 0 ) 
			{
				int i = len;
				while ( !isdigit(read_buffer[i]) && i < BufSize )
					++i;

				if (i >= BufSize) // Version number not found
					continue;

				int j;
				for (j = i; !isspace(read_buffer[i]); ++i)
					tmp_str[i - j] = read_buffer[i]; 
				tmp_str[i - j] = '\0';

				_DictVersion = tmp_str;
			}

			continue;
		}

		if ( read_buffer[0] ==  0) // Skip blank lines
			continue;

		// Start with a fresh Field Def
		pFieldDef = new RDMFieldDef();

		s.assign( &read_buffer[0], strlen( &read_buffer[0] ) );
		istringstream ss( read_buffer );

		// Parse name 1st
		index1 = s.find_first_not_of(" ");
		// the below "if ( index == string::npos )" checks protect from "badly" formatted lines in RDMFieldDictionary file
		// in such case string::find*() methods at some point will return unexpected string::npos
		// at this time, we need to skip a badly formatted line, and to avoid memory leaks pFieldDef needs to be freed
		if ( index1 == string::npos ) { delete pFieldDef; continue; }

		index2 = s.find_first_of( " ", index1 + 1 );
		if ( index2 == string::npos ) { delete pFieldDef; continue; }

		pFieldDef->setName( rfa::common::RFA_String(s.substr( index1, index2 - index1 ).c_str(), 0, false) );

		// Parse display name
		index1 = s.find_first_of( "\"", index2 + 1 );
		if ( index1 == string::npos ) { delete pFieldDef; continue; }
		index2 = s.find_first_of( "\"", index1 + 1 );
		if ( index2 == string::npos ) { delete pFieldDef; continue; }
		pFieldDef->setDisplayName( rfa::common::RFA_String(s.substr( index1 + 1, index2 - index1 - 1 ).c_str(), 0, false) );

		// Get the Field Id, used as key
		index1 = s.find_first_not_of( " ", index2 + 1 );
		if ( index1 == string::npos ) { delete pFieldDef; continue; }
		rfa::common::Int16 fieldId;
		ss.seekg( index1 ); //, ios::beg);
		ss >> fieldId;
		pFieldDef->setFieldId( fieldId );

		// Get Ripple To Field
		index1 = s.find_first_not_of(" ", static_cast<unsigned int>(ss.tellg()) );
		if ( index1 == string::npos ) { delete pFieldDef; continue; }
		index2 = s.find_first_of( " ", index1 + 1 );
		if ( index2 == string::npos ) { delete pFieldDef; continue; }
		pFieldDef->setRipplesToFieldName( rfa::common::RFA_String(s.substr( index1, index2 - index1 ).c_str(), 0, false) );

		// Get MF Field Type
		index1 = s.find_first_not_of(" ", index2 + 1 );
		if ( index1 == string::npos ) { delete pFieldDef; continue; }
		index2 = s.find_first_of( " ", index1 + 1 );
		if ( index2 == string::npos ) { delete pFieldDef; continue; }
		pFieldDef->setMFFieldTypeName( rfa::common::RFA_String(s.substr( index1, index2 - index1 ).c_str(), 0, false) );

		// Get MF Field Length
        index1 = s.find_first_not_of( " ", index2 + 1 );
		if ( index1 == string::npos ) { delete pFieldDef; continue; }
        rfa::common::UInt16 mfFLen;
        ss.seekg((unsigned int)index1, ios::beg);
        ss >> mfFLen;
		pFieldDef->setMFFieldLength( mfFLen );
		index2 = index1 = static_cast<unsigned int>(ss.tellg()); 

		// Get MF enum length
		index1 = s.find_first_of("(", index1 );
			// "(" not found, MF enum length does not exist for this entry
		if( index1 == string::npos )
			index1 = index2;
		else{
	        index1 = s.find_first_not_of( " ", index1 + 1 );
            rfa::common::UInt16 mfELen;
            ss.seekg((unsigned int)index1, ios::beg);
            ss >> mfELen;
			pFieldDef->setMFEnumLength( mfELen );
			index1 = s.find_first_of( ")", static_cast<unsigned int>(ss.tellg()) );
        }

		// Get BufferType
		index1 = s.find_first_not_of( " ", index1 + 1 );
		if ( index1 == string::npos ) { delete pFieldDef; continue; }
		index2 = s.find_first_of( " ", index1 + 1 );
		if ( index2 == string::npos ) { delete pFieldDef; continue; }
		dataTypeIndex1 = index1;
		dataTypeIndex2 = index2;

		// Get RWF length
        index1 = s.find_first_not_of( " ", index2 + 1 );
		if ( index1 == string::npos ) { delete pFieldDef; continue; }
		rfa::common::UInt16 rwfLen;
        ss.seekg((unsigned int)index1, ios::beg);
        ss >> rwfLen;
        pFieldDef->setMaxFieldLength( rwfLen );

		// setting data type requires knowledge of data type name and length
		// e.g. REAL 5 => REAL32
		pFieldDef->setDataType( rfa::common::RFA_String(s.substr( dataTypeIndex1, dataTypeIndex2 - dataTypeIndex1 ).c_str(), 0, false), rwfLen );

		// Skip the rest since it's not used
		_dictionary.putFieldDef( pFieldDef );
		if (RDMDict::Trace & 0x10)
			pFieldDef->dumpFieldDef( );
	}

	file.close();
	return true;
}

bool RDMFileDictionaryDecoder::load( const rfa::common::RFA_String& appendix_a, 
									 const rfa::common::RFA_String& enumTypeDef )
{
	//check to see if strings are empty
	if(appendix_a.empty())
	{
		cout<<"\nField dictionary name is empty!"<<endl;
		return false;
	}
	if(enumTypeDef.empty())
	{
		cout<<"\nenumTypeDef name is empty!"<<endl;
		return false;
	}


	// load appendix_a and enumTypeDef
	if(!loadAppendix_A( appendix_a ) || !loadEnumTypeDef( enumTypeDef, _enumDict ))
		return false;

	associate();
	return true;
}
////////////////////////////////////////////////////////////////////
/// End RDMFileDictionaryDecoder
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
/// Begin RDMNetworkDictionaryDecoder
////////////////////////////////////////////////////////////////////
RDMNetworkDictionaryDecoder::RDMNetworkDictionaryDecoder(RDMFieldDict & dictionary, rfa::common::UInt32 dictVerbosityType)
:	appendixALoaded(false),
	enumTypeDefLoaded(false),
	_dictionary(dictionary),
	_dictVerbosityType(dictVerbosityType),
	_def(0),
	_enumDict(0),
	_first(true),
	pEnumDef(0)
{
	_enumDict = &dictionary.enumDict();
}

RDMNetworkDictionaryDecoder::~RDMNetworkDictionaryDecoder()
{
}

rfa::common::UInt32	RDMNetworkDictionaryDecoder::getDictVerbosityType()
{ return _dictVerbosityType;}


void RDMNetworkDictionaryDecoder::loadAppendix_A(const rfa::common::Data &data, bool moreFragments)
{
	if (RDMDict::Trace & 0x1)
		cout << "Network loadAppendix_A " << (moreFragments ? "" : "last") << endl;

	// start the decoding process with recursion level of 0.
	decodeFieldDefDictionary(data);
	appendixALoaded = !moreFragments;
	if (appendixALoaded)
		_dictionary.fixRipple();
    if (appendixALoaded && enumTypeDefLoaded)
		associate();
}

void RDMNetworkDictionaryDecoder::loadEnumTypeDef(const rfa::common::Data &data, bool moreFragments)
{
	if (RDMDict::Trace & 0x1)
		cout << "Network loadEnumTypeDef " << (moreFragments ? "" : "last") << endl;

	// start the decoding process with recursion level of 0.
	decodeEnumDefTables(data);
	enumTypeDefLoaded = !moreFragments;
	if (appendixALoaded && enumTypeDefLoaded)
		associate();
}

void RDMNetworkDictionaryDecoder::associate()
{
	_dictionary.associateEnumDict();
	
	// _enumDict is now owned by the RDMFieldDict superclass, do not hold on to a reference
	_enumDict = 0; 
}

void RDMNetworkDictionaryDecoder::decodeFieldDefDictionary( const rfa::common::Data& data )
{
	ElementListReadIterator elReadIter;

	const Series& series = static_cast<const Series&>(data);

	if (series.getIndicationMask() & Series::SummaryDataFlag)
	{
		// summary data should be a elementlist
		decodeFieldDefDictionarySummaryData(static_cast<const ElementList&>(series.getSummaryData()), elReadIter);
	}

	if(_dictVerbosityType != rfa::rdm::DICTIONARY_INFO) // INFO only has summary data
	{
		SeriesReadIterator it;
		it.start(series);
		while(!it.off())
		{
			//decode Dictionary Row  - each row should be an elementList
			decodeFieldDefDictionaryRow(static_cast<const ElementList&> (it.value().getData()), elReadIter);
			it.forth();
		}
	}
}



void RDMNetworkDictionaryDecoder::decodeFieldDefDictionarySummaryData(const ElementList& elementList, ElementListReadIterator& it)
{
	it.start(elementList);

	while(!it.off())
	{
		const ElementEntry& element = it.value();

		// The "Type" element is assumed to be DICTIONARY_FIELD_DEFINITIONS (1) - See RDM.h
		if (element.getName() == rfa::rdm::ENAME_DICTIONARY_ID) 
			_dictionary.setDictId(static_cast<const DataBuffer&>(element.getData()).getInt());
		else if (element.getName() == rfa::rdm::ENAME_VERSION)
			_dictionary.setVersion(static_cast<const DataBuffer&>(element.getData()).getAsString());

		it.forth();
	}
	if (RDMDict::Trace & 0x2)
	{
		cout << "type: " << _dictionary.getDictType() << " dictId: " << _dictionary.getDictId() 
			<< " ver: " << _dictionary.getVersion().c_str() << endl;
	}
}

void RDMNetworkDictionaryDecoder::decodeFieldDefDictionaryRow(const ElementList& elementList, ElementListReadIterator& it)
{
	RDMFieldDef* pFieldDef = new RDMFieldDef();
	
	bool success;
	// Decode the first row or others
	if (_first)
	{
		_first = false;
		success = decodeFirstFieldDef(elementList, it, pFieldDef);
	}
	else
	{
		success = decodeOtherFieldDef(elementList, it, pFieldDef);
	}
	if (!success)
	{
		cout << "Dictionary not encoded correctly" << endl;
		delete pFieldDef;
		return;
	}
	// if off we should have a complete fieldDef to put into the dictionary
	_dictionary.putFieldDef(pFieldDef);
	if (RDMDict::Trace & 0x20)
		pFieldDef->dumpFieldDef();
}

bool RDMNetworkDictionaryDecoder::decodeFirstFieldDef(const ElementList& elementList, ElementListReadIterator& it, RDMFieldDef * pFieldDef)
{
	it.start(elementList);

	// will assume the minimal set of of elements defined by DICTIONARY_MINUMAL ('NAME', 'TYPE', 'FID', 'LENGTH', 'RWFTYPE',
	// 'RWFLEN', 'RIPPLETO') but also check for the other elements depending on the dictionary mask

	const ElementEntry& element = it.value();
	if (element.getName() == "NAME") 
		pFieldDef->setName(static_cast<const DataBuffer&>(element.getData()).getAsString());
	else
		return false;

	it.forth();
	const ElementEntry& element3 = it.value();
	if (element3.getName() == "FID")
		pFieldDef->setFieldId((rfa::common::Int16)(static_cast<const DataBuffer&>(element3.getData())).getInt());
	else
		return false;

	it.forth();
	const ElementEntry& element4 = it.value();
	if (element4.getName() == "RIPPLETO")
		pFieldDef->setRipplesToFid(static_cast<const DataBuffer&>(element4.getData()).getInt());
	else
		return false;

	it.forth();
	const ElementEntry& element5 = it.value();
	if (element5.getName() == "TYPE")
		pFieldDef->setMFFieldType(static_cast<const DataBuffer&>(element5.getData()).getInt());
	else
		return false;

	it.forth();
	const ElementEntry& element6 = it.value();
	if (element6.getName() == "LENGTH")
		pFieldDef->setMFFieldLength(static_cast<const DataBuffer&>(element6.getData()).getUInt());
	else
		return false;
	
	it.forth();
	const ElementEntry& element8 = it.value();
	if (element8.getName() == "RWFTYPE")
		pFieldDef->setDataType( static_cast<const DataBuffer&>(element8.getData()).getUInt() );
	else
		return false;
	
	it.forth();
	const ElementEntry& element9 = it.value();
	if (element9.getName() == "RWFLEN")
		pFieldDef->setMaxFieldLength( static_cast<rfa::common::UInt16>(static_cast<const DataBuffer&>(element9.getData()).getUInt()) );
	else
		return false;

	if(_dictVerbosityType == rfa::rdm::DICTIONARY_NORMAL || _dictVerbosityType == rfa::rdm::DICTIONARY_VERBOSE) // DICTIONARY_NORMAL contains these elements
	{
		it.forth();
		const ElementEntry& element7 = it.value();
		if (element7.getName() == "ENUMLENGTH")
			pFieldDef->setMFEnumLength(static_cast<const DataBuffer&>(element7.getData()).getUInt());
		else
			return false;

		it.forth();
		const ElementEntry& element2 = it.value();
		if (element2.getName() ==	"LONGNAME")
			pFieldDef->setDisplayName( static_cast<const DataBuffer&>(element2.getData()).getAsString() );
		else
			return false;
	}

	it.forth();
	return it.off();
}

bool RDMNetworkDictionaryDecoder::decodeOtherFieldDef(const ElementList& elementList, ElementListReadIterator& it, RDMFieldDef * pFieldDef)
{
	// Decode other field and keep in RDMFieldDef 
	it.start(elementList);

	pFieldDef->setName(static_cast<const DataBuffer&>(it.value().getData()).getAsString());

	it.forth();
	pFieldDef->setFieldId((rfa::common::Int16)(static_cast<const DataBuffer&>(it.value().getData())).getInt());

	it.forth();
	pFieldDef->setRipplesToFid(static_cast<const DataBuffer&>(it.value().getData()).getInt());

	it.forth();
	pFieldDef->setMFFieldType(static_cast<const DataBuffer&>(it.value().getData()).getInt());

	it.forth();
	pFieldDef->setMFFieldLength(static_cast<const DataBuffer&>(it.value().getData()).getUInt());
	
	it.forth(); 
	pFieldDef->setDataType( static_cast<rfa::common::UInt16>(static_cast<const DataBuffer&>(it.value().getData()).getUInt()) );
	
	it.forth();
	pFieldDef->setMaxFieldLength( static_cast<rfa::common::UInt16>(static_cast<const DataBuffer&>(it.value().getData()).getUInt()) );

	if(_dictVerbosityType == rfa::rdm::DICTIONARY_NORMAL || _dictVerbosityType == rfa::rdm::DICTIONARY_VERBOSE) // DICTIONARY_NORMAL contains these elements
	{
		it.forth();
		pFieldDef->setMFEnumLength(static_cast<const DataBuffer&>(it.value().getData()).getUInt());

		it.forth();
		pFieldDef->setDisplayName( static_cast<const DataBuffer&>(it.value().getData()).getAsString() );
	}

	it.forth();

	return it.off();
}




void RDMNetworkDictionaryDecoder::decodeEnumDefTables( const Data& data )
{
	ElementListReadIterator elReadIter1;
	ElementListReadIterator elReadIter2;

	const Series& series = static_cast<const Series&>(data);

	if (series.getIndicationMask() & Series::SummaryDataFlag)
	{
		decodeEnumDefTablesSummaryData(static_cast<const ElementList&>(series.getSummaryData()), elReadIter1);
	}

	SeriesReadIterator it;
	it.start(series);
	while(!it.off())
	{
		//cout << "decoding table " << i++ << endl;
		//decode Enum table  - each row should be an elementList
		decodeEnumDefTable(static_cast<const ElementList&> (it.value().getData()), elReadIter1, elReadIter2);
		it.forth();
	}
}

void RDMNetworkDictionaryDecoder::decodeEnumDefTablesSummaryData(const ElementList& elementList, ElementListReadIterator& it)
{
	it.start(elementList);

	while(!it.off())
	{
		const ElementEntry& element = it.value();

		// The "Type" element is assumed to be DICTIONARY_ENUM_TABLES (2) - See RDM.h
		if (element.getName() == rfa::rdm::ENAME_DICTIONARY_ID) 
			_enumDict->setDictId(static_cast<const DataBuffer&>(element.getData()).getInt());
		else if (element.getName() == rfa::rdm::ENAME_VERSION)
			_enumDict->setVersion(static_cast<const DataBuffer&>(element.getData()).getAsString());

		it.forth();
	}
	if (RDMDict::Trace & 0x2)
	{
		cout << "type: " << _enumDict->getDictType() << " dictId: " << _enumDict->getDictId() 
			<< " ver: " << _enumDict->getVersion().c_str() << endl;
	}
}

void RDMNetworkDictionaryDecoder::decodeEnumDefTable(const ElementList& elementList, ElementListReadIterator& elIt1, ElementListReadIterator& elIt2)
{
	// create two iterators so we can do depth traversal on the 2nd 
	// and 3rd elementEntries at the same time.

	RDMEnumDef* enumDef = new RDMEnumDef();
	elIt1.start(elementList);
	elIt2.start(elementList);

	// The first element in the elementList is an Array that contains all of the fids.
	// Decode and add all of the fids to the RDMEnumDef
	const ElementEntry& fidsElement = elIt1.value();
	const Array& fidArray = static_cast<const Array&> (fidsElement.getData());
	ArrayReadIterator fidArrIt;
	fidArrIt.start(fidArray);
	while(!fidArrIt.off())
	{
		const DataBuffer& fidBuffer = static_cast<const DataBuffer&> (fidArrIt.value().getData());
		enumDef->addFid(static_cast<rfa::common::Int16> (fidBuffer.getInt()));
		fidArrIt.forth();
    }

	// next get the second element("VALUES") and the third element("DISPLAYS")
	//	and decode the contained Arrays at the same time, inserting them into the RDMEnumDef
	//  Value-Display are always 1-to-1 (ie. the contained Arrays always have the same count
	elIt1.forth();
	const ElementEntry& valuesElement = elIt1.value();
	const Array& valueArray = static_cast<const Array&> (valuesElement.getData());

	ArrayReadIterator valArrIt;
	valArrIt.start(valueArray);

	elIt2.forth();
	elIt2.forth();
	const ElementEntry& displaysElement = elIt2.value();
	const Array& displayArray = static_cast<const Array&> (displaysElement.getData());
	ArrayReadIterator disArrIt;
	disArrIt.start(displayArray);

	while(!valArrIt.off() && !disArrIt.off())
	{
		const DataBuffer& valueBuffer = static_cast<const DataBuffer&> (valArrIt.value().getData());
		const DataBuffer& displayBuffer = static_cast<const DataBuffer&> (disArrIt.value().getData());

		rfa::common::RFA_String display(displayBuffer.getAsString());

		rfa::common::UInt16 enumeration = valueBuffer.getEnumeration();
		enumDef->insertEnumVal(enumeration, display );

		valArrIt.forth();
		disArrIt.forth();
	}
	_enumDict->insertEnumDef(enumDef);
	if (RDMDict::Trace & 0x10)
		enumDef->dumpEnumDef();
}
////////////////////////////////////////////////////////////////////
/// End RDMNetworkDictionaryDecoder
////////////////////////////////////////////////////////////////////

