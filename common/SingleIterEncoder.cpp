#ifdef WIN32
#pragma warning(disable : 4786)
#endif
#include <iostream>
#include <new>
#include <cstdlib>
#include "SingleIterEncoder.h"


#define DICT_ID	1
#define FIELD_LIST_ID 3
#define INITIAL_TRD_PRICE 100

using namespace rfa::common;
using namespace rfa::data;


///////////////////////////////////////////////////////////////////////////////
/// Constructor
///
SingleIterEncoder::SingleIterEncoder() :
	_rdmFieldDict(0),
	_pDictionaryEncoder(0),
	_trdPrice(INITIAL_TRD_PRICE),
	_bDictionaryLoaded(false),
	_bGenericDomainModel(false),
	_genericDomainModel(0),
	_bMarketPriceDomainModel(true)
{
}

///////////////////////////////////////////////////////////////////////////////
/// Destructor
///
SingleIterEncoder::~SingleIterEncoder()
{
	if ( _pDictionaryEncoder )
		delete _pDictionaryEncoder;
	if ( _rdmFieldDict )
		delete _rdmFieldDict;
}

void SingleIterEncoder::encodeDirectoryMsg( RespMsg* respMsg, const AttribInfo & rAttribInfo, RespStatus & rRStatus, bool bSolicited)
{
	assert(respMsg);

	// set MsgModelType
	respMsg->setMsgModelType(MMT_DIRECTORY);		
	
	if(bSolicited)
	{
		// Set response Type
		respMsg->setRespType(RespMsg::RefreshEnum);

		// Set indication mask
		respMsg->setIndicationMask(RespMsg::RefreshCompleteFlag);

		// Set RespTypeNum
		respMsg->setRespTypeNum(REFRESH_SOLICITED);

		// Set respStatus 
		respMsg->setRespStatus(rRStatus);

		// Set attributes 
		respMsg->setAttribInfo(rAttribInfo);
	}
	else
	{
		// Set response Type
		respMsg->setRespType(RespMsg::UpdateEnum);
	}
}


void SingleIterEncoder::encodeDictionaryMsg(RespMsg* respMsg,  const AttribInfo & rAttribInfo,RespStatus & rRStatus)
{
	assert(respMsg);

	// set MsgModelType
	respMsg->setMsgModelType(MMT_DICTIONARY);		
	
	// Set response Type
	respMsg->setRespType(RespMsg::RefreshEnum);

	// set IndicationMask
	respMsg->setIndicationMask( respMsg->getIndicationMask() | RespMsg::RefreshCompleteFlag );    

	// Set attribute 
	respMsg->setAttribInfo(rAttribInfo);

	// Set respStatus 
	respMsg->setRespStatus(rRStatus);
	return;
}

void SingleIterEncoder::encodeLoginMsg(RespMsg* respMsg, const AttribInfo & rAttribInfo, RespStatus & rRStatus)
{
	assert(respMsg);

	// set MsgModelType
	respMsg->setMsgModelType(MMT_LOGIN);	

	// Set response Type
	respMsg->setRespType(RespMsg::RefreshEnum);
			
	// set RespTypeNum
	respMsg->setRespTypeNum(0);   //for solicited refresh, value is 0 

	// Set attribute 
	respMsg->setAttribInfo(rAttribInfo);

	// Set respStatus 
	respMsg->setRespStatus(rRStatus);
	return;
}

void SingleIterEncoder::encodeMarketPriceMsg( RespMsg* respMsg, RespMsg::RespType respType, const AttribInfo & rAttribInfo, RespStatus & rRStatus, QualityOfService* pQoS, bool bSetAttribute, bool bSolicited)
{
	assert( respMsg );

	// set MsgModelType
	respMsg->setMsgModelType( MMT_MARKET_PRICE );	

	// Set response Type
	respMsg->setRespType( respType );

	if ( respType == RespMsg::RefreshEnum )
	{
		// Set respStatus
		respMsg->setRespStatus( rRStatus );
		// set IndicationMask
		respMsg->setIndicationMask( respMsg->getIndicationMask() | RespMsg::RefreshCompleteFlag );    // TODO: This probably should be configureable for other indicationMask settings
		// set RespTypeNum
		if(bSolicited)
			respMsg->setRespTypeNum( REFRESH_SOLICITED );   //for solicited refresh, value is 0
		else
			respMsg->setRespTypeNum( REFRESH_UNSOLICITED );   //for unsolicited refresh, value is 1
	}
	else if ( respType == RespMsg::UpdateEnum )
	{
		// set indication mask for update
		respMsg->setIndicationMask( INSTRUMENT_UPDATE_UNSPECIFIED );
		// set RespTypeNum
		respMsg->setRespTypeNum( INSTRUMENT_UPDATE_UNSPECIFIED );   //for unspecified update response, value is 0
	}
	else
	{
		// this is a status message
		// Set respStatus
		respMsg->setRespStatus( rRStatus );
		// set IndicationMask
		respMsg->setIndicationMask( respMsg->getIndicationMask() );    // TODO: This probably should be configureable for other indicationMask settings
	}

	// Set attribute    
	if ( bSetAttribute )
		respMsg->setAttribInfo( rAttribInfo );

	// Set Qos 
	if( pQoS )
		respMsg->setQualityOfService( *pQoS );
}

void SingleIterEncoder::reencodeAttributeInfoAttrib(const Data& reqAttrib, Data& respAttrib)
{
	SingleReadIterator rIter;
	SingleWriteIterator wIter;

	ElementList& elementList = static_cast<ElementList&>(respAttrib);
	ElementEntry element;
	DataBuffer elementData;

	wIter.initialize(elementList, 15000);
	wIter.start( elementList );

	if(!reqAttrib.isBlank())
	{
		switch(reqAttrib.getDataType())
		{
			case ElementListEnum:
				{
					const ElementList& inputEL = static_cast<const ElementList&>(reqAttrib);
					rIter.start(inputEL);

					while(!rIter.off())
					{
						ElementEntry& elem = static_cast<ElementEntry&>(const_cast<DataEntry&>(rIter.value()));
						wIter.bind(elem);
						wIter.setData(elem.getData());
						rIter.forth();
					}
					
					break;
				}	
			default:
				//should not go here
				printf("wrong data type in login request message\n");
				break;
		}
	}

	//add support view
	element.clear();
	element.setName(rfa::rdm::ENAME_SUPPORT_VIEW);
	UInt32 supportView = 1;
    wIter.bind( element );
	wIter.setUInt(supportView);

	wIter.complete();
}

void SingleIterEncoder::encodeMarketPriceDataBody( Data* pData, rfa::common::RFA_Vector<int>* fidArr )
{
	FieldList* pFieldList;
	assert(pData);
	pFieldList = static_cast<FieldList*>(pData);
	if ( fidArr && fidArr->size() > 0 )
		encodeMarketPriceFieldListEx( pFieldList, *fidArr);
	else
		encodeMarketPriceFieldList( pFieldList);
}

void SingleIterEncoder::encodeDirectoryDataBody(Data* pData, RFA_String & rSvcName, RFA_String & rVendName, QualityOfService * pQoS)
{
	Map* pMap;
	assert(pData);
	pMap = static_cast<Map*>(pData);
	encodeDirectoryMap( pMap, rSvcName,rVendName, pQoS);
}

void SingleIterEncoder::loadDictionaryFromFile(RFA_String &appendix_a_path, RFA_String &enumtype_def_path )
{
	if (_bDictionaryLoaded) 
		return;

		_pDictionaryEncoder = new RDMDictionaryEncoder();
		_rdmFieldDict = new RDMFieldDict();

	RDMFileDictionaryDecoder decoder(*_rdmFieldDict);


	if( decoder.load(appendix_a_path, enumtype_def_path) )
	{
		RFA_String	version("1.1", 4, false);
		_rdmFieldDict->setVersion(version);
		_rdmFieldDict->setDictId(1);
		_rdmFieldDict->enumDict().setVersion(version);
		_rdmFieldDict->enumDict().setDictId(1);
		_bDictionaryLoaded = true;
	}
	else
	{
		std::cout << "initDictionary failed to load dictionary" << std::endl << std::endl;
	} 
}

void SingleIterEncoder::encodeFieldDictionary(Series & data, RFA_String &appendix_a_path, RFA_String &enumtype_def_path) 
{
	loadDictionaryFromFile(appendix_a_path,enumtype_def_path);
	if ( _bDictionaryLoaded )
		_pDictionaryEncoder->encodeFieldDictionary(data, *_rdmFieldDict, DICTIONARY_NORMAL);
}	
	
void SingleIterEncoder::encodeEnumDictionary(Series & data, RFA_String &appendix_a_path, RFA_String &enumtype_def_path) 
{
	loadDictionaryFromFile(appendix_a_path,enumtype_def_path);
	if ( _bDictionaryLoaded )
		_pDictionaryEncoder->encodeEnumDictionary(data, _rdmFieldDict->enumDict(), DICTIONARY_NORMAL);
}	

void SingleIterEncoder::encodeMarketPriceFieldList(FieldList* pFieldList)
{
	assert(pFieldList);

	//here pFieldList is the same pointer passed in every time this function gets called. So _marketPriceSWIter
	//has been initialized with this object, call clear() for reusing after the first time call.
	if(!_marketPriceSWIter.isInitialized())
	{
		_marketPriceSWIter.initialize(*pFieldList, 65000);
	}
	else
		_marketPriceSWIter.clear();

	pFieldList->setInfo(DICT_ID, FIELD_LIST_ID);		//specify values that are passed through to consumer application.
	_marketPriceSWIter.start(*pFieldList);

	//encode Fields
	FieldEntry field;

	// setup RDNDISPLAY field
	field.setFieldID(2);				// RDNDISPLAY
	_marketPriceSWIter.bind(field);
	_marketPriceSWIter.setInt(100);

	// setup TRDPRC_1 field
	field.setFieldID(6);			//  TRDPRC_1
	_marketPriceSWIter.bind(field);
	_marketPriceSWIter.setReal(_trdPrice++, ExponentNeg2);

	_marketPriceSWIter.complete();
}


void SingleIterEncoder::encodeMarketPriceFieldListEx(FieldList* pFieldList, rfa::common::RFA_Vector<int>& fidArr)
{
	unsigned int fidsCount = fidArr.size();
	if ( !fidsCount )
		return;

	assert(pFieldList);

	//here pFieldList is the same pointer passed in every time this function gets called. So _marketPriceSWIter
	//has been initialized with this object, call clear() for reusing after the first time call.
	if(!_marketPriceSWIter.isInitialized())
	{
		_marketPriceSWIter.initialize(*pFieldList, 65000);
	}
	else
		_marketPriceSWIter.clear();

	static long priceVal = 10;
	static long BidSize = 1000;

	pFieldList->setInfo(DICT_ID, FIELD_LIST_ID);		//specify values that are passed through to consumer application.
	_marketPriceSWIter.start(*pFieldList);

	//encode Fields
	FieldEntry field;
	DataBuffer dataBuffer(true);
	for(unsigned int index = 0; index < fidsCount; ++index)
	{
		field.clear();
		field.setFieldID(fidArr[index]);

		switch(fidArr[index])
		{
		case 2:	// Setup RDNDISPLAY field
		{
			_marketPriceSWIter.bind(field);
			_marketPriceSWIter.setInt(100);
			break;
		}
		case 4:	// Setup RDN_EXCHID  field
		{
			_marketPriceSWIter.bind(field);
			_marketPriceSWIter.setEnum(155);
			break;
		}
		case 6:// Setup TRDPRC_1 field
		{
			_marketPriceSWIter.bind(field);
			_marketPriceSWIter.setReal(priceVal++, ExponentNeg2);
			break;
		}
		case 11: // NETCHNG_1
		{
			_marketPriceSWIter.bind(field);
			_marketPriceSWIter.setReal(BidSize++, Exponent0);
			break;
		}
		case 22: // Setup BID field
		{
			_marketPriceSWIter.bind(field);
			_marketPriceSWIter.setReal(priceVal-2, ExponentNeg2);
			break;
		}
		case 25: // Setup ASK field
		{
			_marketPriceSWIter.bind(field);
			_marketPriceSWIter.setReal(priceVal+2, ExponentNeg2);
			break;
		}
		case 30: // Bid size
		{
			_marketPriceSWIter.bind(field);
			_marketPriceSWIter.setReal(BidSize++, Exponent0);
			break;
		}
		case 31: // Ask Size
		{
			_marketPriceSWIter.bind(field);
			_marketPriceSWIter.setReal(BidSize - 1, Exponent0);
			break;
		}
		case 32: // Setup ACVOL_1 field
		{
			_marketPriceSWIter.bind(field);
			_marketPriceSWIter.setReal(100000, Exponent0);
			break;
		}
		case 38: // Setup DIVIDEND_DATE field
		{
			_marketPriceSWIter.bind(field);
			_marketPriceSWIter.setDate(2005, 12, 10);
			break;
		}
		case 118: // Price code
		{
            _marketPriceSWIter.bind(field);
			_marketPriceSWIter.setEnum(32);
			break;
		}
		case 267: // setup ASK_TIME  field
		{
			_marketPriceSWIter.bind(field);
			_marketPriceSWIter.setTime(rand() % 12, rand() % 60, rand() % 60);
			break;
		}
		case 293: // BID MMID 1
		{
			RFA_String BidStr("Bid MMID data", 0, false);
			_marketPriceSWIter.bind(field);
			_marketPriceSWIter.setString(BidStr, DataBuffer::StringRMTESEnum);
			break;
		}
		case 296: // Ask MMID 1
		{
			RFA_String AskStr("Ask MMID data", 0, false);
			_marketPriceSWIter.bind(field);
			_marketPriceSWIter.setString(AskStr, DataBuffer::StringRMTESEnum);
			break;
		}
		case 1025: // QUOTIM
		{
			_marketPriceSWIter.bind(field);
			_marketPriceSWIter.setTime(rand() % 12, rand() % 60, rand() % 60);
			break;
		}
		default:
			printf("Consumer requested unsupported FID = %d\n", fidArr[index]);
			break;
		} //end switch 
	} //end for loop

	_marketPriceSWIter.complete();
}



void SingleIterEncoder::encodeDirectoryMap( Map* pMap, RFA_String & rSvcName, RFA_String & rVendName, QualityOfService * pQoS)
{
	assert(pMap);

	//since pMap object passed in is always different every time this function gets called,
	//so call initialize() to associate _dirSWIter with new pMap object every time.
	//the single iter with diff pMap.
	_dirSWIter.initialize(*pMap, 320000);
	
	pMap->setIndicationMask(Map::EntriesFlag);
	pMap->setKeyDataType(DataBuffer::StringAsciiEnum);    
	pMap->setTotalCountHint(1);  // Provides a hint to the consuming side of how many map entries are to be provided. In this example there is only one service provided so the totalcounthint is one.
	_dirSWIter.start(*pMap, FilterListEnum);

	//********* Begin Encode MapEntry1**********
	MapEntry mapEntry;
	mapEntry.setAction(MapEntry::Add);

 	// Set ServiceName as key for MapEntry
	DataBuffer keyDataBuffer(true);
	keyDataBuffer.setFromString(rSvcName, DataBuffer::StringAsciiEnum);
	mapEntry.setKeyData(keyDataBuffer);

	_dirSWIter.bind(mapEntry);
	FilterList filterList;
	encodeDirectoryFilterList(&filterList, rSvcName, rVendName,pQoS);	

	_dirSWIter.complete();
}




void SingleIterEncoder::encodeDirectoryFilterList( FilterList* pFilterList, RFA_String & rSvcName, RFA_String & rVendName, QualityOfService * pQoS)
{
	assert(pFilterList);

	pFilterList->setTotalCountHint(2);	// Specify 2 because there is only two filter entries
	_dirSWIter.start(*pFilterList, ElementListEnum);  

	//Encode FilterEntry
	FilterEntry filterEntry;
	filterEntry.setFilterId(SERVICE_INFO_ID);
	filterEntry.setAction(FilterEntry::Set);		// Set this to Set since this is the full filterEntry
	_dirSWIter.bind(filterEntry, ElementListEnum);
	//Encode ElementList for Service Info
	ElementList elementList;
	encodeDirectoryInfoElementList(&elementList, rSvcName, rVendName, pQoS);

	filterEntry.clear();
	elementList.clear();
	filterEntry.setFilterId(SERVICE_STATE_ID);
	filterEntry.setAction(FilterEntry::Set);   // Set this to Set since this is the full filterEntry
	_dirSWIter.bind(filterEntry, ElementListEnum);
	//Encode ElementList for Service State
	encodeDirectoryStateElementList(&elementList);
		
	_dirSWIter.complete();
}

void SingleIterEncoder::encodeDirectoryInfoElementList(ElementList* pElementList, RFA_String & rSvcName, RFA_String & rVendName, QualityOfService * pQoS)
{
	assert(pElementList);

	_dirSWIter.start(*pElementList);

	ElementEntry element;

	// Encode Name
	element.setName(rfa::rdm::ENAME_NAME);
	_dirSWIter.bind(element);
	_dirSWIter.setString(rSvcName, DataBuffer::StringAsciiEnum);

	// Encode Vendor
	element.setName(rfa::rdm::ENAME_VENDOR);
	_dirSWIter.bind(element);
	_dirSWIter.setString(rVendName, DataBuffer::StringAsciiEnum);

	// Encode  isSource 
	element.setName(rfa::rdm::ENAME_IS_SOURCE);
	_dirSWIter.bind(element);
	_dirSWIter.setUInt(1);

	// Encode Capabilities
	element.setName(rfa::rdm::ENAME_CAPABILITIES);
	Array array;
	_dirSWIter.bind(element);
	encodeDirectoryArray(&array);

	// Encode DictionariesUsed
	element.setName(rfa::rdm::ENAME_DICTIONARYS_USED);
	array.clear(); 
	_dirSWIter.bind(element);
	encodeDirectoryArrayDictUsed(&array);

	// Encode Quality of Service
	if(pQoS)
	{
		element.setName(rfa::rdm::ENAME_QOS);
		Array arrayqos;
		_dirSWIter.bind(element);
		encodeDirectoryArrayQoS(&arrayqos,pQoS);
	}

	_dirSWIter.complete();
}

void SingleIterEncoder::encodeDirectoryStateElementList(ElementList* pElementList)
{
	assert(pElementList);
	_dirSWIter.start(*pElementList);
	
	ElementEntry element;

	//Encode Element ServiceState
	element.setName(rfa::rdm::ENAME_SVC_STATE);

	int mType = 1 ; // Yes
	_dirSWIter.bind(element);
	_dirSWIter.setUInt((UInt)mType);

	//Encode Element AcceptingRequests
	element.setName(rfa::rdm::ENAME_ACCEPTING_REQS);
	mType = 1; // Accepts requests
	_dirSWIter.bind(element);
	_dirSWIter.setUInt((UInt)mType);

	_dirSWIter.complete();
}



void SingleIterEncoder::encodeDirectoryArray( Array* pArray)
{
	assert(pArray);
	_dirSWIter.start(*pArray, DataBuffer::UIntEnum);
					
	// Specify Dictionary as a capability
	ArrayEntry arrayEntry;
	int mType = MMT_DICTIONARY;
	_dirSWIter.bind(arrayEntry);
	_dirSWIter.setUInt(mType);

	if(_bGenericDomainModel)
	{
		// Specify Generic Domain Model as a capability
		mType = _genericDomainModel;
		_dirSWIter.bind(arrayEntry);
		_dirSWIter.setUInt(mType);
	}
	if(_bMarketPriceDomainModel)
	{
		// Specify Market Price as a capability
		mType = MMT_MARKET_PRICE;
		_dirSWIter.bind(arrayEntry);
		_dirSWIter.setUInt(mType);
	}
	_dirSWIter.complete();
}

void SingleIterEncoder::encodeDirectoryArrayDictUsed( Array* pArray)
{
	assert(pArray);
	_dirSWIter.start(*pArray, DataBuffer::StringAsciiEnum);
					
	// Specify Dictionary Files uesd
	ArrayEntry arrayEntry;
	RFA_String fldStr("RWFFld", 0, false);
	_dirSWIter.bind(arrayEntry);
	_dirSWIter.setString(fldStr, DataBuffer::StringAsciiEnum);

	RFA_String enumStr("RWFEnum", 0, false);
	_dirSWIter.bind(arrayEntry);
	_dirSWIter.setString(enumStr, DataBuffer::StringAsciiEnum);
		
	_dirSWIter.complete();
}

void SingleIterEncoder::encodeDirectoryArrayQoS( Array* pArray, QualityOfService * pQoS)
{
	assert(pArray);
	_dirSWIter.start(*pArray, DataBuffer::QualityOfServiceInfoEnum);
	
	ArrayEntry arrayEntry;
	_dirSWIter.bind(arrayEntry);
	_dirSWIter.setQos((UInt16)pQoS->getTimeliness(), (UInt16)pQoS->getRate());

	_dirSWIter.complete();

}
