/////////////////////////////////////////////////////////////////////////////////
///	Out.h
/// This decendants of the class are utility plugins passed into the Decoder 
/// class and used to control the output of the Data being decoded.
///
/// Ex. StanardOut prints Field and Element names and value to std::out
///     XmlFileOut generates xml display of the Data being decoded
///			and outputs to a file
/////////////////////////////////////////////////////////////////////////////////

#ifndef __out_h
#define __out_h

// RFA Common and Data package headers
#include "Common/Data.h"
#include "Data/DataBuffer.h"

#include "Data/Array.h"
#include "Data/ArrayReadIterator.h"

#include "Data/Vector.h"
#include "Data/VectorEntry.h"
#include "Data/VectorReadIterator.h"

#include "Data/FilterEntry.h"
#include "Data/FilterList.h"
#include "Data/FilterListReadIterator.h"

#include "Data/ElementEntry.h"
#include "Data/ElementList.h"
#include "Data/ElementListReadIterator.h"

#include "Data/FieldEntry.h"
#include "Data/FieldList.h"
#include "Data/FieldListReadIterator.h"

#include "Data/Series.h"
#include "Data/SeriesReadIterator.h"

#include "Data/Map.h"
#include "Data/MapReadIterator.h"

#include "Data/ElementEntryDef.h"
#include "Data/ElementListDef.h"
#include "Data/ElementListDefReadIterator.h"

#include "Data/FieldEntryDef.h"
#include "Data/FieldListDef.h"
#include "Data/FieldListDefReadIterator.h"

#include "Data/VectorEntry.h"
#include "Data/Vector.h"
#include "Data/VectorReadIterator.h"

#include "Data/Array.h"
#include "Data/ArrayEntry.h"
#include "Data/ArrayReadIterator.h"

#include "Common/Msg.h"
#include "Message/ReqMsg.h"
#include "Message/RespMsg.h"
#include "Message/GenericMsg.h"
#include "Message/PostMsg.h"
#include "Message/AckMsg.h"


using namespace rfa::common;
using namespace rfa::data;
using namespace rfa::message;

class Out
{
	public:
		/// Default Constructor
		Out() {};

		/// Destructor
		virtual ~Out() {};

		/// write DataBuffer
		virtual void out(const DataBuffer& dataBuffer)=0;

		/// write Vector
		virtual void outBegin(const Vector& input)=0;
		virtual void outEnd(const Vector& input)=0;

		/// write VectorEntry
		virtual void outBegin(const VectorEntry& input)=0;
		virtual void outEnd(const VectorEntry& input)=0;

		/// write FilterList
		virtual void outBegin(const FilterList& input)=0;
		virtual void outEnd(const FilterList& input)=0;

		/// write FilterEntry
		virtual void outBegin(const FilterEntry& input)=0;
		virtual void outEnd(const FilterEntry& input)=0;

		/// write FieldList
		virtual void outBegin(const FieldList& fieldList)=0;
		virtual void outEnd(const FieldList& input)=0;
		
		/// write FieldEntry
		//virtual void outBegin(const FieldEntry& input)=0;
		//virtual void outEnd(const FieldEntry& input)=0;

		/// write FieldEntry
		virtual void outBegin(const FieldEntry& input, const char* fieldName)=0;
		virtual void outEnd(const FieldEntry& input, const char* fieldName)=0;

		/// write ElementList
		virtual void outBegin(const ElementList& input)=0;
		virtual void outEnd(const ElementList& input)=0;

		/// write ElementEntry
		virtual void outBegin(const ElementEntry& input)=0;
		virtual void outEnd(const ElementEntry& input)=0;

		/// write Series
		virtual void outBegin(const Series& input)=0;
		virtual void outEnd(const Series& input)=0;
		virtual void outBegin(const SeriesEntry& input)=0;
		virtual void outEnd(const SeriesEntry& input)=0;

		//write ElementListDef
		virtual void outBegin(const ElementListDef& input)=0;
		virtual void outEnd(const ElementListDef& input)=0;

		//write ElementEntryDef
		virtual void out(const ElementEntryDef& input)=0;

		//write FieldListDef
		virtual void outBegin(const FieldListDef& input)=0;
		virtual void outEnd(const FieldListDef& input)=0;

		//write FieldEntryDef
		virtual void out(const FieldEntryDef& input)=0;

		/// write Map
		virtual void outBegin(const Map& input)=0;
		virtual void outEnd(const Map& input)=0;

		/// write MapEntry
		virtual void outBegin(const MapEntry& input)=0;
		virtual void outEnd(const MapEntry& input)=0;
		virtual void outKeyBegin(const Data& input)=0;
		virtual void outKeyEnd(const Data& input)=0;

		/// write Array
		virtual void outBegin(const Array& input)=0;
		virtual void outEnd(const Array& input)=0;

		// write Msg
		virtual void outBegin(const Msg& input)=0;
		virtual void outEnd(const Msg& input)=0;
		virtual void outAttrib(const AttribInfo& attribInfo)=0;
		virtual void outBegin(const ReqMsg& input)=0;
		virtual void outEnd(const ReqMsg& input)=0;
		virtual void outBegin(const RespMsg& input)=0;
		virtual void outEnd(const RespMsg& input)=0;
		virtual void outBegin(const GenericMsg& input)=0;
		virtual void outEnd(const GenericMsg& input)=0;
		virtual void outBegin(const PostMsg& input)=0;
		virtual void outEnd(const PostMsg& input)=0;
		virtual void outBegin(const AckMsg& input)=0;
		virtual void outEnd(const AckMsg& input)=0;

		/// write ArrayEntry
		virtual void outArrayEntryBegin(const ArrayEntry& input)=0;
		virtual void outArrayEntryEnd(const ArrayEntry& input)=0;

		//dataTypes
		virtual void out(const Time& input)=0;
		virtual void out(const Date& input)=0;
		virtual void out(const DateTime& input)=0;
		virtual void out(const QualityOfServiceInfo& input)=0;
		virtual void out(const RespStatus& input)=0;
		virtual void out(UInt16 input)=0;
		virtual void out(Float input)=0;
		virtual void out(Double input)=0;
		virtual void out(const rfa::common::RFA_String & input)=0;
		virtual void out(const char *input, ...)=0;

		virtual void writeDataBodyBegin()=0;
		virtual void writeDataBodyEnd()=0;
private:
		// Don't implement
		Out(const Out&);
		Out& operator=(const Out&);
};

#endif  //__out_h
