#include "GenericOMMParser.h"

#include "OMMStrings.h"
#include "Message/RespMsg.h"
#include "Message/ReqMsg.h"
#include "Data/ElementListReadIterator.h"
#include "Data/ArrayReadIterator.h"
#include "Data/FilterListReadIterator.h"
#include "Data/VectorReadIterator.h"
#include "Data/SeriesReadIterator.h"
#include "Common/Data.h"
#include "Data/FieldList.h"
#include "Data/FieldEntry.h"
#include "Data/FieldListReadIterator.h"

#include "Data/ElementList.h"
#include "Data/ElementEntry.h"

#include "Data/Array.h"
#include "Data/ArrayEntry.h"

#include "Data/FilterList.h"
#include "Data/FilterEntry.h"

#include "Data/Map.h"
#include "Data/MapEntry.h"
#include "Data/MapReadIterator.h"

#include "Data/Series.h"
#include "Data/SeriesEntry.h"

#include "Data/Vector.h"
#include "Data/VectorEntry.h"

using namespace rfa::common;
using namespace rfa::message;
using namespace rfa::rdm;
using namespace rfa::data;

GenericOMMParser::GenericOMMParser( rfa::rdm::RDMFieldDictionary & dictionary ) :
 m_dictionary( dictionary ),
 m_indents(0)
{
}

GenericOMMParser::~GenericOMMParser()
{
}

void GenericOMMParser::parse( RFA_String& text, const Msg& msg )
{
	if ( msg.getMsgType() == rfa::message::RespMsgEnum )
	{
		parseRespMsg( text, static_cast<const RespMsg&>(msg));
	}
	else if ( msg.getMsgType() == rfa::message::ReqMsgEnum )
	{
		parseReqMsg( text, static_cast<const ReqMsg&>(msg) );
	}
	else // rfa::message::GenericMsgEnum
	{
		text = RFA_String( "Parsing of GenericMsg is not yet implemented", 0, false );
	}
}

void GenericOMMParser::parseReqMsg( RFA_String& text, const ReqMsg& reqMsg )
{
	parseMsgInfo( text, reqMsg );

	if ( reqMsg.getHintMask() & ReqMsg::AttribInfoFlag )
	{
		parseMsgAttribInfo( text, reqMsg.getAttribInfo() );
	}

	if ( reqMsg.getHintMask() & ReqMsg::QualityOfServiceReqFlag )
	{
		parseMsgQOSReq( text, reqMsg.getRequestedQualityOfService() );
	}

	if ( reqMsg.getHintMask() & ReqMsg::PayloadFlag )
	{
		text += RFA_String( "Payload :\n", 0, false );

		if ( reqMsg.getPayload().isBlank() )
		{
			text += RFA_String( "Blank (empty)\n", 0, false );
		}
		else
		{
			parse( text, reqMsg.getPayload() );
		}
	}
	else
	{
		text += RFA_String( "No Payload\n", 0, false );
	}
}

void GenericOMMParser::parseRespMsg( RFA_String& text, const RespMsg& respMsg )
{
	if ( respMsg.getRespType() == RespMsg::RefreshEnum )
	{
		parseRefreshRespMsg( text, respMsg );
	}
	else if ( respMsg.getRespType() == RespMsg::StatusEnum )
	{
		parseStatusRespMsg( text, respMsg );
	}
	else // update
	{
		parseUpdateRespMsg( text, respMsg );
	}
}

void GenericOMMParser::parseStatusRespMsg( RFA_String& text, const RespMsg& respMsg )
{
	parseServiceAndItemName( text, respMsg );

	parseMsgInfo( text, respMsg );

	if ( respMsg.getHintMask() & RespMsg::RespStatusFlag )
	{
		parseMsgRespStatus( text, respMsg.getRespStatus() );
	}

	if ( respMsg.getHintMask() & RespMsg::AttribInfoFlag )
	{
		parseMsgAttribInfo( text, respMsg.getAttribInfo() );
	}

	if ( respMsg.getHintMask() & RespMsg::ManifestFlag )
	{
		parseMsgManifest( text, respMsg.getManifest() );
	}

	if ( respMsg.getHintMask() & RespMsg::QualityOfServiceFlag )
	{
		parseMsgQOSInfo( text, respMsg.getQualityOfService() );
	}
	
	if ( respMsg.getHintMask() & RespMsg::PayloadFlag )
	{
		text += RFA_String( "Payload :\n", 0, false );

		if ( respMsg.getPayload().isBlank() )
		{
			text += RFA_String( "Blank (empty)\n", 0, false );
		}
		else
		{
			parse( text, respMsg.getPayload() );
		}
	}
	else
	{
		text += RFA_String( "No Payload\n", 0, false );
	}
}

void GenericOMMParser::parseRefreshRespMsg( RFA_String& text, const RespMsg& respMsg )
{
	parseServiceAndItemName( text, respMsg );
	
	parseMsgInfo( text, respMsg );

	if ( respMsg.getHintMask() & RespMsg::RespStatusFlag )
	{
		parseMsgRespStatus( text, respMsg.getRespStatus() );
	}

	if ( respMsg.getHintMask() & RespMsg::AttribInfoFlag )
	{
		parseMsgAttribInfo( text, respMsg.getAttribInfo() );
	}
	
	if ( respMsg.getHintMask() & RespMsg::ManifestFlag )
	{
		parseMsgManifest( text, respMsg.getManifest() );
	}
	
	if ( respMsg.getHintMask() & RespMsg::QualityOfServiceFlag )
	{
		parseMsgQOSInfo( text, respMsg.getQualityOfService() );
	}

	if ( respMsg.getHintMask() & RespMsg::PayloadFlag )
	{
		text += RFA_String( "Payload :\n", 0, false );

		if ( respMsg.getPayload().isBlank() )
		{
			text += RFA_String( "Blank (empty)\n", 0, false );
		}
		else
		{
			parse( text, respMsg.getPayload() );
		}
	}
	else
	{
		text += RFA_String( "No Payload\n", 0, false );
	}

	if ( respMsg.getIndicationMask() & RespMsg::RefreshCompleteFlag )
	{
		text += RFA_String( "RefreshComplete flag received.\n", 0, false );
	}
	
	text += RFA_String( "\n", 0, false );
}

void GenericOMMParser::parseUpdateRespMsg( RFA_String& text, const RespMsg& respMsg )
{
	parseServiceAndItemName( text, respMsg );
	
	parseMsgInfo( text, respMsg );

	if ( respMsg.getHintMask() & RespMsg::RespStatusFlag )
	{
		parseMsgRespStatus( text, respMsg.getRespStatus() );
	}
	
	if ( respMsg.getHintMask() & RespMsg::AttribInfoFlag )
	{
		parseMsgAttribInfo( text, respMsg.getAttribInfo() );
	}
	
	if ( respMsg.getHintMask() & RespMsg::ManifestFlag )
	{
		parseMsgManifest( text, respMsg.getManifest() );
	}
	
	if ( respMsg.getHintMask() & RespMsg::QualityOfServiceFlag )
	{
		parseMsgQOSInfo( text, respMsg.getQualityOfService() );
	}

	if ( respMsg.getHintMask() & RespMsg::PayloadFlag )
	{
		text += RFA_String( "Payload :\n", 0, false );

		if ( respMsg.getPayload().isBlank() )
		{
			text += RFA_String( "Blank (empty)\n", 0, false );
		}
		else
		{
			parse( text, respMsg.getPayload() );
		}
	}
	else
	{
		text += RFA_String( "No Payload\n", 0, false );
	}

	text += RFA_String( "\n", 0, false );
}

void GenericOMMParser::parseServiceAndItemName( RFA_String& text, const RespMsg& respMsg )
{
	if ( respMsg.getHintMask() & RespMsg::AttribInfoFlag )
	{
		text += RFA_String( "\n", 0, false );
	
		if ( respMsg.getAttribInfo().getHintMask() & AttribInfo::ServiceNameFlag )
		{
			text += respMsg.getAttribInfo().getServiceName()
				+ RFA_String( " - ", 0, false );
		}

		if ( respMsg.getAttribInfo().getHintMask() & AttribInfo::NameFlag )
		{
			text += respMsg.getAttribInfo().getName()
				+ RFA_String( " (", 0, false )
				+ OMMStrings::msgModelTypeToString( respMsg.getMsgModelType() )
				+ RFA_String( ") :\n", 0, false );
		}
	}
}

void GenericOMMParser::parseMsgRespStatus( RFA_String& text, const RespStatus& status )
{
	text += RFA_String( "Status : dataState=\"", 0, false )
		+ OMMStrings::dataStateToString( status.getDataState() )
		+ RFA_String( "\" streamState=\"", 0, false )
		+ OMMStrings::streamStateToString( status.getStreamState() ) 
		+ RFA_String( "\" statusCode=\"", 0, false )
		+ OMMStrings::statusCodeToString( status.getStatusCode() )
		+ RFA_String( "\" statusText=\"", 0, false )
		+ status.getStatusText()
		+ RFA_String( "\"\n", 0, false );
}

void GenericOMMParser::parseMsgInfo( RFA_String& text, const ReqMsg& reqMsg )
{
	text += RFA_String( "ReqMsg :", 0, false );

	text += RFA_String( " hMask=\"", 0, false );
	text.append( (UInt32) reqMsg.getHintMask() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " iMask=\"", 0, false );
	text.append( (UInt32) reqMsg.getIndicationMask() );
	text += RFA_String( "\"", 0, false );
	
	if ( reqMsg.getInteractionType() == (ReqMsg::InitialImageFlag | ReqMsg::InterestAfterRefreshFlag) )
	{
		text += RFA_String( "  interationType: streaming req", 0, false );
	}
	else if ( reqMsg.getInteractionType() == ReqMsg::InitialImageFlag )
	{
		text += RFA_String( "  interationType: nonstreaming req", 0, false );
	}
	else if ( reqMsg.getInteractionType() == ReqMsg::InterestAfterRefreshFlag )
	{
		text += RFA_String( "  interationType: priority req or resume req", 0, false );
	}
	else if ( reqMsg.getInteractionType() & ReqMsg::PauseFlag )
	{
		text += RFA_String( " interationType: pause req", 0, false );
	}
	else if ( reqMsg.getInteractionType() == 0 )
	{
		text += RFA_String( " interationType: close req", 0, false );
	}
	else
	{
		text += RFA_String( " unexpected interactionType", 0, false );
	}

	text += RFA_String( "\n", 0, false );
}

void GenericOMMParser::parseMsgInfo( RFA_String& text, const RespMsg& respMsg )
{
	text += RFA_String( "RespMsg :", 0, false );

	text += RFA_String( " hMask=\"", 0, false );
	text.append( (UInt32) respMsg.getHintMask() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " iMask=\"", 0, false );
	text.append( (UInt32) respMsg.getIndicationMask() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " respType=\"", 0, false );
	text += OMMStrings::msgRespTypeToString( respMsg.getRespType() );
	text += RFA_String( "\"", 0, false );

	if ( respMsg.getRespType() == RespMsg::RefreshEnum )
	{
		text += RFA_String( " respTypeNum=\"", 0, false )
			+ OMMStrings::msgRefreshRespTypeNumToString( respMsg.getRespTypeNum() )
			+ RFA_String( "\"", 0, false );
	}
	else if ( respMsg.getRespType() == RespMsg::UpdateEnum )
	{
		text += RFA_String( " respTypeNum=\"", 0, false )
			+ OMMStrings::msgUpdateRespTypeNumToString( respMsg.getRespTypeNum() )
			+ RFA_String( "\"", 0, false );
	}

	if ( respMsg.getHintMask() & RespMsg::PrincipalIdentityFlag )
	{
		if ( respMsg.getPrincipalIdentity().getIdentityType() == PublisherPrincipalIdentityEnum )
		{
			const PublisherPrincipalIdentity& ppi = static_cast<const PublisherPrincipalIdentity&>(respMsg.getPrincipalIdentity());
			UInt32 userAddress = ppi.getUserAddress();
			UInt32 userID = ppi.getUserID();
			text.append( "message received from client (ID: " );
			text.append( userID );
			text.append( ") at address: " );
			text.append( userAddress );
			text.append( "\n" );
		}
	}

	text += RFA_String( "\n", 0, false );
}

void GenericOMMParser::parseMsgAttribInfo( RFA_String& text, const AttribInfo& info )
{
	text += RFA_String( "AttribInfo :", 0, false );

	text += RFA_String( " dMask=\"", 0, false );
	text.append( (UInt32) info.getDataMask() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " hMask=\"", 0, false );
	text.append( (UInt32) info.getHintMask() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " name=\"", 0, false );
	text += info.getName();
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " nameType=\"", 0, false );
	text += OMMStrings::itemAttribInfoNameTypeToString( info.getNameType() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " serviceName=\"", 0, false );
	text += info.getServiceName();
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " ID=\"", 0, false );
	text.append( info.getID() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( "\n", 0, false );
}

void GenericOMMParser::parseMsgManifest( RFA_String& text, const Manifest& info )
{
	text += RFA_String( "Manifest :", 0, false );

	text += RFA_String( " hMask=\"", 0, false );
	text.append( (UInt32) info.getHintMask() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " seqNum=\"", 0, false );
	text.append( (UInt32) info.getSeqNum() );
	text += RFA_String( "\"", 0, false );

	if ( info.getHintMask() & Manifest::ItemGroupFlag )
	{
		text += RFA_String( " itemGroupId=\"", 0, false );
		displayHexBuffer( text, info.getItemGroup() );
		text += RFA_String( "\"", 0, false );
	}
	 
	text += RFA_String( " filteredCount=\"", 0, false );
	text.append( (UInt32) info.getFilteredCount() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " filteredTime=\"", 0, false );
	text.append( (UInt32) info.getFilteredTime() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( "\n", 0, false );
}

void GenericOMMParser::parseMsgQOSInfo( RFA_String& text, const QualityOfService& info )
{
	text += RFA_String( "QualityOfService :", 0, false );

	text += RFA_String( " rate=\"", 0, false );
	text += OMMStrings::qosRateToString( info.getRate() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " timeliness=\"", 0, false );
	text += OMMStrings::qosTimelinessToString( info.getTimeliness() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( "\n", 0, false );
}

void GenericOMMParser::parseMsgQOSReq( RFA_String& text, const QualityOfServiceRequest& qosr )
{
	text += RFA_String( "QualityOfServiceRequst :", 0, false );

	text += RFA_String( " stream property=\"", 0, false );
	text += OMMStrings::qosPropertyToString( qosr.getStreamProperty() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " best rate=\"", 0, false );
	text += OMMStrings::qosRateToString( qosr.getBestRate() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " best timeliness=\"", 0, false );
	text += OMMStrings::qosTimelinessToString( qosr.getBestTimeliness() );
	text += RFA_String( "\"", 0, false );
	text += RFA_String( "\n", 0, false );

	text += RFA_String( " worst rate=\"", 0, false );
	text += OMMStrings::qosRateToString( qosr.getWorstRate() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " worst timeliness=\"", 0, false );
	text += OMMStrings::qosTimelinessToString( qosr.getWorstTimeliness() );
	text += RFA_String( "\"", 0, false );
	text += RFA_String( "\n", 0, false );
}

void GenericOMMParser::displayHexBuffer( RFA_String& text, const Buffer& buffer )
{
	const unsigned char* cbuf = buffer.c_buf();

	// custom routine to print hex values
	for ( int i = 0; i < buffer.size(); i++ )
	{
		char lowerByte = 0x0F & cbuf[i];
		char upperByte = (0xF0 & cbuf[i]) >> 4;

		if ( upperByte <= 0x09 )
			upperByte |= 0x30;
		else
			upperByte |= 0x37;

		if ( lowerByte <= 0x09 )
			lowerByte |= 0x30;
		else
			lowerByte |= 0x37;

		text += RFA_String( " ", 0, false )
			+ RFA_String( &upperByte, 1, false )
			+ RFA_String( &lowerByte, 1, false );
	}
}

void GenericOMMParser::parse( RFA_String& text, const Data& data )
{
	// always check before decoding if data is blank!
	if ( !data.isBlank() )
	{
		// parse based on dataType
		switch( data.getDataType() )
		{
		case DataBufferEnum:
			{
				const DataBuffer& input = static_cast<const DataBuffer&>(data);
				parseDataBuffer( text, input );
				break;
			}
		case VectorEnum:
			{
				const Vector& input = static_cast<const Vector&>(data);
				parseVector( text, input );
				break;
			}
		case FilterListEnum:
			{
				const FilterList& input = static_cast<const FilterList&>(data);
				parseFilterList( text, input );
				break;
			}
		case ElementListEnum:
			{
				const ElementList& input = static_cast<const ElementList&>(data);
				parseElementList( text, input );
				break;
			}
		case FieldListEnum:
			{
				const FieldList& input = static_cast<const FieldList&>(data);
				parseFieldList( text, input );
				break;
			}
		case SeriesEnum:
			{
				const Series& input = static_cast<const Series&>(data);
				parseSeries( text, input );
				break;
			}
		case MapEnum:
			{
				const Map& input = static_cast<const Map&>(data);
				parseMap( text, input );
				break;
			}
		case ArrayEnum:
			{
				const Array& input = static_cast<const Array&>(data);
				parseArray( text, input );
				break;
			}
		case NoDataEnum:
			{
				break;
			}	
		default:
			{
				text += RFA_String( "\nERROR: GenericOMMParser::parse() trying to parse unsupported data format\n", 0, false );
				break;
			}
		}
	}
	else //blank data
	{
		text += RFA_String( " value=\"blank\"\n", 0, false );
	}
}

void GenericOMMParser::parseFieldList( RFA_String& text, const FieldList& input )	
{
	m_indents++;
	writeIndents( text );

	// parse data type specific info
	text += RFA_String( "Fieldlist", 0, false );

	text += RFA_String( " hMask=\"", 0, false );
	text.append( (UInt32)input.getHintMask() );
	text += RFA_String( "\"", 0, false );
	
	text += RFA_String( " cMask=\"", 0, false );
	text.append( (UInt32)input.getContentMask() );
	text += RFA_String( "\"", 0, false );
	
	text += RFA_String( " dictId=\"", 0, false );
	text.append( (Int16)input.getInfoDictID() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " fieldListNum=\"", 0, false );
	text.append( (Int16)input.getInfoFieldListNum() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " count=\"", 0, false );
	text.append( (UInt32)input.getStandardDataCount() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " defID=\"", 0, false );
	text.append( (UInt32)input.getDataDefID() );
	text += RFA_String( "\"\n", 0, false );

	FieldListReadIterator it;
	it.start( input );
	while( !it.off() )
	{
		parseFieldEntry( text, it.value() );	
		it.forth();
	}

	m_indents--;
}

void GenericOMMParser::parseFieldEntry( RFA_String& text, const FieldEntry& input )	
{	
	m_indents++;
	writeIndents( text );

	// must cast out of UInt8s return types,they are interpreted as unsigned chars
	text += RFA_String( "FieldEntry", 0, false );

	text += RFA_String( " mask=\"", 0, false );
	text.append( (UInt32)input.getContentMask() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " fieldID=\"", 0, false );
	text.append( (Int32)input.getFieldID() );
	text += RFA_String( "\"", 0, false );

	try
	{
		// Look up field definition in field dictionary by fid
		const rfa::rdm::RDMFidDef& fieldDef = m_dictionary.getFidDef(input.getFieldID());

		text += RFA_String( " fieldName=\"", 0, false );
		text += fieldDef.getName();
		text += RFA_String( "\"", 0, false );

		text += RFA_String( " dataType=\"", 0, false );
		text += OMMStrings::dataTypeToString( (UInt8)fieldDef.getOMMType());
		text += RFA_String( "\"", 0, false );

		parse( text, input.getData( (UInt8)fieldDef.getOMMType() ) );
	}
	catch(InvalidUsageException& e)
	{
		text += RFA_String(e.getStatus().getStatusText().c_str(), 0, false);
	} 

	m_indents--;
}

void GenericOMMParser::parseElementList( RFA_String& text, const ElementList& input )	
{
	m_indents++;
	writeIndents( text );

	// parse data type specific info

	text += RFA_String( "ElementList", 0, false );

	text += RFA_String( " hMask=\"", 0, false );
	text.append( (UInt32)input.getHintMask() );
	text += RFA_String( "\"", 0, false );
	
	text += RFA_String( " cMask=\"", 0, false );
	text.append( (UInt32)input.getContentMask() );
	text += RFA_String( "\"", 0, false );
	
	text += RFA_String( " count=\"", 0, false );
	text.append( (UInt32)input.getStandardDataCount() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " defID=\"", 0, false );
	text.append( (UInt32)input.getDataDefID() );
	text += RFA_String( "\"\n", 0, false );

	ElementListReadIterator it;
	it.start( input );
	while( !it.off() )
	{
		parseElementEntry( text, it.value() );	
		it.forth();
	}

	m_indents--;
}

void GenericOMMParser::parseElementEntry( RFA_String& text, const ElementEntry& input )	
{	
	m_indents++;
	writeIndents( text );

	text += RFA_String( "ElementEntry", 0, false );

	text += RFA_String( " mask=\"", 0, false );
	text.append( (UInt32)input.getContentMask() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " name=\"", 0, false );
	text += input.getName();
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " dataType=\"", 0, false );
	text += OMMStrings::dataTypeToString( input.getData().getDataType() );
	text += RFA_String( "\"", 0, false );

	parse( text, input.getData() );

	m_indents--;
}


void GenericOMMParser::parseMap( RFA_String& text, const Map& input )
{
	m_indents++;
	writeIndents( text );

	text += RFA_String( "Map", 0, false );

	text += RFA_String( " hMask=\"", 0, false );
	text.append( (UInt32)input.getHintMask() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " iMask=\"", 0, false );
	text.append( (UInt32)input.getIndicationMask() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " keyType=\"", 0, false );
	text += OMMStrings::dataTypeToString( input.getKeyDataType() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " dataDefCount=\"", 0, false );
	text.append( (UInt32)input.getDataDefCount() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " count=\"", 0, false );
	text.append( (UInt32)input.getCount() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " totalCountHint=\"", 0, false );
	text.append( (UInt32)input.getTotalCountHint() );
	text += RFA_String( "\"\n", 0, false );

	MapReadIterator it;
	it.start( input );

	if ( input.getIndicationMask() & Map::SummaryDataFlag )
	{
		m_indents++;
		writeIndents( text );
		text += RFA_String( "SummaryData : \n", 0, false );
		parse( text, input.getSummaryData() );
		m_indents--;
	}

	while( !it.off() )
	{
		parseMapEntry( text, it.value() );	
		it.forth();
	}

	m_indents--;
	text += RFA_String( "\n", 0, false );
}

void GenericOMMParser::parseMapEntry( RFA_String& text, const MapEntry& input )
{
	m_indents++;
	writeIndents( text );

	text += RFA_String( "MapEntry", 0, false );

	text += RFA_String( " hMask=\"", 0, false );
	text.append( (UInt32)input.getHintMask() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " action=\"", 0, false );
	text += OMMStrings::mapActionToString( input.getAction() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " key=\"", 0, false );
	text += static_cast<const DataBuffer&> (input.getKeyData()).getAsString();
	text += RFA_String( "\"", 0, false );

	if ( !(input.getData().getDataType() == DataBufferEnum ) )
		text += RFA_String( "\n", 0, false ); // for DataBuffer we do not need a newline

	if ( input.getAction() != MapEntry::Delete )
		parse( text, input.getData() );

	m_indents--;
}

void GenericOMMParser::parseVector( RFA_String& text, const Vector& input )
{
	m_indents++;
	writeIndents( text );

	text += RFA_String( "Vector ", 0, false );

	text += RFA_String( " hMask=\"", 0, false );
	text.append( (UInt32)input.getHintMask() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " iMask=\"", 0, false );
	text.append( (UInt32)input.getIndicationMask() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " dataDefCount=\"", 0, false );
	text.append( (UInt32)input.getDataDefCount() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " count=\"", 0, false );
	text.append( (UInt32)input.getCount() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " totalCountHint=\"", 0, false );
	text.append( (UInt32)input.getTotalCountHint() );
	text += RFA_String( "\"\n", 0, false );
	
	VectorReadIterator it;
	it.start( input );

	if ( input.getIndicationMask() & Vector::SummaryDataFlag )
	{
		m_indents++;
		writeIndents( text );

		text += RFA_String( "SummaryData : \n", 0, false );
		parse( text, input.getSummaryData() );
		m_indents--;
	}

	while( !it.off() )
	{
		parseVectorEntry( text, it.value() );	
		it.forth();
	}

	m_indents--;
	text += RFA_String( "\n", 0, false );
}

void GenericOMMParser::parseVectorEntry( RFA_String& text, const VectorEntry& input )
{
	m_indents++;
	writeIndents( text );

	text += RFA_String( "VectorEntry", 0, false );

	text += RFA_String( " hMask=\"", 0, false );
	text.append( (UInt32)input.getHintMask() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " action=\"", 0, false );
	text += OMMStrings::vectorActionToString( input.getAction() );
	text += RFA_String( "\"", 0, false );

	if ( !(input.getData().getDataType() == DataBufferEnum) )
		text += RFA_String( "\n", 0, false ); // for DataBuffer we do not need a newline

	if ( input.getAction() != VectorEntry::Delete && input.getAction() != VectorEntry::Clear )
		parse( text, input.getData() );
	
	m_indents--;
}

void GenericOMMParser::parseSeries( RFA_String& text, const Series& input )
{
	m_indents++;
	writeIndents( text );

	text += RFA_String( "Series", 0, false );

	text += RFA_String( " hMask=\"", 0, false );
	text.append( (UInt32)input.getHintMask() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " iMask=\"", 0, false );
	text.append( (UInt32)input.getIndicationMask() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " dataDefCount=\"", 0, false );
	text.append( (UInt32)input.getDataDefCount() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " count=\"", 0, false );
	text.append( (UInt32)input.getCount() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " totalCountHint=\"", 0, false );
	text.append( (UInt32)input.getTotalCountHint() );
	text += RFA_String( "\"\n", 0, false );

	SeriesReadIterator it;
	it.start( input );

	if ( input.getIndicationMask() & Series::SummaryDataFlag )
	{
		m_indents++;
		writeIndents( text );
		text += RFA_String( "SummaryData : \n", 0, false );
		parse( text, input.getSummaryData() );
		m_indents--;
	}

	while( !it.off() )
	{
		parseSeriesEntry( text, it.value() );	
		it.forth();
	}

	m_indents--;
	text += RFA_String( "\n", 0, false );
}

void GenericOMMParser::parseSeriesEntry( RFA_String& text, const SeriesEntry& input )
{
	m_indents++;
	writeIndents( text );

	text += RFA_String( "SeriesEntry", 0, false );

	if ( !(input.getData().getDataType() == DataBufferEnum) )
		text += RFA_String( "\n", 0, false );  //for DataBuffer we do not need a newline

	parse( text, input.getData() );
	m_indents--;
}

void GenericOMMParser::parseFilterList(RFA_String& text, const FilterList& input)
{
	m_indents++;
	writeIndents( text );

	text += RFA_String( "FilterList", 0, false );

	text += RFA_String( " hMask=\"", 0, false );
	text.append( (UInt32)input.getHintMask() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " iMask=\"", 0, false );
	text.append( (UInt32)input.getIndicationMask() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " count=\"", 0, false );
	text.append( (UInt32)input.getCount() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " totalCountHint=\"", 0, false );
	text.append( (UInt32)input.getTotalCountHint() );
	text += RFA_String( "\"\n", 0, false );

	FilterListReadIterator it;
	it.start( input );

	while( !it.off() )
	{
		parseFilterEntry( text, it.value() );	
		it.forth();
	}

	m_indents--;
	text += RFA_String( "\n", 0, false );
}

void GenericOMMParser::parseFilterEntry( RFA_String& text, const FilterEntry& input )
{
	m_indents++;
	writeIndents( text );

	text += RFA_String( "FilterEntry", 0, false );

	text += RFA_String( " hMask=\"", 0, false );
	text.append( (UInt32)input.getHintMask() );
	text += RFA_String( "\"", 0, false );

	text += RFA_String( " action=\"", 0, false );
	text += OMMStrings::filterActionToString( input.getAction() );
	text += RFA_String( "\"", 0, false );

	if ( !(input.getData().getDataType() == DataBufferEnum) )
		text += RFA_String( "\n", 0, false ); // for DataBuffer we do not need a newline

	if ( input.getAction() != FilterEntry::Clear )
		parse( text, input.getData() );
	m_indents--;
}

void GenericOMMParser::parseArray(RFA_String& text, const Array& input)
{
	m_indents++;
	writeIndents( text );
	text += RFA_String( "Array", 0, false );

	text += RFA_String( " count=\"", 0, false );
	text.append( (UInt32)input.getCount() );
	text += RFA_String( "\"", 0, false );

	ArrayReadIterator it;
	it.start( input );

	while( !it.off() )
	{
		parseArrayEntry( text, it.value() );	
		it.forth();
	}

	m_indents--;
	text += RFA_String( "\n", 0, false );
}

void GenericOMMParser::parseArrayEntry( RFA_String& text, const ArrayEntry& input )
{
	m_indents++;
	writeIndents( text );

	parse( text, input.getData() );
	m_indents--;
}

void GenericOMMParser::parseDataBuffer(RFA_String& text, const DataBuffer& dataBuffer)	
{
	UInt8 dataBufferType = dataBuffer.getDataBufferType();

	text += RFA_String( " value=\"", 0, false );
	if ( dataBuffer.isBlank() )  // blank buffer
	{
		text += RFA_String( "\"blank\"\n", 0, false );
		return;
	}

	switch( dataBufferType )
	{
		case DataBuffer::Int32Enum:
		case DataBuffer::UInt32Enum:
		case DataBuffer::IntEnum:		
		case DataBuffer::UIntEnum:
		case DataBuffer::FloatEnum:
		case DataBuffer::DoubleEnum:
		case DataBuffer::Real32Enum:
		case DataBuffer::RealEnum:
		case DataBuffer::StringAsciiEnum:
		case DataBuffer::StringUTF8Enum:
		case DataBuffer::StringRMTESEnum:
		case DataBuffer::EnumerationEnum:
			text += dataBuffer.getAsString() + RFA_String( "\"", 0, false );
			break;

		case DataBuffer::DateEnum:
			{
				text += dataBuffer.getAsString() + RFA_String( "\"", 0, false );
				break;
			}

		case DataBuffer::TimeEnum:
			{
				text += dataBuffer.getAsString() + RFA_String( "\"", 0, false );
				break;
			}

		case DataBuffer::DateTimeEnum:
			{
				text.append( (UInt32) dataBuffer.getDateTime().getMonth() );
				text += RFA_String( "/", 0, false );
				text.append( (UInt32) dataBuffer.getDateTime().getDay() );
				text += RFA_String( "/", 0, false );
				text.append( (UInt32) dataBuffer.getDateTime().getYear() );
				text += RFA_String( " ", 0, false );
				text.append( (UInt32) dataBuffer.getDateTime().getHour() );
				text += RFA_String( ":", 0, false );
				text.append( (UInt32) dataBuffer.getDateTime().getMinute() );
				text += RFA_String( ":", 0, false );
				text.append( (UInt32) dataBuffer.getDateTime().getSecond() );
				text += RFA_String( "\"", 0, false );
			}
			break;
		
		case DataBuffer::QualityOfServiceInfoEnum:
			{
				text.append( (Int32) dataBuffer.getQualityOfServiceInfo().getQualityOfService().getRate() );
				text += RFA_String( " ", 0, false );
				text.append( (Int32) dataBuffer.getQualityOfServiceInfo().getQualityOfService().getTimeliness() );
				text += RFA_String( "\"", 0, false );
			}
			break;

		case DataBuffer::BufferEnum:
		case DataBuffer::XMLEnum:
		case DataBuffer::OpaqueEnum:
		case DataBuffer::ANSI_PageEnum:
			text += (const char*)dataBuffer.getBuffer().c_buf() + RFA_String( "\"", 0, false );
			break;

	case DataBuffer::RespStatusEnum:
		parseMsgRespStatus( text, dataBuffer.getRespStatus() );
		break;

	case DataBuffer::NoDataBufferEnum:
	case DataBuffer::UnknownDataBufferEnum:
	default:
		{
			text += RFA_String( "\nGenericOMMParser::parseDataBuffer() trying to parse unsupported DataBuffer format.\n", 0, false );
		}
	}
	
	text += RFA_String( "\n", 0, false );
}

/// write formatting
void GenericOMMParser::writeIndents( RFA_String& text )
{
	for ( int i = 0; i < m_indents; i++ )
		text += RFA_String( "  ", 0, false ); //2 spaces
}
