#ifndef __RDM_GenericOMMParser_H__
#define __RDM_GenericOMMParser_H__

/**
	\class GenericOMMParser GenericOMMParser.h 
	\brief
	Handles and parses market data items of all domain types, either RDM or
	user defined. It parses all relevent message and data information and
	should be used either for diagnostic purposes or for handling domain models
	that do not currently have a specific implementation written for them
	(like all level two domains have in this application)
*/

// RFA Includes
#include "Common/DataDef.h"
#include "Data/DataBuffer.h"
#include "Data/ElementList.h"
#include "RDM/RDMFieldDictionary.h"
#include "RDM/RDMFidDef.h"

namespace rfa {

	namespace data {
		class FieldList;
		class FieldEntry;
		class ElementList;
		class ElementEntry;
		class Vector;
		class VectorEntry;
		class Series;
		class SeriesEntry;
		class Map;
		class MapEntry;
		class FilterList;
		class FilterEntry;
		class Array;
		class ArrayEntry;
		class DataBuffer;
	}
	
	namespace message {
		class RespMsg;
		class ReqMsg;
		class AttribInfo;
		class Manifest;
	}
	
	namespace common {
		class Msg;
		class Data;
		class QualityOfService;
		class QualityOfServiceRequest;
	}
}

class GenericOMMParser
{
public:
	GenericOMMParser( rfa::rdm::RDMFieldDictionary & dictionary );
	~GenericOMMParser();

	// text is an instance of an RFA_String into which
	// the message or data will be parsed
	// passing a unique instance of RFA_String will avoid
	// thread problems (overwrites and crashes)
	void parse( rfa::common::RFA_String& text, const rfa::common::Msg& msg );
	void parse( rfa::common::RFA_String& text, const rfa::common::Data& data );
	static void displayHexBuffer( rfa::common::RFA_String& text, const rfa::common::Buffer& buffer );

private:
	void parseRespMsg( rfa::common::RFA_String& text, const rfa::message::RespMsg& respMsg );
	void parseReqMsg( rfa::common::RFA_String& text, const rfa::message::ReqMsg& reqMsg );
	void parseStatusRespMsg( rfa::common::RFA_String& text, const rfa::message::RespMsg& respMsg );
	void parseRefreshRespMsg( rfa::common::RFA_String& text, const rfa::message::RespMsg& respMsg );
	void parseUpdateRespMsg( rfa::common::RFA_String& text, const rfa::message::RespMsg& respMsg );
	
	void parseServiceAndItemName( rfa::common::RFA_String& text, const rfa::message::RespMsg& respMsg );
	void parseMsgInfo( rfa::common::RFA_String& text, const rfa::message::RespMsg& respMsg );
	void parseMsgInfo( rfa::common::RFA_String& text, const rfa::message::ReqMsg& reqMsg );
	void parseMsgAttribInfo( rfa::common::RFA_String& text, const rfa::message::AttribInfo& info );
	void parseMsgManifest( rfa::common::RFA_String& text, const rfa::message::Manifest& info );
	void parseMsgRespStatus( rfa::common::RFA_String& text, const rfa::common::RespStatus& status );
	void parseMsgQOSInfo( rfa::common::RFA_String& text, const rfa::common::QualityOfService& info );
	void parseMsgQOSReq( rfa::common::RFA_String& text, const rfa::common::QualityOfServiceRequest& qosr );


	/// Currently were are only showing how to 'generically' parse Map, FieldList an DataBuffer
	/// since the supported RDM Msg Domain types only use these. Generic decoding for 
	/// other data types will be implemented in later releases
	/// For an examples of how to parse Series, Array, ElementList and FilterList see
	/// the DirectoryManager or DictioanryManager(and RDMDicts)
	void parseData( rfa::common::RFA_String& text, const rfa::common::Data& data);
	void parseArray( rfa::common::RFA_String& text, const rfa::data::Array& input);
	void parseArrayEntry( rfa::common::RFA_String& text, const rfa::data::ArrayEntry& input);
	void parseFieldList( rfa::common::RFA_String& text, const rfa::data::FieldList& input);
	void parseFieldEntry( rfa::common::RFA_String& text, const rfa::data::FieldEntry& input);
	void parseElementList( rfa::common::RFA_String& text, const rfa::data::ElementList& input);
	void parseElementEntry( rfa::common::RFA_String& text, const rfa::data::ElementEntry& input);
	void parseFilterList( rfa::common::RFA_String& text, const rfa::data::FilterList& input);
	void parseFilterEntry( rfa::common::RFA_String& text, const rfa::data::FilterEntry& input);
	void parseSeries( rfa::common::RFA_String& text, const rfa::data::Series& input);
	void parseSeriesEntry( rfa::common::RFA_String& text, const rfa::data::SeriesEntry& input);
	void parseVector( rfa::common::RFA_String& text, const rfa::data::Vector& input);
	void parseVectorEntry( rfa::common::RFA_String& text, const rfa::data::VectorEntry& input);
	void parseMap( rfa::common::RFA_String& text, const rfa::data::Map& input);
	void parseMapEntry( rfa::common::RFA_String& text, const rfa::data::MapEntry& input);
	void parseDataBuffer( rfa::common::RFA_String& text, const rfa::data::DataBuffer& dataBuffer);

	void writeIndents( rfa::common::RFA_String& text );

private:
	rfa::rdm::RDMFieldDictionary&	m_dictionary;
	int							m_indents;

private:
	GenericOMMParser();
	GenericOMMParser( const GenericOMMParser& );
	GenericOMMParser& operator= ( const GenericOMMParser& );

};

#endif
