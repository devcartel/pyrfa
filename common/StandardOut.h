/////////////////////////////////////////////////////////////////////////////////
///	StandardOut.h
/// This class is a utility plugin passed into the Decoder class
///  and is used to control the output of the Data being decoded.
/////////////////////////////////////////////////////////////////////////////////

#ifndef __StandardOut_h
#define __StandardOut_h

#include "Out.h"

#include <assert.h>
#ifdef _ASSERTE
#undef _ASSERTE
#endif
#define _ASSERTE assert


class StandardOut : public Out
{
	public:
		/// Constructor
		StandardOut();

		/// Destructor
		virtual ~StandardOut();
	
		/// write DataBuffer
		void out(const DataBuffer& dataBuffer);

		/// write Vector
		void outBegin(const Vector& input);
		void outEnd(const Vector& input);

		/// write VectorEntry
		void outBegin(const VectorEntry& input);
		void outEnd(const VectorEntry& input);

		/// write FilterList
		void outBegin(const FilterList& input);
		void outEnd(const FilterList& input);

		/// write FilterEntry
		void outBegin(const FilterEntry& input);
		void outEnd(const FilterEntry& input);

		/// write FieldList
		void outBegin(const FieldList& fieldList);
		void outEnd(const FieldList& input);

		/// write FieldEntry
		void outBegin(const FieldEntry& input, const char* fieldName);
		void outEnd(const FieldEntry& input, const char* fieldName);

		/// write ElementList
		void outBegin(const ElementList& input);
		void outEnd(const ElementList& input);

		/// write ElementEntry
		void outBegin(const ElementEntry& input);
		void outEnd(const ElementEntry& input);

		/// write Series
		void outBegin(const Series& input);
		void outEnd(const Series& input);
		void outBegin(const SeriesEntry& input);
		void outEnd(const SeriesEntry& input);

		//write ElementListDef
		void outBegin(const ElementListDef& input);
		void outEnd(const ElementListDef& input);

		//write ElementEntryDef
		void out(const ElementEntryDef& input);

		//write FieldListDef
		void outBegin(const FieldListDef& input);
		void outEnd(const FieldListDef& input);

		//write FieldEntryDef
		void out(const FieldEntryDef& input);

		/// write Map
		void outBegin(const Map& input);
		void outEnd(const Map& input);

		/// write MapEntry
		void outBegin(const MapEntry& input);
		void outEnd(const MapEntry& input);
		void outKeyBegin(const Data& input);
		void outKeyEnd(const Data& input);

		/// write Array
		void outBegin(const Array& input);
		void outEnd(const Array& input);
		void outArrayEntryBegin(const ArrayEntry& input);
		void outArrayEntryEnd(const ArrayEntry& input);

		// write Msg
		void outBegin(const Msg& input);
		void outEnd(const Msg& input);
		void outAttrib(const AttribInfo& attribInfo);
		void outBegin(const ReqMsg& input);
		void outEnd(const ReqMsg& input);
		void outBegin(const RespMsg& input);
		void outEnd(const RespMsg& input);
		void outBegin(const GenericMsg& input);
		void outEnd(const GenericMsg& input);
		void outBegin(const PostMsg& input);
		void outEnd(const PostMsg& input);
		void outBegin(const AckMsg& input);
		void outEnd(const AckMsg& input);

		//dataTypes
		void out(const Time& input);
		void out(const Date& input);
		void out(const DateTime& input);
		void out(const QualityOfServiceInfo& input);
		void out(const RespStatus& input);
		void out(UInt16 input);
		void out(Float input);
		void out(Double input);
		void out(const std::string & input);
		void out(const char * input, ...);

		//Perform pre- or post- decode instructions
		void writeDataBodyBegin();
		void writeDataBodyEnd();

	private:
		StandardOut(const StandardOut&);
		StandardOut& operator=(const StandardOut&);

		//utilities
		void encodeIndents();

		// members
		int m_indents;
};

#endif  //__StandardOut_h
