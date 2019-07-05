#ifndef __SingleIterEncoder_h
#define __SingleIterEncoder_h

#include <assert.h>

#ifdef _ASSERTE
#undef _ASSERTE
#endif

#define _ASSERTE	assert

#include "Data/DataBuffer.h"
#include "Data/FilterEntry.h"
#include "Data/FilterList.h"
#include "Data/ElementEntry.h"
#include "Data/ElementList.h"
#include "Data/FieldEntry.h"
#include "Data/FieldList.h"
#include "Data/Map.h"
#include "Data/MapEntry.h"
#include "Data/Array.h"
#include "Data/ArrayEntry.h"
#include "Message/RespMsg.h"
#include "Message/ReqMsg.h"
#include "Message/Manifest.h"
#include "Message/AttribInfo.h"
#include "RDM/RDM.h"
#include "Data/Series.h" 
#include "Common/RFA_Vector.h"
#include "RDMDictionaryEncoder.h" 
#include "RDMDictionaryDecoder.h" 
#include "Data/SingleWriteIterator.h"
#include "Data/SingleReadIterator.h"

using namespace std;
using namespace rfa::common;
using namespace rfa::message;
using namespace rfa::data;
using namespace rfa::rdm;

class SingleIterEncoder
{
	public:
        /// Constructor
		SingleIterEncoder();

        /// Destructor
        virtual ~SingleIterEncoder();

		// Msg SingleIterEncoders
		void encodeDictionaryMsg(RespMsg* respMsg, const AttribInfo & rAttribInfo, RespStatus & rRStatus);
		void encodeLoginMsg(RespMsg* respMsg, const AttribInfo & rAttribInfo, RespStatus & rRStatus);
		void encodeDirectoryMsg(RespMsg* respMsg, const AttribInfo & rAttribInfo, RespStatus & rRStatus, bool bSolicited = true);
		void encodeMarketPriceMsg(RespMsg* respMsg, RespMsg::RespType respType, const AttribInfo & rAttribInfo, RespStatus & rRStatus, QualityOfService* pQoS=0, bool bSetAttribute = true, bool bSolicited = true);

		// Data Body SingleIterEncoders
		void encodeDataBody(rfa::common::Data* pData, UInt16 msgType );
		void encodeMarketPriceDataBody(rfa::common::Data* pData, rfa::common::RFA_Vector<int>* fidArr = 0 );
		void encodeDirectoryDataBody(rfa::common::Data* pData, RFA_String & rSvcName, RFA_String & rVendName, QualityOfService * pQoS=0);

		void loadDictionaryFromFile(RFA_String &appendix_a_path, RFA_String &enumtype_def_path);

		void encodeFieldDictionary(Series & data, RFA_String &appendix_a_path, RFA_String &enumtype_def_path);
		void encodeEnumDictionary(Series & data, RFA_String &appendix_a_path, RFA_String &enumtype_def_path);
		void setGenericModelInCapabilites(UInt32 genDomain) { _genericDomainModel = genDomain ; _bGenericDomainModel= true; }
		void setMarketPriceInCapabilites(bool val = true) {  _bMarketPriceDomainModel = val; }
		void reencodeAttributeInfoAttrib(const Data& reqAttrib, Data& respAttrib);
	
	private:
		void encodeDirectoryMap( Map* pMap, RFA_String & rSvcName, RFA_String & rVendName, QualityOfService * pQoS=0);
		void encodeDirectoryFilterList( rfa::data::FilterList* pFilterList, RFA_String & rSvcName, RFA_String & rVendName, QualityOfService * pQoS);
		void encodeDirectoryInfoElementList(ElementList* pElementList, RFA_String & rSvcName, RFA_String & rVendName, QualityOfService * pQoS=0);
		void encodeDirectoryStateElementList(ElementList* pElementList);
		void encodeDirectoryArray(Array* pArray);
		void encodeDirectoryArrayDictUsed( Array* pArray);
		void encodeDirectoryArrayQoS(Array* pArray, QualityOfService * pQoS=0);

		void encodeDirectoryArrayEx( Array* pArray );
		void encodeMarketPriceFieldList(FieldList* pFieldList);
		void encodeMarketPriceFieldListEx(FieldList* pFieldList, rfa::common::RFA_Vector<int>&  fidArr );
		
		RDMFieldDict * _rdmFieldDict;
		RDMEnumDict * _rdmEnumDict;  
		RDMDictionaryEncoder * _pDictionaryEncoder;
		int _trdPrice;
		bool _bDictionaryLoaded; 
		bool _bGenericDomainModel;
		UInt32 _genericDomainModel;
		bool _bMarketPriceDomainModel;
		SingleWriteIterator _marketPriceSWIter;
		SingleWriteIterator _dirSWIter;
	private:
		// Don't implement
		SingleIterEncoder(const SingleIterEncoder&);
		SingleIterEncoder& operator=(const SingleIterEncoder&);
};

#endif  //__SingleIterEncoder_h
