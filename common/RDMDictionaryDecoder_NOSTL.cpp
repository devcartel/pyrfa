
#include "RDMDictionaryDecoder_NOSTL.h"

#include "Data/ElementListDef.h"
#include "Data/ElementEntryDef.h"
#include "Data/ElementEntry.h"

#include "Data/Series.h"
#include "Data/SeriesReadIterator.h"
#include "Data/ArrayReadIterator.h"
#include "Data/ElementListReadIterator.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_FLINE_LEN 255

using namespace rfa::common;
using namespace rfa::rdm;

////////////////////////////////////////////////////////////////////
/// Begin RDMFileDictionaryDecoder
////////////////////////////////////////////////////////////////////

RDMFileDictionaryDecoder::RDMFileDictionaryDecoder(RDMFieldDict & dictionary)
: _dictionary(dictionary)
{
	_enumDict = &dictionary.enumDict();
}

RDMFileDictionaryDecoder::~RDMFileDictionaryDecoder()
{}

bool RDMFileDictionaryDecoder::loadEnumTypeDef( const RFA_String& enumTypeDef, RDMEnumDict *pEnumDict )
{
	if (RDMDict::Trace & 0x1)
		printf("File loadEnumTypeDef\n"); 

	
	FILE *fp;
	if ((fp = fopen(enumTypeDef.c_str(), "r")) == NULL)
	{
		
		printf("Can't open EnumTypeDef file: %s\n", enumTypeDef.c_str());
		return false;
	}
	
	bool	newEnumExpected  = true;
	char	fline[MAX_FLINE_LEN + 1];
	char	strval[MAX_FLINE_LEN+1];
	int		curPos;
	int		lineNum = 0;

	RDMEnumDef *pEnumDef = 0;
	
	RFA_String s, fidName, expandedValue;

	while (fgets(fline, MAX_FLINE_LEN, fp) != NULL)
	{
		lineNum++;

		if( fline[0] == '!' ) // Skip comments
			continue;
		
		if( fline[0] ==  0) // Skip blank lines
			continue;

		curPos = 1;

		if ( fline[0] != ' ' )
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

			curPos = getCopyUntilSpace( fline, curPos, (char*)&strval); // Skip Acronym
			
			if ((curPos = getCopyUntilSpace( fline, curPos, (char*)&strval)) < 0)
			{
				printf("Cannot find FID (Line=%d).\n",lineNum);
				continue;
			}
			pEnumDef->addFid(atoi(strval));
		}
		else
		{
			newEnumExpected = true;
			if ((curPos = getCopyUntilSpace( fline, curPos, (char*)&strval)) < 0)
			{
				printf("Cannot find enum Value (Line=%d).\n",lineNum);
				continue;
			}
	
			UInt16 enumVal = atoi(strval);

			bool charQuote;
			if ((curPos = getCopyPoundOrQuoteStr( charQuote, fline, curPos, (char*)&strval)) < 0)
			{
				printf("Cannot find display (Line=%d).\n",lineNum);
				continue;
			}

			expandedValue.clear();

			if(! charQuote)
			{
				unsigned int length = (unsigned int)strlen(strval);
				for (unsigned int i = 0; i < length; i += 2)
				{
					char tmp[10];
					memset(tmp,'\0',10);
		
					memcpy(tmp,strval+i,2);
	
					int hexchar = (int)strtol( tmp, (char**) 0, 16);
					memset(tmp,'\0',10);
					sprintf(tmp,"%d",hexchar);
					expandedValue += tmp;
				}
			}
			else
			{
				expandedValue = strval;
			}
			pEnumDef->insertEnumVal( enumVal, expandedValue );
		}
	}

	if( pEnumDef )
	{
		pEnumDict->insertEnumDef( pEnumDef );
		if (RDMDict::Trace & 0x10)
			pEnumDef->dumpEnumDef();
	}
	
	fclose(fp);
	return true;
}


void RDMFileDictionaryDecoder::associate()
{
	_dictionary.fixRipple();
	_dictionary.associateEnumDict();
}

// Read appendinx_a file
bool RDMFileDictionaryDecoder::loadAppendix_A( const RFA_String& appendix_a )
{
	if (RDMDict::Trace & 0x1)
		printf("File loadAppendix_A\n");

	if( appendix_a.empty() )
	{
		printf(" FileName for FieldDictionaryFilename is empty\n");
		return false;
	}

	FILE *fp;
	if ((fp = fopen(appendix_a.c_str(), "r")) == NULL)
	{
		
		printf("Error opening FieldDictionaryFilename: %s\n", appendix_a.c_str());
		return false;
	}

	char	fline[MAX_FLINE_LEN + 1];
	char	strval[MAX_FLINE_LEN+1];
	int		curPos;
	int		lineNum = 0;

	RDMFieldDef *pFieldDef = 0;


	while (fgets(fline, MAX_FLINE_LEN, fp) != NULL)
	{
		lineNum++;
		
		if( fline[0] == '!' ) // Skip comments
			continue;

		if( fline[0] ==  0) // Skip blank lines
			continue;

		// Start with a fresh Field Def
		pFieldDef = new RDMFieldDef();

		curPos = 0;

		if ((curPos = getCopyUntilSpace( fline, curPos, (char*)&strval)) < 0)
		{
			printf("Cannot find Acronym (Line=%d).\n",lineNum);
			delete pFieldDef;
			return false;
		}
		pFieldDef->setName(RFA_String(strval, 0, false));

		if ((curPos = getCopyQuotedStr( fline, curPos, (char*)&strval)) < 0)
		{
			printf("Cannot find DDE Acronym (Line=%d).\n",lineNum);
			delete pFieldDef;
			return false;
		}
		pFieldDef->setDisplayName(RFA_String(strval, 0, false));

		if ((curPos = getCopyUntilSpace( fline, curPos, (char*)&strval)) < 0)
		{
			printf("Cannot find Fid Number (Line=%d).\n",lineNum);
			delete pFieldDef;
			return false;
		}
		pFieldDef->setFieldId(atoi(strval));

		if ((curPos = getCopyUntilSpace( fline, curPos, (char*)&strval)) < 0)
		{
			printf("Cannot find Ripples To (Line=%d).\n",lineNum);
			delete pFieldDef;
			return false;
		}
		
		pFieldDef->setRipplesToFieldName(RFA_String(strval, 0, false));

		// Get MF Field Type
		if ((curPos = getCopyUntilSpace( fline, curPos, (char*)&strval)) < 0)
		{
			printf("Cannot find Field Type (Line=%d).\n",lineNum);
			delete pFieldDef;
			return false;
		}
		pFieldDef->setMFFieldTypeName(RFA_String(strval, 0, false));

		// Get MF Field Length
		if ((curPos = getCopyUntilSpace( fline, curPos, (char*)&strval)) < 0)
		{
			printf("Cannot find Field length (Line=%d).\n",lineNum);
			delete pFieldDef;
			return false;
		}
		pFieldDef->setMFFieldLength(atoi(strval));

		if ((curPos = getCopyUntilSpace( fline, curPos, (char*)&strval)) < 0)
		{
			printf("Cannot find EnumLen or RwfType (Line=%d).\n",lineNum);
			delete pFieldDef;
			return false;
		}
       
		if (strval[0] == '(')
		{
			if ((curPos = getCopyUntilSpace( fline, curPos, (char*)&strval)) < 0)
			{
				printf("Cannot find EnumLen (Line=%d).\n",lineNum);
				delete pFieldDef;
				return false;
			}
			pFieldDef->setMFEnumLength(atoi(strval));

			if ((curPos = getCopyUntilSpace( fline, curPos, (char*)&strval)) < 0)
			{
				printf("Cannot find end ')' in EnumLen (Line=%d).\n",lineNum);
				delete pFieldDef;
				return false;
			}
			if (strval[0] != ')')
			{
				printf("No ')' at end of EnumLen definition (Line=%d).\n",lineNum);
				delete pFieldDef;
				return false;
			}
			if ((curPos = getCopyUntilSpace( fline, curPos, (char*)&strval)) < 0)
			{
				printf("Cannot find Rwf Type (Line=%d).\n",lineNum);
				delete pFieldDef;
				return false;
			}
		}
		
		pFieldDef->setDataType(RFA_String(strval, 0, false));

		if ((curPos = getCopyUntilSpace( fline, curPos, (char*)&strval)) < 0)
		{
			printf("Cannot find Rwf Length (Line=%d).\n",lineNum);
			delete pFieldDef;
			return false;
		}

		pFieldDef->setMaxFieldLength(atoi(strval));
		
		// Skip the rest since it's not used
		_dictionary.putFieldDef( pFieldDef );

		if (RDMDict::Trace & 0x10)
			pFieldDef->dumpFieldDef( );
	}

	fclose(fp);
	return true;
}

bool RDMFileDictionaryDecoder::load( const RFA_String& appendix_a, 
									 const RFA_String& enumTypeDef )
{
	//check to see if strings are empty
	if(appendix_a.empty())
	{
		printf("\nField dictionary name is empty!\n");
		return false;
	}
	if(enumTypeDef.empty())
	{
		printf("\nenumTypeDef name is empty!\n");
		return false;
	}


	// load appendix_a and enumTypeDef
	if(!loadAppendix_A( appendix_a ) || !loadEnumTypeDef( enumTypeDef, _enumDict ))
		return false;

	associate();
	return true;
}
int RDMFileDictionaryDecoder::getCopyUntilSpace( char * line, int curpos, char *copyToStr )
{
	int copyIdx=0;

		/* Remove whitespace */
	while ((line[curpos] != '\0') && isspace(line[curpos]))
		curpos++;

		/* Copy information until whitespace is found */
	while ((line[curpos] != '\0') && !isspace(line[curpos]))
		copyToStr[copyIdx++] = line[curpos++];

		/* Null Terminate */
	copyToStr[copyIdx] = '\0';
	return( copyIdx == 0 ? -1 : curpos);
}

int RDMFileDictionaryDecoder::getCopyQuotedStr( char * line, int curpos, char *copyToStr )
{
	int copyIdx=0;

		/* Remove whitespace */
	while ((line[curpos] != '\0') && isspace(line[curpos]))
		curpos++;

		/* Check for quote */
	if (line[curpos++] != '"')
		return -1;

		/* Copy information until another quote is found */
	while ((line[curpos] != '\0') && (line[curpos] != '"'))
		copyToStr[copyIdx++] = line[curpos++];

		/* Null Terminate */
	copyToStr[copyIdx] = '\0';

	return( line[curpos] != '"' ? -1 : curpos + 1 );
}
int RDMFileDictionaryDecoder::getCopyPoundOrQuoteStr( bool& quoteFnd, char * line, int curpos, char *copyToStr )
{
	int copyIdx=0;

		/* Remove whitespace */
	while ((line[curpos] != '\0') && isspace(line[curpos]))
		curpos++;

		/* Check for quote */
	if( line[curpos] == '"' )
	{
		quoteFnd = true;
	}
	else if(line[curpos] == '#')
	{
		quoteFnd = false;
	}
	else
	{
		return -1;
	}

	++curpos;

	char chFnd = '"';
	if ( !quoteFnd )
	{
		chFnd = '#';
	}
	
	/* Copy information until another chFnd is found */
	while ((line[curpos] != '\0') && (line[curpos] != chFnd))
		copyToStr[copyIdx++] = line[curpos++];


		/* Null Terminate */
	copyToStr[copyIdx] = '\0';

	return( line[curpos] != chFnd ? -1 : curpos + 1 );
}
////////////////////////////////////////////////////////////////////
/// End RDMFileDictionaryDecoder
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
/// Begin RDMNetworkDictionaryDecoder
////////////////////////////////////////////////////////////////////

RDMNetworkDictionaryDecoder::RDMNetworkDictionaryDecoder(RDMFieldDict & dictionary, UInt32 dictVerbosityType)
:	appendixALoaded(false),
	enumTypeDefLoaded(false),
	_dictionary(dictionary),
	_defId(0),
	_dictVerbosityType(dictVerbosityType),
	_def(0),
	_defTemp(0),
	_enumDict(0),
	_first(true),
	pEnumDef(0)
{
	_enumDict = &dictionary.enumDict();
}

RDMNetworkDictionaryDecoder::~RDMNetworkDictionaryDecoder()
{
}

UInt32	RDMNetworkDictionaryDecoder::getDictVerbosityType()
{ return _dictVerbosityType;}


void RDMNetworkDictionaryDecoder::loadAppendix_A(const rfa::common::Data &data, bool moreFragments)
{
	if (RDMDict::Trace & 0x1)
		//cout << "Network loadAppendix_A " << (moreFragments ? "" : "last") << endl;
		printf("Network loadAppendix_A %s\n",(moreFragments ? "" : "last"));

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
		//cout << "Network loadEnumTypeDef " << (moreFragments ? "" : "last") << endl;
		printf("Network loadEnumTypeDef %s\n",(moreFragments ? "" : "last"));

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

	// all series are completed.  No need to keep the _defTemp around.
	delete _defTemp;
}

void RDMNetworkDictionaryDecoder::decodeFieldDefDictionary( const Data& data )
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
		/*
		cout << "type: " << _dictionary.getDictType() << " dictId: " << _dictionary.getDictId() 
			<< " ver: " << _dictionary.getVersion() << endl;
		*/
		printf("type: %d dictId: %d ver: %s\n",_dictionary.getDictType(),
				_dictionary.getDictId(),
				_dictionary.getVersion().c_str());
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
		printf("Dictionary not encoded correctly\n");
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
		pFieldDef->setFieldId((Int16)(static_cast<const DataBuffer&>(element3.getData())).getInt());
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
		pFieldDef->setMaxFieldLength( static_cast<UInt16>(static_cast<const DataBuffer&>(element9.getData()).getUInt()) );
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
	pFieldDef->setFieldId((Int16)(static_cast<const DataBuffer&>(it.value().getData())).getInt());

	it.forth();
	pFieldDef->setRipplesToFid(static_cast<const DataBuffer&>(it.value().getData()).getInt());

	it.forth();
	pFieldDef->setMFFieldType(static_cast<const DataBuffer&>(it.value().getData()).getInt());

	it.forth();
	pFieldDef->setMFFieldLength(static_cast<const DataBuffer&>(it.value().getData()).getUInt());
	
	it.forth(); 
	pFieldDef->setDataType( static_cast<UInt16>(static_cast<const DataBuffer&>(it.value().getData()).getUInt()) );
	
	it.forth();
	pFieldDef->setMaxFieldLength( static_cast<UInt16>(static_cast<const DataBuffer&>(it.value().getData()).getUInt()) );

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
		/*
		cout << "type: " << _enumDict->getDictType() << " dictId: " << _enumDict->getDictId() 
			<< " ver: " << _enumDict->getVersion() << endl;
		*/
		printf("type: %d dictId: %d ver: %s\n",
			_enumDict->getDictType(),
			_enumDict->getDictId(),
			_enumDict->getVersion().c_str() );
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
		enumDef->addFid(static_cast<Int16> (fidBuffer.getInt()));
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

		RFA_String display = displayBuffer.getAsString();

		UInt16 enumeration = valueBuffer.getEnumeration();
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


