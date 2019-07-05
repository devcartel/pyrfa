#ifdef WIN32
#pragma warning(disable : 4786)
#endif
#include <iostream>
#include <new>
#include <cstdlib>
#include "Encoder.h"
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define DICT_ID    1
#define FIELD_LIST_ID 3

using namespace rfa::common;
using namespace rfa::data;


///////////////////////////////////////////////////////////////////////////////
/// Constructor
///
Encoder::Encoder() :
    _rdmFieldDict(0),
    _pDictionaryEncoder(0),
    _bDictionaryLoaded(false),
    _bGenericDomainModel(false),
    _genericDomainModel(0),
    _bMarketPriceDomainModel(false),
    _bSymbolListDomainModel(false),
    _bMarketByOrderDomainModel(false),
    _bMarketByPriceDomainModel(false),
    _bHistoryDomainModel(false),
    _debug(true)
{
}

///////////////////////////////////////////////////////////////////////////////
/// Destructor
///
Encoder::~Encoder()
{
    if ( _pDictionaryEncoder )
        delete _pDictionaryEncoder;
    if ( _rdmFieldDict )
        delete _rdmFieldDict;
}

void Encoder::encodeDirectoryMsg(RespMsg* respMsg, const AttribInfo& rAttribInfo, RespStatus& rRStatus, bool bSolicited)
{
	assert(respMsg);
	respMsg->setMsgModelType(MMT_DIRECTORY);
	if(bSolicited)
	{
		respMsg->setRespType(RespMsg::RefreshEnum);
		respMsg->setIndicationMask(RespMsg::RefreshCompleteFlag | RespMsg::ClearCacheFlag );
		respMsg->setRespTypeNum(REFRESH_SOLICITED);
		respMsg->setRespStatus(rRStatus);
		respMsg->setAttribInfo(rAttribInfo);
	}
	else
    {
		respMsg->setRespType(RespMsg::UpdateEnum);
        respMsg->setRespTypeNum(INSTRUMENT_UPDATE_UNSPECIFIED);
    }
}

void Encoder::encodeDictionaryMsg(RespMsg* respMsg, const AttribInfo& rAttribInfo,RespStatus& rRStatus, rfa::common::UInt8 indicationMask)
{
	assert(respMsg);
	respMsg->setMsgModelType(MMT_DICTIONARY);
	respMsg->setRespType(RespMsg::RefreshEnum);
	respMsg->setIndicationMask(indicationMask | RespMsg::ClearCacheFlag );    
	respMsg->setAttribInfo(rAttribInfo);
	respMsg->setRespStatus(rRStatus);
	return;
}

void Encoder::encodeLoginMsg(RespMsg* respMsg, const AttribInfo& rAttribInfo, RespStatus& rRStatus)
{
	assert(respMsg);
	respMsg->setMsgModelType(MMT_LOGIN);
	respMsg->setRespType(RespMsg::RefreshEnum);
	respMsg->setRespTypeNum(REFRESH_SOLICITED);
	respMsg->setAttribInfo(rAttribInfo);
	respMsg->setRespStatus(rRStatus);
	respMsg->setIndicationMask(RespMsg::RefreshCompleteFlag| RespMsg::ClearCacheFlag ) ;
	return;
}
void Encoder::encodeMarketPriceMsg( RespMsg* respMsg, RespMsg::RespType respType, const AttribInfo & rAttribInfo, RespStatus & rRStatus, QualityOfService* pQoS, bool bSetAttribute, bool bSolicited)
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

void Encoder::encodeMarketPriceDataBody(FieldList* pFieldList, RespMsg::RespType respType, const rfa::common::RFA_Vector<rfa::common::RFA_String>& fieldList, const RDMFieldDict* pDict)
{
    if(_debug)
        cout << "[Encoder::encodeMarketPriceDataBody]" << endl;
    assert(pFieldList);
    FieldListWriteIterator fieldListWIt;
    fieldListWIt.start(*pFieldList);

    pFieldList->setInfo(DICT_ID, FIELD_LIST_ID);

    //encode Fields
    FieldEntry field;
    DataBuffer dataBuffer( true );

    // encode fields specific to a refresh
    if (respType == RespMsg::RefreshEnum)
    {
    }

    /*
    // current time of publishing
    // don't support milliseconds at the moment
    time_t tb;
    struct tm * now;
    time(&tb);
#if defined (WIN32) || defined (_WIN32)
    now = localtime( &tb );
#else
    struct tm ltm;
    now = localtime_r(&tb, &ltm);
#endif
    */

    // encoding fields
    const rfa::common::RFA_String n="ABCDEFDGHIJKLMNOPQRSTUVWXYZ_ %#/*\xDE\xFE";
    const char* empty="";
    bool isFieldName=false;
    rfa::common::Int16 fieldID;
    rfa::common::RFA_String fieldName;
    size_t length = fieldList.size();
    
    for (size_t indx = 0; indx != length; indx=indx+2) {

        // check if given field is in fieldID or fieldName format (number or string)
        for(unsigned int i = 0; i <= n.size(); ++i) {
            if( fieldList[indx].toUpper().find(n.substr(i,1)) >= 0 ) {
                isFieldName = true;
                break;
            }
        }
        
        // if fieldID is given, convert RFA_String to Int16
        // and use dict to get its coresponding name
        const RDMFieldDef* fieldDef = NULL;
        if(isFieldName) {
            fieldDef = pDict->getFieldDef(fieldList[indx].toUpper());
            if(!fieldDef) {
                if(_debug)
                    cout << fieldList[indx].toUpper().c_str() << "(not found in dict) <<<<<<" << endl;;
                continue;
            }
            fieldID = fieldDef->getFieldId();
        } else {
            fieldID = boost::lexical_cast<rfa::common::Int16>(fieldList[indx].c_str());
            fieldDef = pDict->getFieldDef(fieldID);
            if(!fieldDef) {
                if(_debug)
                    cout << fieldID << "(not found in dict) <<<<<<" << endl;
                continue;
            }
        }
        assert(fieldDef);

        // get field name for debugging
        if(_debug) {
            fieldName = pDict->getFieldDef(fieldID)->getName();
            cout << fieldName.c_str() << "(" << fieldID << ")=";
        }

        int exponentEnum = 14;
        rfa::common::RFA_String fieldValue(fieldList[indx+1]);

        // if value is blank
        if(fieldValue.empty()) {
            field.setFieldID(fieldID);
            dataBuffer.setBlankData(fieldDef->getDataType());
            field.setData(dataBuffer);
            fieldListWIt.bind(field);
            if(_debug)
                cout << fieldValue.c_str() << endl;
            continue;
        }

        // if value is not blank
        switch(fieldDef->getDataType()) {
            // UInt64
            case rfa::data::DataBuffer::UInt64Enum:
            {
                field.setFieldID(fieldID);                
                dataBuffer.setUInt64(boost::lexical_cast<rfa::common::UInt64>(fieldValue.c_str()));
                field.setData(dataBuffer);
                fieldListWIt.bind(field);
                if(_debug)
                    cout << fieldValue.c_str() << endl;
                break;
            }
            // UInt32
            case rfa::data::DataBuffer::UInt32Enum:
            {
                field.setFieldID(fieldID);                
                dataBuffer.setUInt32(boost::lexical_cast<rfa::common::UInt32>(fieldValue.c_str()));
                field.setData(dataBuffer);
                fieldListWIt.bind(field);
                if(_debug)
                    cout << fieldValue.c_str() << endl;
                break;
            }
            // Enum
            case rfa::data::DataBuffer::EnumerationEnum:
            {
                // check if given field is in fieldID or fieldName format (number or string)
                bool isAphanumberic=false;
                for(unsigned int i = 0; i <= n.size(); ++i) {
                    if( fieldValue.toUpper().find(n.substr(i,1)) >= 0 ) {
                        isAphanumberic = true;
                        break;
                    }
                }
                
                // if fieldValue happens to be enum string, 
                // find associated enum value
                rfa::common::Int16 enumVal;
                rfa::common::RFA_String enumStr;
                if(isAphanumberic) {
                    for(rfa::common::Int16 i = 0; i <= pDict->maxPositiveFieldId(); ++i) {
                        pDict->getFieldDef(fieldID)->getEnumString(i,enumStr);
                        if(enumStr == fieldValue) {
                            enumVal = i;
                            break;
                        }
                    }
                } else {
                    enumVal = boost::lexical_cast<rfa::common::UInt16>(fieldValue.c_str());
                    pDict->getFieldDef(fieldID)->getEnumString(enumVal,enumStr);
                }

                field.setFieldID(fieldID);
                dataBuffer.setEnumeration(enumVal);
                field.setData(dataBuffer);
                fieldListWIt.bind(field);
                if(_debug) {
                    cout << enumVal << "(" << enumStr.c_str() << ")" << endl;
                }
                break;
            }
            // Real32 e.g. 8.424 -> 8424e-2
            case rfa::data::DataBuffer::Real32Enum:
            {
                int decimalPosition = fieldValue.find( rfa::common::RFA_String(".") );
                if ( decimalPosition >= 0 ) {
                    exponentEnum = exponentEnum - (fieldValue.size() - decimalPosition - 1);
                }
                // remove decimal point from string
                fieldValue.replace(decimalPosition,1,empty);
                
                rfa::data::Real32 real32;
                real32.setMagnitudeType(exponentEnum);
                real32.setValue(boost::lexical_cast<rfa::common::Int32>(fieldValue.c_str()));
                field.setFieldID(fieldID);
                dataBuffer.setReal32(real32);
                field.setData(dataBuffer);
                fieldListWIt.bind(field);
                if(_debug)
                    cout << fieldValue.c_str() << "e-" << (14-exponentEnum) << endl;
                break;
            }
            // Real64
            case rfa::data::DataBuffer::Real64Enum:
            {
                int decimalPosition = fieldValue.find( rfa::common::RFA_String(".") );
                if ( decimalPosition >= 0 ) {
                    exponentEnum = exponentEnum - (fieldValue.size() - decimalPosition - 1);
                }
                // remove decimal point from string
                fieldValue.replace(decimalPosition,1,empty);
                
                rfa::data::Real64 real64;
                real64.setMagnitudeType(exponentEnum);
                real64.setValue(boost::lexical_cast<rfa::common::Int64>(fieldValue.c_str()));
                field.setFieldID(fieldID);
                dataBuffer.setReal64(real64);
                field.setData(dataBuffer);
                fieldListWIt.bind(field);
                if(_debug)
                    cout << fieldValue.c_str() << "e-" << (14-exponentEnum) << endl;
                break;
            }
            // Time
            case rfa::data::DataBuffer::TimeEnum:
            {
                rfa::data::Time rfaTime;
                if(fieldValue == "now") {
                    boost::posix_time::time_duration td = boost::posix_time::microsec_clock::local_time().time_of_day();
                    rfa::common::RFA_String now(boost::posix_time::to_simple_string(td).c_str());

                    // 08:28:12.080990
                    rfa::common::RFA_String h(now.substr(0,2));
                    rfa::common::RFA_String m(now.substr(3,2));
                    rfa::common::RFA_String s(now.substr(6,2));
                    rfa::common::RFA_String ms(now.substr(9,3));
                    rfa::common::RFA_String us(now.substr(12,3));

                    rfaTime.setHour(boost::lexical_cast<int>(h.c_str()));
                    rfaTime.setMinute(boost::lexical_cast<int>(m.c_str()));
                    rfaTime.setSecond(boost::lexical_cast<int>(s.c_str()));
                    rfaTime.setMillisecond(boost::lexical_cast<long>(ms.c_str()));
                    rfaTime.setMicrosecond(boost::lexical_cast<long>(us.c_str()));

                    //rfaTime.setHour((UInt8)(now->tm_hour));
                    //rfaTime.setMinute((UInt8)(now->tm_min));
                    //rfaTime.setSecond((UInt8)(now->tm_sec));

                    field.setFieldID(fieldID);
                    dataBuffer.setTime(rfaTime);
                    field.setData(dataBuffer);
                    fieldListWIt.bind(field);
                    if(_debug)
                        cout << h.c_str() << ":" << m.c_str() << ":" << s.c_str() << ":" << ms.c_str() << ":" <<  us.c_str() << endl;

                    
                } else {
                    // HH:MM:SS:mmm:uuu or HH:MM:SS:mmm or HH:MM:SS
                    rfa::common::RFA_String h(fieldValue.substr(0,2));
                    rfa::common::RFA_String m(fieldValue.substr(3,2));
                    rfa::common::RFA_String s(fieldValue.substr(6,2));
                    rfa::common::RFA_String ms(fieldValue.substr(9,3));
                    rfa::common::RFA_String us(fieldValue.substr(13,3));
                    if(ms.empty())
                        ms = "000";
                    if(us.empty())
                        us = "000";

                    rfaTime.setHour(boost::lexical_cast<int>(h.c_str()));
                    rfaTime.setMinute(boost::lexical_cast<int>(m.c_str()));
                    rfaTime.setSecond(boost::lexical_cast<int>(s.c_str()));
                    rfaTime.setMillisecond(boost::lexical_cast<long>(ms.c_str()));
                    rfaTime.setMicrosecond(boost::lexical_cast<long>(us.c_str()));

                    field.setFieldID(fieldID);
                    dataBuffer.setTime(rfaTime);
                    field.setData(dataBuffer);
                    fieldListWIt.bind(field);
                    if(_debug)
                        cout << h.c_str() << ":" << m.c_str() << ":" << s.c_str() << ":" << ms.c_str() << ":" << us.c_str() << endl;
                }
                break;
            }
            // Date
            case rfa::data::DataBuffer::DateEnum:
            {
                bool isAphanumberic=false;
                for(unsigned int i = 0; i <= n.size(); ++i) {
                    if( fieldValue.toUpper().find(n.substr(i,1)) >= 0 ) {
                        isAphanumberic = true;
                        break;
                    }
                }

                if(isAphanumberic) {
                    // dd MMM yyyy (22 AUG 2011)
                    rfa::data::Date date;
                    rfa::common::RFA_String month(fieldValue.substr(3,3).toUpper());
                    if(month == "JAN")
                        date.setMonth(1);
                    else if(month == "FEB")
                        date.setMonth(2);
                    else if(month == "MAR")
                        date.setMonth(3);
                    else if(month == "APR")
                        date.setMonth(4);
                    else if(month == "MAY")
                        date.setMonth(5);
                    else if(month == "JUN")
                        date.setMonth(6);
                    else if(month == "JUL")
                        date.setMonth(7);
                    else if(month == "AUG")
                        date.setMonth(8);
                    else if(month == "SEP")
                        date.setMonth(9);
                    else if(month == "OCT")
                        date.setMonth(10);
                    else if(month == "NOV")
                        date.setMonth(11);
                    else if(month == "DEC")
                        date.setMonth(12);
                    else {
                        if(_debug)
                            cout << "(wrong month format) <<<<<<" << endl;
                        continue;
                    }

                    rfa::common::RFA_String y(fieldValue.substr(7,4));
                    rfa::common::RFA_String d(fieldValue.substr(0,2));

                    date.setYear(boost::lexical_cast<int>(y.c_str()));
                    date.setDay(boost::lexical_cast<int>(d.c_str()));

                    field.setFieldID(fieldID);
                    dataBuffer.setDate(date);
                    field.setData(dataBuffer);
                    fieldListWIt.bind(field);
                    if(_debug)
                        cout << d.c_str() << " " << month.c_str() << " " << y.c_str() << endl;
                } else {
                    // yyyymmdd
                    rfa::common::RFA_String y(fieldValue.substr(0,4));
                    rfa::common::RFA_String m(fieldValue.substr(4,2));
                    rfa::common::RFA_String d(fieldValue.substr(6,2));

                    rfa::data::Date date;
                    date.setYear(boost::lexical_cast<int>(y.c_str()));
                    date.setMonth(boost::lexical_cast<int>(m.c_str()));
                    date.setDay(boost::lexical_cast<int>(d.c_str()));

                    field.setFieldID(fieldID);
                    dataBuffer.setDate(date);
                    field.setData(dataBuffer);
                    fieldListWIt.bind(field);
                    if(_debug)
                        cout << y.c_str() << m.c_str() << d.c_str() << endl;
                }
                break;
            }
            // RMTES string
            case rfa::data::DataBuffer::StringRMTESEnum:
            case rfa::data::DataBuffer::StringAsciiEnum:
            {
                field.setFieldID(fieldID);
                dataBuffer.setFromString(fieldValue, rfa::data::DataBuffer::StringAsciiEnum);
                field.setData(dataBuffer);
                fieldListWIt.bind(field);
                if(_debug)
                    cout << fieldValue.c_str() << endl;
                break;
            }
            default: {
                if(_debug)
                    cout << "(dataType not supported)" << endl;
            }
        }
    }
    fieldListWIt.complete();
}

void Encoder::encodeMarketByOrderMsg( RespMsg* respMsg, RespMsg::RespType respType, const AttribInfo & rAttribInfo, RespStatus & rRStatus, QualityOfService* pQoS, bool bSetAttribute, bool bSolicited)
{
    assert( respMsg );

    // set MsgModelType
    respMsg->setMsgModelType( MMT_MARKET_BY_ORDER );    

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

void Encoder::encodeMarketByOrderDataBody(Map* pMap, FieldList* pFieldList, RespMsg::RespType respType, const rfa::common::RFA_Vector<rfa::common::RFA_String>& fieldList, const RDMFieldDict* pDict, const std::string& mapAction, const std::string& mapKey)
{
    if(_debug)
        cout << "[Encoder::encodeMarketByOrderDataBody]" << endl;

    if(mapAction == "" || mapKey == "") {
        cerr << "[Encoder::encodeMarketByOrderDataBody] mapAction or mapKey is empty." << endl;
    }

    assert(pMap);
    MapWriteIterator mapWIt;
    mapWIt.start(*pMap);
    pMap->setKeyDataType(DataBuffer::BufferEnum);    
    pMap->setTotalCountHint(1);  // Provides a hint to the consuming side of how many map entries are to be provided. In this example there is only one service provided so the totalcounthint is one.


    //********* Begin Encode MapEntry1 **********
    MapEntry mapEntry;
    if(mapAction == "add") {
        mapEntry.setAction(MapEntry::Add);
    } else if(mapAction == "update") {
        mapEntry.setAction(MapEntry::Update);
    } else if(mapAction == "delete") {
        mapEntry.setAction(MapEntry::Delete);
    } else {
        return;
    }
    
     // Set order id as key for MapEntry (e.g. 463463B)
    RFA_String key;
    key.set(mapKey.c_str(), mapKey.length(), false);
    DataBuffer keyDataBuffer(true);
    keyDataBuffer.setFromString(key, DataBuffer::BufferEnum);
    mapEntry.setKeyData(keyDataBuffer);

    if(_debug) {
        cout << "Action: " << mapAction.c_str() << endl;
        cout << "Key   : " << key.c_str() << endl;
    }
    
    if(mapAction != "delete") {
        // encode fieldList using encodeMarketPriceDataBody()
        encodeMarketPriceDataBody(pFieldList, respType, fieldList, pDict);
        mapEntry.setData(static_cast<Data&>(*pFieldList));
    }
    mapWIt.bind(mapEntry);
    mapWIt.complete();
}

void Encoder::encodeMarketByPriceMsg( RespMsg* respMsg, RespMsg::RespType respType, const AttribInfo & rAttribInfo, RespStatus & rRStatus, QualityOfService* pQoS, bool bSetAttribute, bool bSolicited)
{
    assert( respMsg );

    // set MsgModelType
    respMsg->setMsgModelType( MMT_MARKET_BY_PRICE );    

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

void Encoder::encodeMarketByPriceDataBody(Map* pMap, FieldList* pFieldList, RespMsg::RespType respType, const rfa::common::RFA_Vector<rfa::common::RFA_String>& fieldList, const RDMFieldDict* pDict, const std::string& mapAction, const std::string& mapKey)
{
    if(_debug)
        cout << "[Encoder::encodeMarketByPriceDataBody]" << endl;

    if(mapAction == "" || mapKey == "") {
        cerr << "[Encoder::encodeMarketByPriceDataBody] mapAction or mapKey is empty." << endl;
    }

    assert(pMap);
    MapWriteIterator mapWIt;
    mapWIt.start(*pMap);
    pMap->setKeyDataType(DataBuffer::BufferEnum);    
    pMap->setTotalCountHint(1);  // Provides a hint to the consuming side of how many map entries are to be provided. In this example there is only one service provided so the totalcounthint is one.


    //********* Begin Encode MapEntry1 **********
    MapEntry mapEntry;
    if(mapAction == "add") {
        mapEntry.setAction(MapEntry::Add);
    } else if(mapAction == "update") {
        mapEntry.setAction(MapEntry::Update);
    } else if(mapAction == "delete") {
        mapEntry.setAction(MapEntry::Delete);
    } else {
        return;
    }
    
     // Set order id as key for MapEntry (e.g. 463463B)
    RFA_String key;
    key.set(mapKey.c_str(), mapKey.length(), false);
    DataBuffer keyDataBuffer(true);
    keyDataBuffer.setFromString(key, DataBuffer::BufferEnum);
    mapEntry.setKeyData(keyDataBuffer);

    if(_debug) {
        cout << "Action: " << mapAction.c_str() << endl;
        cout << "Key   : " << key.c_str() << endl;
    }
    
    if(mapAction != "delete") {
        // encode fieldList using encodeMarketPriceDataBody()
        encodeMarketPriceDataBody(pFieldList, respType, fieldList, pDict);
        mapEntry.setData(static_cast<Data&>(*pFieldList));
    }
    mapWIt.bind(mapEntry);
    mapWIt.complete();
}

void Encoder::encodeSymbolListMsg( RespMsg* respMsg, RespMsg::RespType respType, const AttribInfo & rAttribInfo, RespStatus & rRStatus, QualityOfService* pQoS, bool bSetAttribute, bool bSolicited)
{
    assert( respMsg );

    // set MsgModelType
    respMsg->setMsgModelType( MMT_SYMBOL_LIST );    

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

void Encoder::encodeSymbolListDataBody(Map* pMap, FieldList* pFieldList, RespMsg::RespType respType, const rfa::common::RFA_Vector<rfa::common::RFA_String>& fieldList, const RDMFieldDict* pDict, const std::string& mapAction, const std::string& mapKey)
{
    if(_debug)
        cout << "[Encoder::encodeSymbolListDataBody]" << endl;

    if(mapAction == "" || mapKey == "") {
        cerr << "[Encoder::encodeSymbolListDataBody] mapAction or mapKey is empty." << endl;
    }

    assert(pMap);
    MapWriteIterator mapWIt;
    mapWIt.start(*pMap);
    pMap->setKeyDataType(DataBuffer::BufferEnum);    
    pMap->setTotalCountHint(1);  // Provides a hint to the consuming side of how many map entries are to be provided. In this example there is only one service provided so the totalcounthint is one.


    //********* Begin Encode MapEntry1 **********
    MapEntry mapEntry;
    if(mapAction == "add") {
        mapEntry.setAction(MapEntry::Add);
    } else if(mapAction == "update") {
        mapEntry.setAction(MapEntry::Update);
    } else if(mapAction == "delete") {
        mapEntry.setAction(MapEntry::Delete);
    } else {
        return;
    }
    
     // Set order id as key for MapEntry (e.g. 463463B)
    RFA_String key;
    key.set(mapKey.c_str(), mapKey.length(), false);
    DataBuffer keyDataBuffer(true);
    keyDataBuffer.setFromString(key, DataBuffer::BufferEnum);
    mapEntry.setKeyData(keyDataBuffer);

    if(_debug) {
        cout << "Action: " << mapAction.c_str() << endl;
        cout << "Key   : " << key.c_str() << endl;
    }
    
    if(mapAction != "delete") {
        // encode fieldList using encodeMarketPriceDataBody()
        encodeMarketPriceDataBody(pFieldList, respType, fieldList, pDict);
        mapEntry.setData(static_cast<Data&>(*pFieldList));
    }
    mapWIt.bind(mapEntry);
    mapWIt.complete();
}

void Encoder::encodeHistoryMsg( RespMsg* respMsg, RespMsg::RespType respType, const AttribInfo & rAttribInfo, RespStatus & rRStatus, QualityOfService* pQoS, bool bSetAttribute, bool bSolicited)
{
    assert( respMsg );

    // set MsgModelType
    respMsg->setMsgModelType( MMT_HISTORY );    

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

void Encoder::encodeHistoryDataBody(Series* pSeries, FieldList* pFieldList, RespMsg::RespType respType, const rfa::common::RFA_Vector<rfa::common::RFA_String>& fieldList, const RDMFieldDict* pDict)
{
    if(_debug)
        cout << "[Encoder::encodeHistoryDataBody]" << endl;

    assert(pSeries);
    SeriesWriteIterator seriesWIt;
    seriesWIt.start(*pSeries);
    //pSeries->setKeyDataType(DataBuffer::BufferEnum);    
    pSeries->setTotalCountHint(1);

    //********* Begin Encode SeriesEntry1 **********
    SeriesEntry seriesEntry;
    encodeMarketPriceDataBody(pFieldList, respType, fieldList, pDict);
    seriesEntry.setData(static_cast<Data&>(*pFieldList));
    seriesWIt.bind(seriesEntry);
    seriesWIt.complete();
}

void Encoder::encodeDirectoryDataBody(Data* pData, RFA_String & rSvcName, RFA_String & rVendName, int& serviceState, QualityOfService * pQoS)
{
    Map* pMap;
    assert(pData);
    pMap = static_cast<Map*>(pData);
    encodeDirectoryMap( pMap, rSvcName,rVendName, serviceState, pQoS);
}

void Encoder::loadDictionaryFromFile(RFA_String &appendix_a_path, RFA_String &enumtype_def_path )
{
    if (_bDictionaryLoaded) 
        return;

        _pDictionaryEncoder = new RDMDictionaryEncoder();
        _rdmFieldDict = new RDMFieldDict();

    RDMFileDictionaryDecoder decoder(*_rdmFieldDict);


    if( decoder.load(appendix_a_path, enumtype_def_path) )
    {
        RFA_String    version("1.1", 4, false);
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

void Encoder::encodeFieldDictionary(Series & data, RFA_String &appendix_a_path, RFA_String &enumtype_def_path) 
{
    loadDictionaryFromFile(appendix_a_path,enumtype_def_path);
    if ( _bDictionaryLoaded )
        _pDictionaryEncoder->encodeFieldDictionary(data, *_rdmFieldDict, DICTIONARY_NORMAL);
}    
    
void Encoder::encodeEnumDictionary(Series & data, RFA_String &appendix_a_path, RFA_String &enumtype_def_path) 
{
    loadDictionaryFromFile(appendix_a_path,enumtype_def_path);
    if ( _bDictionaryLoaded )
        _pDictionaryEncoder->encodeEnumDictionary(data, _rdmFieldDict->enumDict(), DICTIONARY_NORMAL);
}    

void Encoder::encodeDirectoryMap( Map* pMap, RFA_String & rSvcName, RFA_String & rVendName, int& serviceState, QualityOfService * pQoS)
{
    assert(pMap);
    MapWriteIterator mapWIt;
    mapWIt.start(*pMap);
    pMap->setKeyDataType( DataBuffer::StringAsciiEnum);    
    pMap->setTotalCountHint(1);  // Provides a hint to the consuming side of how many map entries are to be provided. In this example there is only one service provided so the totalcounthint is one.


    //********* Begin Encode MapEntry1 **********
    MapEntry mapEntry;
    mapEntry.setAction(MapEntry::Add);


     // Set ServiceName as key for MapEntry
    DataBuffer keyDataBuffer(true);
    keyDataBuffer.setFromString(rSvcName, DataBuffer::StringAsciiEnum);
    mapEntry.setKeyData(keyDataBuffer);

    FilterList filterList;
    filterList.setAssociatedMetaInfo(pMap->getMajorVersion(), pMap->getMinorVersion());
    encodeDirectoryFilterList(&filterList, rSvcName, rVendName, serviceState, pQoS);    
    mapEntry.setData(static_cast<Data&>(filterList));

    mapWIt.bind(mapEntry);
    mapWIt.complete();
}




void Encoder::encodeDirectoryFilterList( FilterList* pFilterList, RFA_String & rSvcName, RFA_String & rVendName, int& serviceState, QualityOfService * pQoS)
{
    assert(pFilterList);
    FilterListWriteIterator filterListWIt;
    filterListWIt.start(*pFilterList);  

    pFilterList->setTotalCountHint(2);    // Specify 2 because there is only two filter entries


    //Encode FilterEntry
    FilterEntry filterEntry;
    filterEntry.setFilterId(SERVICE_INFO_ID);
    filterEntry.setAction(FilterEntry::Set);        // Set this to Set since this is the full filterEntry

    //Encode ElementList for Service Info
    ElementList elementList;
    elementList.setAssociatedMetaInfo(pFilterList->getMajorVersion(), pFilterList->getMinorVersion());
    encodeDirectoryInfoElementList(&elementList, rSvcName, rVendName, pQoS);
    filterEntry.setData(static_cast<const Data&>(elementList));
    filterListWIt.bind(filterEntry);


    //Encode ElementList for Service State
    filterEntry.clear();
    elementList.clear();
    elementList.setAssociatedMetaInfo(pFilterList->getMajorVersion(), pFilterList->getMinorVersion());
    filterEntry.setFilterId(SERVICE_STATE_ID);
    filterEntry.setAction(FilterEntry::Set);   // Set this to Set since this is the full filterEntry
    encodeDirectoryStateElementList(&elementList, serviceState);
    filterEntry.setData(static_cast<const Data&>(elementList));
    filterListWIt.bind(filterEntry);
    
    filterListWIt.complete();
}

void Encoder::encodeDirectoryInfoElementList(ElementList* pElementList, RFA_String & rSvcName, RFA_String & rVendName, QualityOfService * pQoS)
{
    assert(pElementList);
    ElementListWriteIterator elemListWIt;
    elemListWIt.start(*pElementList);

    ElementEntry element;
    DataBuffer dataBuffer;     
    RFA_String nameValue;

    // Encode Name
    element.setName(rfa::rdm::ENAME_NAME);
    nameValue =rSvcName.c_str();
    dataBuffer.setFromString(nameValue,DataBuffer::StringAsciiEnum);
    element.setData( dataBuffer );
    elemListWIt.bind(element);

    // Encode Vendor
    element.clear();
    dataBuffer.clear();
    element.setName(rfa::rdm::ENAME_VENDOR);
    nameValue =rVendName.c_str();
    dataBuffer.setFromString(nameValue,DataBuffer::StringAsciiEnum);
    element.setData( dataBuffer );
    elemListWIt.bind(element);

    // Encode  isSource 
    element.clear();
    dataBuffer.clear();
    element.setName(rfa::rdm::ENAME_IS_SOURCE);
    dataBuffer.setUInt(1);        // Original Publisher  TODO: Will this be an global value?
    element.setData( dataBuffer );
    elemListWIt.bind(element);

    // Encode Capabilities
    element.clear();
    element.setName(rfa::rdm::ENAME_CAPABILITIES);
    Array array;
    encodeDirectoryArray(&array);
    element.setData(static_cast<const Data&>(array));
    elemListWIt.bind(element);

    // Encode DictionariesUsed
    element.clear();
    element.setName(rfa::rdm::ENAME_DICTIONARYS_USED);
    array.clear(); 
    encodeDirectoryArrayDictUsed(&array);
    element.setData(static_cast<const Data&>(array));
    elemListWIt.bind(element);

    // Encode Quality of Service
    if(pQoS)
    {
        element.clear();
        element.setName(rfa::rdm::ENAME_QOS);
        Array arrayqos;
        encodeDirectoryArrayQoS(&arrayqos,pQoS);
        element.setData(static_cast<const Data&>(arrayqos));
        elemListWIt.bind(element);
    }

    elemListWIt.complete();
}


void Encoder::encodeDirectoryStateElementList(ElementList* pElementList, int& serviceState)
{
    assert(pElementList);
    ElementListWriteIterator elemListWIt;
    elemListWIt.start(*pElementList);
    
    ElementEntry element;

    //Encode Element ServiceState
    element.setName(rfa::rdm::ENAME_SVC_STATE);

    int mType = serviceState ; // 1 = Up, 0 = Down
    DataBuffer dataBuffer(true);
    dataBuffer.setUInt((UInt32)mType);
    element.setData( dataBuffer );
    elemListWIt.bind(element);

    //Encode Element AcceptingRequests
    element.clear();
    element.setName(rfa::rdm::ENAME_ACCEPTING_REQS);
    mType = 1; // Accepts requests
    dataBuffer.setUInt((UInt32)mType);
    element.setData( dataBuffer );
    elemListWIt.bind(element);

    elemListWIt.complete();
}



void Encoder::encodeDirectoryArray( Array* pArray)
{
    assert(pArray);
    ArrayWriteIterator arrWIt; 
    arrWIt.start(*pArray);
                    
    // Specify Dictionary as a capability
    DataBuffer dataBuffer(true);
    ArrayEntry arrayEntry;
    int mType = MMT_DICTIONARY;
    dataBuffer.setUInt((UInt32)mType);
    arrayEntry.setData(dataBuffer);
    arrWIt.bind(arrayEntry);

    
    if ( _bGenericDomainModel )
    {
        mType = _genericDomainModel;
        arrayEntry.clear();
        dataBuffer.setUInt((UInt32)mType);
        arrayEntry.setData(dataBuffer);
        arrWIt.bind(arrayEntry);
    }

    if(_bMarketPriceDomainModel)
    {
        mType = MMT_MARKET_PRICE;
        arrayEntry.clear();
        dataBuffer.setUInt((UInt32)mType);
        arrayEntry.setData(dataBuffer);
        arrWIt.bind(arrayEntry);
    }

    if(_bMarketByOrderDomainModel)
    {
        mType = rfa::rdm::MMT_MARKET_BY_ORDER;
        arrayEntry.clear();
        dataBuffer.setUInt((UInt32)mType);
        arrayEntry.setData(dataBuffer);
        arrWIt.bind(arrayEntry);
    }

    if(_bMarketByPriceDomainModel)
    {
        mType = rfa::rdm::MMT_MARKET_BY_PRICE;
        arrayEntry.clear();
        dataBuffer.setUInt((UInt32)mType);
        arrayEntry.setData(dataBuffer);
        arrWIt.bind(arrayEntry);
    }

    if(_bSymbolListDomainModel)
    {
        mType = rfa::rdm::MMT_SYMBOL_LIST;
        arrayEntry.clear();
        dataBuffer.setUInt((UInt32)mType);
        arrayEntry.setData(dataBuffer);
        arrWIt.bind(arrayEntry);
    }

    if(_bHistoryDomainModel)
    {
        mType = rfa::rdm::MMT_HISTORY;
        arrayEntry.clear();
        dataBuffer.setUInt((UInt32)mType);
        arrayEntry.setData(dataBuffer);
        arrWIt.bind(arrayEntry);
    }

    arrWIt.complete();
}

void Encoder::encodeDirectoryArrayDictUsed( Array* pArray)
{
    assert(pArray);
    ArrayWriteIterator arrWIt; 
    arrWIt.start(*pArray);
                    
    // Specify Dictionary Files uesd
    DataBuffer dataBuffer(true);
    ArrayEntry arrayEntry;
    RFA_String fldStr("RWFFld", 0, false);
    dataBuffer.setFromString(fldStr, DataBuffer::StringAsciiEnum); 
    arrayEntry.setData(dataBuffer);
    arrWIt.bind(arrayEntry);

    RFA_String enumStr("RWFEnum", 0, false);
    arrayEntry.clear();
    dataBuffer.setFromString(enumStr, DataBuffer::StringAsciiEnum); 
    arrayEntry.setData(dataBuffer);
    arrWIt.bind(arrayEntry);
        
    arrWIt.complete();
}

void Encoder::encodeDirectoryArrayQoS( Array* pArray, QualityOfService * pQoS)
{
    assert(pArray);
    ArrayWriteIterator arrWIt; 
    arrWIt.start(*pArray);
                    
    DataBuffer dataBuffer(true);
    ArrayEntry arrayEntry;
    QualityOfServiceInfo QoSInfo;  
    QoSInfo.setQualityOfService(*pQoS);
    // TODO: Does instance count need to be set?     QoSInfo.setQualityOfServiceInstCount(?);

    dataBuffer.setQualityOfServiceInfo(QoSInfo);
    arrayEntry.setData(dataBuffer);
    arrWIt.bind(arrayEntry);
        
    arrWIt.complete();
}

void Encoder::setDebugMode(const bool &debug) {
    _debug = debug;
}

void Encoder::setDomainModelType(const UInt8 &mType) {
    switch(mType) {
        case rfa::rdm::MMT_MARKET_PRICE:
            _bMarketPriceDomainModel = true;
            break;
        case rfa::rdm::MMT_MARKET_BY_ORDER:
            _bMarketByOrderDomainModel = true;
            break;
        case rfa::rdm::MMT_MARKET_BY_PRICE:
            _bMarketByPriceDomainModel = true;
            break;
        case rfa::rdm::MMT_SYMBOL_LIST:
            _bSymbolListDomainModel = true;
            break;
        case rfa::rdm::MMT_HISTORY:
            _bHistoryDomainModel = true;
            break;
    } 
}

bool Encoder::hasDomainModelType(const UInt8 &mType) {
    switch(mType) {
        case rfa::rdm::MMT_MARKET_PRICE:
            return _bMarketPriceDomainModel;
            break;
        case rfa::rdm::MMT_MARKET_BY_ORDER:
            return _bMarketByOrderDomainModel;
            break;
        case rfa::rdm::MMT_MARKET_BY_PRICE:
            return _bMarketByPriceDomainModel;
            break;
        case rfa::rdm::MMT_SYMBOL_LIST:
            return _bSymbolListDomainModel;
            break;
        case rfa::rdm::MMT_HISTORY:
            return _bHistoryDomainModel;
            break;
        default:
            return false;
    } 
}

void Encoder::clearAllDomainModelTypes() {
    _bMarketPriceDomainModel = false;
    _bMarketByOrderDomainModel = false;
    _bMarketByPriceDomainModel = false;
    _bSymbolListDomainModel = false;
    _bHistoryDomainModel = false;
}

bool Encoder::reencodeAttribs(const Data& reqAttrib, Data& respAttrib, UInt8 supportFlags)
{
	bool returnValue = false;
	ElementListReadIterator rIter;
	ElementListWriteIterator wIter;
	ElementEntry element;

	ElementList& elementList = static_cast<ElementList&>(respAttrib);
	wIter.start(elementList);
	if(!reqAttrib.isBlank()) {
		switch(reqAttrib.getDataType())
		{
			case ElementListEnum:
			{
				const ElementList& inputEL = static_cast<const ElementList&>(reqAttrib);
				rIter.start(inputEL);
				while(!rIter.off())
				{
					element.clear();
					ElementEntry& elem = const_cast<ElementEntry&>(rIter.value());
					if(elem.getName() == ENAME_DOWNLOAD_CON_CONFIG)
						returnValue = true;
					element.setName(elem.getName());
					element.setData(elem.getData());
					wIter.bind(elem);
					rIter.forth();
				}
				break;
			}	
			default:
				break;
		}
	}
	if(supportFlags) {
		bool supportedFlag = false;
		if(supportFlags & SUPPORT_POST_FLAG) {
			addAttrib(ENAME_SUPPORT_POST, 1, wIter, element);
			supportedFlag = true;
		}
		if(supportFlags & SUPPORT_VIEW_FLAG) {
			addAttrib(ENAME_SUPPORT_VIEW, 1, wIter, element);
			supportedFlag = true;
		}
		if(supportFlags & SUPPORT_STANDBY_FLAG) {
			addAttrib(ENAME_SUPPORT_STANDBY, 1, wIter, element);
			supportedFlag = true;
		}
	}
	wIter.complete();
	return returnValue;
}

void Encoder::addAttrib(const RFA_String& name, UInt value, ElementListWriteIterator& wi, ElementEntry& element) {
	DataBuffer elementData;
	element.clear();
	element.setName(name);
	elementData.setUInt(value);
	element.setData(elementData);
	wi.bind(element);
}
