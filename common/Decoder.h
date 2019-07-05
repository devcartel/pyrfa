//
//|---------------------------------------------------------------
//|				Decoder											--
//|---------------------------------------------------------------
//

#ifndef __Decoder_h
#define __Decoder_h

#include <assert.h>
#ifdef _ASSERTE
#undef _ASSERTE
#endif

#define _ASSERTE	assert

#include "Out.h"
#include "RDMDictionaryDecoder.h"

using namespace rfa::message;

class Decoder
{
	public:
		/// Constructor
		Decoder(Out* pOut, const char* a_path, const char* e_path);
		Decoder(Out* pOut);
		/// Destructor
		virtual ~Decoder();

		//data
		void decodeVector(const Vector& vector);
		void decodeFilterList(const FilterList& filterList);
		void decodeElementList(const ElementList& elementList);
		void decodeFieldList(const FieldList& fieldList);
		void decodeSeries(const Series& series);
		void decodeMap(const Map& map);
		void decodeArray(const Array& array);

		void decodeMsg(const Msg& msg);
		void decodeReqMsg(const ReqMsg& msg);
		void decodeRespMsg(const RespMsg& msg);
		void decodeGenericMsg(const GenericMsg& msg);
		void decodePostMsg(const PostMsg& msg);
		void decodeAckMsg(const AckMsg& msg);

		void decodeData(const Data& data);

		void setRawBuffer(const Buffer& pBuffer, Data& data);

		void decodeDictionaryNames(const Array& input);
		void setRWFAppendixAComplete();
		void setRWFEnumComplete();
		bool dictionaryComplete() { return _enumComplete && _appendixAComplete; };
		void loadDictionaryFromFile( const RFA_String& appendix_a_path, const RFA_String& enumtype_def_path );

		void decodeAppendixA(const Data& data);
		void decodeEnum(const Data& data);
		void associate();

	private:
		//entry
		void decodeFilterEntry(const FilterEntry& input);
		void decodeElementEntry(const ElementEntry& input);
		void decodeFieldEntry(const FieldEntry& input);
		void decodeMapEntry(const MapEntry& input);
		void decodeKey(const Data& data);
		void decodeVectorEntry(const VectorEntry& input);
		void decodeArrayEntry(const ArrayEntry& data);
		void decodeSeriesEntry(const SeriesEntry& input);

		
		//def
		void decodeDataDef(const Data& input);

		void decodeElementListDef(const ElementListDef& elementListDef);
		void decodeElementEntryDef(const ElementEntryDef& def);
		void decodeFieldListDef(const FieldListDef& fieldListDef);
		void decodeFieldEntryDef(const FieldEntryDef& DataDef);

		//buffers
		void decodeDataBuffer(const DataBuffer& dataBuffer);
		void encodeIndents();
	private:
		// Don't implement
		Decoder(const Decoder&);
		Decoder& operator=(const Decoder&);

	private:
		// output data
		Out* _out;
		int _indents;

		// dictionary
		const RDMEnumDef*	_currentEnumDef;
		RDMEnumDict*		_rdmEnumDict;
		// Use a class member var instead of a pointer because Purify sees
		// a potential memory leak in the multiple plaeces where it might be
		// new'd vs. the one where it is deleted.
		RDMFieldDict		_rdmFieldDict;

		bool				_bDictionaryLoaded;
		RDMFileDictionaryDecoder _dictionaryDecoder;

		const char*			_appendix_a_path;
		bool				_appendixAComplete;

		const char*			_enumType_def_path;
		bool				_enumComplete;

		// definition
		DataDef*			_def;
		UInt16				_defId;
};

#endif  //__rfa_Common_Decoder_h
