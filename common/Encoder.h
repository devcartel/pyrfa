//
//|---------------------------------------------------------------
//|                Copyright (C) 2007 Reuters,                  --
//|         1111 W. 22nd Street, Suite 500, Oak Brook, IL. 60523          --
//| All rights reserved. Duplication or distribution prohibited --
//|---------------------------------------------------------------
//
// Encoder.h: Helper class to encode messages and data body

#ifndef __Encoder_h
#define __Encoder_h

#include <assert.h>

#ifdef _ASSERTE
#undef _ASSERTE
#endif

#define _ASSERTE    assert

#include "Data/DataBuffer.h"
#include "Data/FilterEntry.h"
#include "Data/FilterList.h"
#include "Data/FilterListWriteIterator.h"
#include "Data/ElementEntry.h"
#include "Data/ElementList.h"
#include "Data/ElementListWriteIterator.h"
#include "Data/FieldEntry.h"
#include "Data/FieldList.h"
#include "Data/FieldListWriteIterator.h"
#include "Data/Map.h"
#include "Data/MapWriteIterator.h"
#include "Data/SeriesEntry.h"
#include "Data/Series.h"
#include "Data/SeriesWriteIterator.h"
#include "Data/Array.h"
#include "Data/ArrayWriteIterator.h"
#include "Data/ArrayEntry.h"
#include "Message/RespMsg.h"
#include "Message/ReqMsg.h"
#include "Message/Manifest.h"
#include "Message/AttribInfo.h"
#include "RDM/RDM.h"
#include "Common/RFA_Vector.h"
#include "RDMDictionaryEncoder.h" 
#include "RDMDictionaryDecoder.h" 

using namespace std;
using namespace rfa::common;
using namespace rfa::message;
using namespace rfa::data;
using namespace rfa::rdm;

class Encoder
{
    public:
        /// Constructor
        Encoder();

        /// Destructor
        virtual ~Encoder();

        // Msg Encoders
    	void encodeDictionaryMsg(rfa::message::RespMsg* respMsg, const rfa::message::AttribInfo& rAttribInfo, rfa::common::RespStatus& rRStatus, rfa::common::UInt8 indicationMask);
        void encodeLoginMsg(RespMsg* respMsg, const AttribInfo & rAttribInfo, RespStatus & rRStatus);
        void encodeDirectoryMsg(RespMsg* respMsg, const AttribInfo & rAttribInfo, RespStatus & rRStatus, bool bSolicited = true);
        void encodeMarketPriceMsg(RespMsg* respMsg, RespMsg::RespType respType, const AttribInfo & rAttribInfo, RespStatus & rRStatus, QualityOfService* pQoS=0, bool bSetAttribute = true, bool bSolicited = true);
        void encodeMarketByOrderMsg(RespMsg* respMsg, RespMsg::RespType respType, const AttribInfo & rAttribInfo, RespStatus & rRStatus, QualityOfService* pQoS=0, bool bSetAttribute = true, bool bSolicited = true);
        void encodeMarketByPriceMsg(RespMsg* respMsg, RespMsg::RespType respType, const AttribInfo & rAttribInfo, RespStatus & rRStatus, QualityOfService* pQoS=0, bool bSetAttribute = true, bool bSolicited = true);
        void encodeSymbolListMsg(RespMsg* respMsg, RespMsg::RespType respType, const AttribInfo & rAttribInfo, RespStatus & rRStatus, QualityOfService* pQoS=0, bool bSetAttribute = true, bool bSolicited = true);
        void encodeHistoryMsg(RespMsg* respMsg, RespMsg::RespType respType, const AttribInfo & rAttribInfo, RespStatus & rRStatus, QualityOfService* pQoS=0, bool bSetAttribute = true, bool bSolicited = true);

        // Data Body Encoders
        void encodeDataBody(rfa::common::Data* pData, UInt16 msgType );
        void encodeDirectoryDataBody(rfa::common::Data* pData, RFA_String & rSvcName, RFA_String & rVendName, int& serviceState, QualityOfService * pQoS=0);
        void encodeMarketPriceDataBody(FieldList* pFieldList, RespMsg::RespType respType, const rfa::common::RFA_Vector<rfa::common::RFA_String>& fieldList, const RDMFieldDict* pDict);
        void encodeMarketByOrderDataBody(Map* pMap, FieldList* pFieldList, RespMsg::RespType respType, const rfa::common::RFA_Vector<rfa::common::RFA_String>& fieldList, const RDMFieldDict* pDict, const std::string& mapAction, const std::string& mapKey);
        void encodeMarketByPriceDataBody(Map* pMap, FieldList* pFieldList, RespMsg::RespType respType, const rfa::common::RFA_Vector<rfa::common::RFA_String>& fieldList, const RDMFieldDict* pDict, const std::string& mapAction, const std::string& mapKey);
        void encodeSymbolListDataBody(Map* pMap, FieldList* pFieldList, RespMsg::RespType respType, const rfa::common::RFA_Vector<rfa::common::RFA_String>& fieldList, const RDMFieldDict* pDict, const std::string& mapAction, const std::string& mapKey);
        void encodeHistoryDataBody(Series* pSeries, FieldList* pFieldList, RespMsg::RespType respType, const rfa::common::RFA_Vector<rfa::common::RFA_String>& fieldList, const RDMFieldDict* pDict);
        
        void loadDictionaryFromFile(RFA_String &appendix_a_path, RFA_String &enumtype_def_path);
        void encodeFieldDictionary(Series & data, RFA_String &appendix_a_path, RFA_String &enumtype_def_path);
        void encodeEnumDictionary(Series & data, RFA_String &appendix_a_path, RFA_String &enumtype_def_path);
        void setGenericModelInCapabilites(UInt32 genDomain) { _genericDomainModel = genDomain ; _bGenericDomainModel= true; }
        void setMarketPriceInCapabilites(bool val = true) {  _bMarketPriceDomainModel = val; }
        void setDebugMode(const bool &debug);
        void setDomainModelType(const UInt8 &mType);
        bool hasDomainModelType(const UInt8 &mType);
        void clearAllDomainModelTypes();
    
        bool reencodeAttribs(const rfa::common::Data& reqAttrib, rfa::common::Data& respAttrib, rfa::common::UInt8 supportFlags = 0);
        void addAttrib(const rfa::common::RFA_String& name, rfa::common::UInt value, rfa::data::ElementListWriteIterator& wi, rfa::data::ElementEntry& element);
    
        static const rfa::common::UInt8 SUPPORT_VIEW_FLAG = 0x01;
        static const rfa::common::UInt8 SUPPORT_POST_FLAG = 0x02;
        static const rfa::common::UInt8 SUPPORT_STANDBY_FLAG = 0x04;
    
    private:
        void encodeDirectoryMap( Map* pMap, RFA_String & rSvcName, RFA_String & rVendName, int& serviceState, QualityOfService * pQoS=0);
        void encodeDirectoryFilterList( rfa::data::FilterList* pFilterList, RFA_String & rSvcName, RFA_String & rVendName, int& serviceState, QualityOfService * pQoS);
        void encodeDirectoryInfoElementList(ElementList* pElementList, RFA_String & rSvcName, RFA_String & rVendName, QualityOfService * pQoS=0);
        void encodeDirectoryStateElementList(ElementList* pElementList, int& serviceState);
        void encodeDirectoryArray(Array* pArray);
        void encodeDirectoryArrayDictUsed( Array* pArray);
        void encodeDirectoryArrayQoS(Array* pArray, QualityOfService * pQoS=0);

        RDMFieldDict*           _rdmFieldDict;
        RDMEnumDict*            _rdmEnumDict;  
        RDMDictionaryEncoder*   _pDictionaryEncoder;
        bool                    _bDictionaryLoaded; 
        bool                    _bGenericDomainModel;
        UInt32                  _genericDomainModel;
        bool                    _bMarketPriceDomainModel;
        bool                    _bSymbolListDomainModel;
        bool                    _bMarketByOrderDomainModel;
        bool                    _bMarketByPriceDomainModel;
        bool                    _bHistoryDomainModel;
        bool                    _debug;
    private:
        // Don't implement
        Encoder(const Encoder&);
        Encoder& operator=(const Encoder&);
};

#endif  //__rfa_Common_Encoder_h
