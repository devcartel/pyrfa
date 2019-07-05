#include "OMMStrings.h"

#include "RDM/RDM.h"
#include "Data/FilterEntry.h"
#include "Data/VectorEntry.h"
#include "Data/MapEntry.h"
#include "Message/RespMsg.h"
#include "Message/ReqMsg.h"
#include "SessionLayer/ConnectionStatus.h"
#include "Logger/LoggerNotifyEvent.h"

using namespace rfa;
using namespace rfa::sessionLayer;
using namespace rfa::data;
using namespace rfa::message;
using namespace rfa::common;
using namespace rfa::logger;

RFA_String OMMStrings::text;

RFA_String OMMStrings::RespMsgEnumText( "Response Message", 0, false );
RFA_String OMMStrings::ReqMsgEnumText( "Request Message", 0, false );
RFA_String OMMStrings::GenericMsgEnumText( "Generic Message", 0, false );
RFA_String OMMStrings::PostMsgEnumText( "Post Message", 0, false );
RFA_String OMMStrings::AckMsgEnumText( "Ack Message", 0, false );

RFA_String OMMStrings::MmtLoginText( "Login", 0, false );
RFA_String OMMStrings::MmtDirectoryText( "Directory", 0, false );
RFA_String OMMStrings::MmtDictionaryText( "Dictionary", 0, false );
RFA_String OMMStrings::MmtMarketPriceText( "MarketPrice", 0, false );
RFA_String OMMStrings::MmtMarketByOrderText( "MarketByOrder", 0, false );
RFA_String OMMStrings::MmtMarketByPriceText( "MarketByPrice", 0, false );
RFA_String OMMStrings::MmtMarketMakerText( "MarketMaker", 0, false );
RFA_String OMMStrings::MmtSymbolListText( "SymbolList", 0, false );
RFA_String OMMStrings::MmtHistorText( "History", 0, false );
RFA_String OMMStrings::MmtYieldCurveText( "Yield Curve", 0, false );
RFA_String OMMStrings::MmtContributionText( "Contribution", 0, false );

RFA_String OMMStrings::RefreshEnumText( "Refresh", 0, false );
RFA_String OMMStrings::StatusEnumText( "Status", 0, false );
RFA_String OMMStrings::UpdateEnumText( "Update", 0, false );

RFA_String OMMStrings::RS_NoneEnumText( "None", 0, false );
RFA_String OMMStrings::RS_NotFoundEnumText( "NotFound", 0, false );
RFA_String OMMStrings::RS_TimeoutEnumText( "Timeout", 0, false );
RFA_String OMMStrings::RS_NotAuthorizedEnumText( "NotAuthorized", 0, false );
RFA_String OMMStrings::RS_InvalidArgumentEnumText( "InvalidArgument", 0, false );
RFA_String OMMStrings::RS_UsageErrorEnumText( "UsageError", 0, false );
RFA_String OMMStrings::RS_PreemptedEnumText( "Preempted", 0, false );
RFA_String OMMStrings::RS_JustInTimeFilteringStartedEnumText( "JustInTimeFilteringStarted", 0, false );
RFA_String OMMStrings::RS_TickByTickResumedEnumText( "TickByTickResumed", 0, false );
RFA_String OMMStrings::RS_FailoverStartedEnumText( "FailoverStarted", 0, false );
RFA_String OMMStrings::RS_FailoverCompletedEnumText( "FailoverCompleted", 0, false );
RFA_String OMMStrings::RS_GapDetectedEnumText( "GapDetected", 0, false );
RFA_String OMMStrings::RS_NoResourcesEnumText( "NoResources", 0, false );
RFA_String OMMStrings::RS_TooManyItemsEnumText( "TooManyItems", 0, false );
RFA_String OMMStrings::RS_AlreadyOpenEnumText( "AlreadyOpen", 0, false );
RFA_String OMMStrings::RS_SourceUnknownEnumText( "SourceUnknown", 0, false );
RFA_String OMMStrings::RS_NotOpenEnumText( "NotOpen", 0, false );
RFA_String OMMStrings::RS_NonUpdatingItemEnumText( "NonUpdatingItem", 0, false );
RFA_String OMMStrings::RS_UnsupportedViewTypeEnumText( "UnsupportedViewType", 0, false );
RFA_String OMMStrings::RS_InvalidViewEnumText( "InvalidView", 0, false );
RFA_String OMMStrings::RS_FullViewProvidedEnumText( "FullViewProvided", 0, false );
RFA_String OMMStrings::RS_UnableToRequestAsBatchEnumText( "UnableToRequestAsBatch", 0, false );
RFA_String OMMStrings::RS_NoBatchViewSupportInReqEnumText("NoBatchViewSupportInReq", 0, false);
RFA_String OMMStrings::RS_ExceededMaxMountsPerUserEnumText("ExceededMaxMountsPerUser", 0, false);	
RFA_String OMMStrings::RS_ErrorEnumText("ErrorEnum", 0, false); 
RFA_String OMMStrings::RS_DacsDownEnumText("DacsDown", 0, false);	
RFA_String OMMStrings::RS_UserUnknownToPermSysEnumText("UserUnknownToPermSys", 0, false); 
RFA_String OMMStrings::RS_DacsMaxLoginsReachedEnumText("DacsMaxLoginsReached", 0, false);
RFA_String OMMStrings::RS_DacsUserAccessToAppDeniedEnumText("DacsUserAccessToAppDenied", 0, false);	

RFA_String OMMStrings::CS_NoneEnumText( "None", 0, false );
RFA_String OMMStrings::CS_AccessDeniedEnumText( "AccessDenied", 0, false );
RFA_String OMMStrings::CS_IntermittentProblemsEnumText( "IntermittentProblems", 0, false );
RFA_String OMMStrings::CS_ServerSwitchedEnumText( "ServerSwitched", 0, false );
RFA_String OMMStrings::CS_ClosedEnumText( "Closed", 0, false );

RFA_String OMMStrings::QOS_DynamicStreamEnumText( "DynamicStream", 0, false );
RFA_String OMMStrings::QOS_StaticStreamEnumText( "StaticStream", 0, false );

RFA_String OMMStrings::QOS_tickByTickEnumText( "TickByTick", 0, false );
RFA_String OMMStrings::QOS_justInTimeFilteredRateEnumText( "JustInTimeFilteredRate", 0, false );
RFA_String OMMStrings::QOS_unspecifiedRateEnumText( "UnspecifiedRate", 0, false );

RFA_String OMMStrings::QOS_realTimeEnumText( "RealTime", 0, false );
RFA_String OMMStrings::QOS_unspecifiedTimelinessEnumText( "UnspecifiedTimeliness", 0, false );
RFA_String OMMStrings::QOS_unspecifiedDelayedTimelinessEnumText( "UnspecifiedDelayedTimeliness", 0, false );

RFA_String OMMStrings::RS_OkEnumText( "Ok", 0, false );
RFA_String OMMStrings::RS_SuspectEnumText( "Suspect", 0, false );
RFA_String OMMStrings::RS_NoChangeEnumText( "NoChange", 0, false );

RFA_String OMMStrings::RS_UnspecifiedStreamStateEnumText( "UnspecifiedStreamState", 0, false );
RFA_String OMMStrings::RS_OpenEnumText( "Open", 0, false );
RFA_String OMMStrings::RS_NonStreamingEnumText( "NonStreaming", 0, false );
RFA_String OMMStrings::RS_ClosedRecoverEnumText( "ClosedRecover", 0, false );
RFA_String OMMStrings::RS_ClosedEnumText( "Closed", 0, false );
RFA_String OMMStrings::RS_RedirectedEnumText( "Redirected", 0, false );

RFA_String OMMStrings::AI_IN_UnspecifiedText( "INSTRUMENT_NAME_UNSPECIFIED", 0, false );
RFA_String OMMStrings::AI_IN_RicText( "INSTRUMENT_NAME_RIC", 0, false );
RFA_String OMMStrings::AI_IN_NameMaxReservedText( "INSTRUMENT_NAME_MAX_RESERVED", 0, false );

RFA_String OMMStrings::AI_UserEmailAddressText( "USER_EMAIL_ADDRESS", 0, false );
RFA_String OMMStrings::AI_UserNameText( "USER_NAME", 0, false );
RFA_String OMMStrings::AI_UserTokenText( "USER_TOKEN", 0, false );

RFA_String OMMStrings::UpdateType_UnspecifiedText( "INSTRUMENT_UPDATE_UNSPECIFIED", 0, false );
RFA_String OMMStrings::UpdateType_QuoteText( "INSTRUMENT_UPDATE_QUOTE", 0, false );
RFA_String OMMStrings::UpdateType_TradeText( "INSTRUMENT_UPDATE_TRADE", 0, false );
RFA_String OMMStrings::UpdateType_NewsAlertText( "INSTRUMENT_UPDATE_NEWS_ALERT", 0, false );
RFA_String OMMStrings::UpdateType_VolumeAlertText( "INSTRUMENT_UPDATE_VOLUME_ALERT", 0, false );
RFA_String OMMStrings::UpdateType_OrderIndicationText( "INSTRUMENT_UPDATE_ORDER_INDICATION", 0, false );
RFA_String OMMStrings::UpdateType_ClosingRunText( "INSTRUMENT_UPDATE_CLOSING_RUN", 0, false );
RFA_String OMMStrings::UpdateType_CorrectionText( "INSTRUMENT_UPDATE_CORRECTION", 0, false );
RFA_String OMMStrings::UpdateType_MarketDigestText( "INSTRUMENT_UPDATE_MARKET_DIGEST", 0, false );
RFA_String OMMStrings::UpdateType_QuotesTradeText( "INSTRUMENT_UPDATE_QUOTES_TRADE", 0, false );
RFA_String OMMStrings::UpdateType_MultipleText( "INSTRUMENT_UPDATE_MULTIPLE", 0, false );
RFA_String OMMStrings::UpdateType_VerifyText( "INSTRUMENT_UPDATE_VERIFY", 0, false );

RFA_String OMMStrings::RefreshType_SolicitedText( "REFRESH_SOLICITED", 0, false );
RFA_String OMMStrings::RefreshType_UnsolicitedText( "REFRESH_UNSOLICITED", 0, false );

RFA_String OMMStrings::DT_VectorText( "Vector", 0, false );
RFA_String OMMStrings::DT_ElementListText( "ElementList", 0, false );
RFA_String OMMStrings::DT_FieldListText( "FieldList", 0, false );
RFA_String OMMStrings::DT_SeriesText( "Series", 0, false );
RFA_String OMMStrings::DT_ArrayText( "Array", 0, false );
RFA_String OMMStrings::DT_MapText( "Map", 0, false );
RFA_String OMMStrings::DT_FilterListText( "FilterList", 0, false );
RFA_String OMMStrings::DT_DataBufferText( "DataBuffer", 0, false );
RFA_String OMMStrings::DT_DB_EnumerationText( "DataBuffer::Enumeration", 0, false );
RFA_String OMMStrings::DT_DB_Int32EnumText( "DataBuffer::Int32", 0, false );
RFA_String OMMStrings::DT_DB_UInt32EnumText( "DataBuffer::UInt32", 0, false );
RFA_String OMMStrings::DT_DB_IntEnumText( "DataBuffer::Int", 0, false );
RFA_String OMMStrings::DT_DB_UIntEnumText( "DataBuffer::UInt", 0, false );
RFA_String OMMStrings::DT_DB_FloatText( "DataBuffer::Float", 0, false );
RFA_String OMMStrings::DT_DB_DoubleText( "DataBuffer::Double", 0, false );
RFA_String OMMStrings::DT_DB_Real32Text( "DataBuffer::Real32", 0, false );
RFA_String OMMStrings::DT_DB_RealText( "DataBuffer::Real", 0, false );
RFA_String OMMStrings::DT_DB_DateText( "DataBuffer::Date", 0, false );
RFA_String OMMStrings::DT_DB_TimeText( "DataBuffer::Time", 0, false );
RFA_String OMMStrings::DT_DB_DateTimeText( "DataBuffer::DateTime", 0, false );
RFA_String OMMStrings::DT_DB_QualityOfServiceInfoText( "DataBuffer::QualityOfServiceInfo", 0, false );
RFA_String OMMStrings::DT_DB_BufferText( "DataBuffer::Buffer", 0, false );
RFA_String OMMStrings::DT_DB_StringAsciiText( "DataBuffer::StringAscii", 0, false );
RFA_String OMMStrings::DT_DB_StringUTF8Text( "DataBuffer::StringUTF8", 0, false );
RFA_String OMMStrings::DT_DB_StringRMTESText( "DataBuffer::StringRMTES", 0, false );
RFA_String OMMStrings::DT_DB_OpaqueText( "DataBuffer::Opaque", 0, false );
RFA_String OMMStrings::DT_DB_XMLText( "DataBuffer::XNL", 0, false );
RFA_String OMMStrings::DT_DB_ANSI_PageText( "DataBuffer::ANSI_Page", 0, false );
RFA_String OMMStrings::DT_DB_UnknownDataBufferText( "DataBuffer::UnknownDataBuffer", 0, false );
RFA_String OMMStrings::DT_DB_NoDataText( "DataBuffer::NoData", 0, false );

RFA_String OMMStrings::Action_AddText( "Add", 0, false );
RFA_String OMMStrings::Action_UpdateText( "Update", 0, false );
RFA_String OMMStrings::Action_DeleteText( "Delete", 0, false );
RFA_String OMMStrings::Action_SetText( "Set", 0, false );
RFA_String OMMStrings::Action_ClearText( "Clear", 0, false );
RFA_String OMMStrings::Action_InsertText( "Insert", 0, false );

RFA_String OMMStrings::Dictionary_InfoText( "DICTIONARY_INFO", 0, false );
RFA_String OMMStrings::Dictionary_MinimalText( "DICTIONARY_MINIMAL", 0, false );
RFA_String OMMStrings::Dictionary_NormalText( "DICTIONARY_NORMAL", 0, false );
RFA_String OMMStrings::Dictionary_VerboseText( "DICTIONARY_VERBOSE", 0, false );

RFA_String OMMStrings::ConnectionEventText( "ConnectionEvent", 0, false );
RFA_String OMMStrings::OMMItemEventText( "OMMItemEvent", 0, false );
RFA_String OMMStrings::OMMActiveClientSessionEventText( "OMMActiveClientSessionEvent", 0, false );
RFA_String OMMStrings::OMMInactiveClientSessionEventText( "OMMInactiveClientSessionEvent", 0, false );
RFA_String OMMStrings::OMMSolicitedItemEventText( "OMMSolicitedItemEvent", 0, false );
RFA_String OMMStrings::OMMCmdErrorEventText( "OMMCmdErrorEvent", 0, false );
RFA_String OMMStrings::LoggerNotifyEventText( "LoggerNotifyEvent", 0, false );

const RFA_String& OMMStrings::msgTypeToString( UInt32 mt )
{
	switch ( mt )
	{
		case RespMsgEnum:
			return RespMsgEnumText;
		case ReqMsgEnum:
			return ReqMsgEnumText;
		case GenericMsgEnum:
			return GenericMsgEnumText;
		case PostMsgEnum:
			return PostMsgEnumText;
		case AckMsgEnum:
			return AckMsgEnumText;
		default:
		{
			text.set( "Unknown message type (", 0, false );
			text.append( mt );
			text.append( ")" );
			return text;
		}
	}
}

const RFA_String& OMMStrings::msgModelTypeToString( UInt mmt )
{
	switch ( mmt )
	{
		case rdm::MMT_LOGIN:
			return MmtLoginText;
		case rdm::MMT_DIRECTORY:
			return MmtDirectoryText;
		case rdm::MMT_DICTIONARY:
			return MmtDictionaryText;
		case rdm::MMT_MARKET_PRICE:
			return MmtMarketPriceText;
		case rdm::MMT_MARKET_BY_ORDER:
			return MmtMarketByOrderText;
		case rdm::MMT_MARKET_BY_PRICE:
			return MmtMarketByPriceText;
		case rdm::MMT_MARKET_MAKER:
			return MmtMarketMakerText;
		case rdm::MMT_SYMBOL_LIST:
			return MmtSymbolListText;
		case rdm::MMT_HISTORY:
			return MmtHistorText;
		case rdm::MMT_YIELD_CURVE:
			return MmtYieldCurveText;
		case rdm::MMT_CONTRIBUTION:
			return MmtContributionText;
		default:
		{
			if ( mmt <= 127 )
				text.set( "Invalid mmt(", 0, false );
			else 
				text.set( "UserDef mmt(", 0, false );
			text.append( mmt );
			text.append( ")" );
			return text;
		}
	}
}

const RFA_String& OMMStrings::msgRespTypeToString( UInt32 respType )
{
	switch ( respType )
	{
		case RespMsg::RefreshEnum:
			return RefreshEnumText;
		case RespMsg::StatusEnum:
			return StatusEnumText;
		case RespMsg::UpdateEnum:
			return UpdateEnumText;
		default:
		{
			text.set( "Invalid respType (", 0, false );
			text.append( respType );
			text.append( ")" );
			return text;
		}
	}
}

const RFA_String& OMMStrings::statusCodeToString( UInt8 statusCode )
{
	switch ( statusCode )
	{
		case RespStatus::NoneEnum:
			return RS_NoneEnumText;
		case RespStatus::NotFoundEnum:
			return RS_NotFoundEnumText;
		case RespStatus::TimeoutEnum:
			return RS_TimeoutEnumText;
		case RespStatus::NotAuthorizedEnum:
			return RS_NotAuthorizedEnumText;
		case RespStatus::InvalidArgumentEnum:
			return RS_InvalidArgumentEnumText;
		case RespStatus::UsageErrorEnum:
			return RS_UsageErrorEnumText;
		case RespStatus::PreemptedEnum:
			return RS_PreemptedEnumText;
		case RespStatus::JustInTimeFilteringStartedEnum:
			return RS_JustInTimeFilteringStartedEnumText;
        case RespStatus::TickByTickResumedEnum:
            return RS_TickByTickResumedEnumText;
        case RespStatus::FailoverStartedEnum:
            return RS_FailoverStartedEnumText;
        case RespStatus::FailoverCompletedEnum:
            return RS_FailoverCompletedEnumText;
        case RespStatus::GapDetectedEnum:
            return RS_GapDetectedEnumText;
		case RespStatus::NoResourcesEnum:
            return RS_NoResourcesEnumText;
		case RespStatus::TooManyItemsEnum:
            return RS_TooManyItemsEnumText;
		case RespStatus::AlreadyOpenEnum:
            return RS_AlreadyOpenEnumText;
		case RespStatus::SourceUnknownEnum:
            return RS_SourceUnknownEnumText;
		case RespStatus::NotOpenEnum:
            return RS_NotOpenEnumText;
		case RespStatus::NonUpdatingItemEnum:
            return RS_NonUpdatingItemEnumText;
		case RespStatus::UnsupportedViewTypeEnum:
            return RS_UnsupportedViewTypeEnumText;
		case RespStatus::InvalidViewEnum:
            return RS_InvalidViewEnumText;
		case RespStatus::FullViewProvidedEnum:
            return RS_FullViewProvidedEnumText;
		case RespStatus::UnableToRequestAsBatchEnum:
            return RS_UnableToRequestAsBatchEnumText;
		case RespStatus::NoBatchViewSupportInReqEnum:
			return RS_NoBatchViewSupportInReqEnumText;
		case RespStatus::ExceededMaxMountsPerUserEnum:
			return RS_ExceededMaxMountsPerUserEnumText;	
		case RespStatus::ErrorEnum:
			return RS_ErrorEnumText; 
		case RespStatus::DacsDownEnum:
			return RS_DacsDownEnumText;	
		case RespStatus::UserUnknownToPermSysEnum:
			return RS_UserUnknownToPermSysEnumText; 
		case RespStatus::DacsMaxLoginsReachedEnum:
			return RS_DacsMaxLoginsReachedEnumText; 
		case RespStatus::DacsUserAccessToAppDeniedEnum:
			return RS_DacsUserAccessToAppDeniedEnumText;	
		default:
		{
			text.set( "Unknown status code (", 0, false );
			text.append( statusCode );
			text.append( ")" );
			return text;
		}
	}
}

const RFA_String& OMMStrings::connectionStatusCodeToString( UInt8 statusCode )
{
	switch ( statusCode )
	{
		case ConnectionStatus::None:
			return CS_NoneEnumText;
		case ConnectionStatus::AccessDenied:
			return CS_AccessDeniedEnumText;
		case ConnectionStatus::IntermittentProblems:
			return CS_IntermittentProblemsEnumText;
		case ConnectionStatus::ServerSwitched:
			return CS_ServerSwitchedEnumText;
		case ConnectionStatus::Closed:
			return CS_ClosedEnumText;
		default:
		{
			text.set( "Unknown connection status code (", 0, false );
			text.append( statusCode );
			text.append( ")" );
			return text;
		}
	}
}

const RFA_String& OMMStrings::qosPropertyToString( QualityOfServiceRequest::StreamPropertyEnum e )
{
	switch ( e )
	{
		case QualityOfServiceRequest::DynamicStream :
			return QOS_DynamicStreamEnumText;
		case QualityOfServiceRequest::StaticStream :
			return QOS_StaticStreamEnumText;
		default:
		{
			text.set( "Unknown QualityOfServiceRequest StreamProperty (", 0, false );
			text.append( (Int32)e );
			text.append( ")" );
			return text;
		}
	}
}

const RFA_String& OMMStrings::qosRateToString( long qosRate )
{
	if ( qosRate == QualityOfService::tickByTick )
		return QOS_tickByTickEnumText;
	else if ( qosRate == QualityOfService::justInTimeFilteredRate )
		return QOS_justInTimeFilteredRateEnumText;
	else if ( qosRate == QualityOfService::unspecifiedRate )
		return QOS_unspecifiedRateEnumText;
	else
	{
		text.set( "Numeric Rate (", 0, false );
		text.append( (Int32)qosRate );
		text.append( ")" );
		return text;
	}
}

const RFA_String& OMMStrings::qosTimelinessToString( long qosTimeliness )
{
	if ( qosTimeliness == QualityOfService::realTime )
		return QOS_realTimeEnumText;
	else if ( qosTimeliness == QualityOfService::unspecifiedTimeliness )
		return QOS_unspecifiedTimelinessEnumText;
	else if ( qosTimeliness == QualityOfService::unspecifiedDelayedTimeliness )
		return QOS_unspecifiedDelayedTimelinessEnumText;
	else
	{
		text.set( "Numeric Timeliness (", 0, false );
		text.append( (Int32)qosTimeliness );
		text.append( ")" );
		return text;
	}
}

const RFA_String& OMMStrings::dataStateToString( RespStatus::DataState dataState )
{
	switch ( dataState )
	{
		case RespStatus::OkEnum:
			return RS_OkEnumText;
		case RespStatus::SuspectEnum:
			return RS_SuspectEnumText;
		case RespStatus::NoChangeEnum:
			return RS_NoChangeEnumText;
		default:
		{
			text.set( "Invalid DataState (", 0, false );
			text.append( (Int32)dataState );
			text.append( ")" );
			return text;
		}
	}
}

const RFA_String& OMMStrings::streamStateToString( RespStatus::StreamState streamState )
{
	switch ( streamState )
	{
		case RespStatus::UnspecifiedStreamStateEnum:
			return RS_UnspecifiedStreamStateEnumText;
		case RespStatus::OpenEnum:
			return RS_OpenEnumText;
		case RespStatus::NonStreamingEnum:
			return RS_NonStreamingEnumText;
		case RespStatus::ClosedRecoverEnum:
			return RS_ClosedRecoverEnumText;
		case RespStatus::ClosedEnum:
			return RS_ClosedEnumText;
		case RespStatus::RedirectedEnum:
			return RS_RedirectedEnumText;
		default:
		{
			text.set( "Invalid StreamState (", 0, false );
			text.append( (Int32)streamState );
			text.append( ")" );
			return text;
		}
	}
}

const RFA_String& OMMStrings::itemAttribInfoNameTypeToString( UInt8 type )
{
	switch ( type )
	{
		case rdm::INSTRUMENT_NAME_UNSPECIFIED:
			return AI_IN_UnspecifiedText;
		case rdm::INSTRUMENT_NAME_RIC:
			return AI_IN_RicText;
		case rdm::INSTRUMENT_NAME_MAX_RESERVED:
			return AI_IN_NameMaxReservedText;
		default:
		{
			text.set( "Unknown name type (", 0, false );
			text.append( (UInt32)type );
			text.append( ")" );
			return text;
		}
	}
}

const RFA_String& OMMStrings::loginAttribInfoNameTypeToString( UInt8 type )
{
	switch ( type )
	{
		case rdm::USER_EMAIL_ADDRESS:
			return AI_UserEmailAddressText;
		case rdm::USER_NAME:
			return AI_UserNameText;
		case rdm::USER_TOKEN:
			return AI_UserTokenText;
		default:
		{
			text.set( "Unknown name type (", 0, false );
			text.append( (UInt32)type );
			text.append( ")" );
			return text;
		}
	}
}

const RFA_String& OMMStrings::msgUpdateRespTypeNumToString( UInt8 type )
{
	switch ( type )
	{
		case rdm::INSTRUMENT_UPDATE_UNSPECIFIED:
			return UpdateType_UnspecifiedText;
		case rdm::INSTRUMENT_UPDATE_QUOTE:
			return UpdateType_QuoteText;
		case rdm::INSTRUMENT_UPDATE_TRADE:
			return UpdateType_TradeText;
		case rdm::INSTRUMENT_UPDATE_NEWS_ALERT:
			return UpdateType_NewsAlertText;
		case rdm::INSTRUMENT_UPDATE_VOLUME_ALERT:
			return UpdateType_VolumeAlertText;
		case rdm::INSTRUMENT_UPDATE_ORDER_INDICATION:
			return UpdateType_OrderIndicationText;
		case rdm::INSTRUMENT_UPDATE_CLOSING_RUN:
			return UpdateType_ClosingRunText;
		case rdm::INSTRUMENT_UPDATE_CORRECTION:
			return UpdateType_CorrectionText;
		case rdm::INSTRUMENT_UPDATE_MARKET_DIGEST:
			return UpdateType_MarketDigestText;
		case rdm::INSTRUMENT_UPDATE_QUOTES_TRADE:
			return UpdateType_QuotesTradeText;
		case rdm::INSTRUMENT_UPDATE_MULTIPLE:
			return UpdateType_MultipleText;
		case rdm::INSTRUMENT_UPDATE_VERIFY:
			return UpdateType_VerifyText;
		default:
		{
			text.set( "Unknown instrument update type (", 0, false );
			text.append( (UInt32)type );
			text.append( ")" );
			return text;
		}
	}
}

const RFA_String& OMMStrings::msgRefreshRespTypeNumToString( UInt8 type )
{
	switch ( type )
	{
		case rdm::REFRESH_SOLICITED:
			return RefreshType_SolicitedText;
		case rdm::REFRESH_UNSOLICITED:
			return RefreshType_UnsolicitedText;
		default:
		{
			text.set( "Invalid refresh respTypeNum (", 0, false );
			text.append( (UInt32)type );
			text.append( ")" );
			return text;
		}
	}
}

const RFA_String& OMMStrings::dataTypeToString( UInt8 type )
{
	switch ( type )
	{
		case VectorEnum:
			return DT_VectorText;
		case ElementListEnum:
			return DT_ElementListText;
		case FieldListEnum:
			return DT_FieldListText;
		case SeriesEnum:
			return DT_SeriesText;
		case ArrayEnum:
			return DT_ArrayText;
		case MapEnum:
			return DT_MapText;
		case FilterListEnum:
			return DT_FilterListText;
		case DataBufferEnum:
			return DT_DataBufferText;
		case DataBuffer::EnumerationEnum:
			return DT_DB_EnumerationText;
		case DataBuffer::Int32Enum:
			return DT_DB_Int32EnumText;
		case DataBuffer::UInt32Enum:
			return DT_DB_UInt32EnumText;
		case DataBuffer::IntEnum:
			return DT_DB_IntEnumText;
		case DataBuffer::UIntEnum:
			return DT_DB_UIntEnumText;
		case DataBuffer::FloatEnum:
			return DT_DB_FloatText;
		case DataBuffer::DoubleEnum:
			return DT_DB_DoubleText;
		case DataBuffer::Real32Enum:
			return DT_DB_Real32Text;
		case DataBuffer::RealEnum:
			return DT_DB_RealText;
		case DataBuffer::DateEnum:
			return DT_DB_DateText;
		case DataBuffer::TimeEnum:
			return DT_DB_TimeText;
		case DataBuffer::DateTimeEnum:
			return DT_DB_DateTimeText;
		case DataBuffer::QualityOfServiceInfoEnum:
			return DT_DB_QualityOfServiceInfoText;
		case DataBuffer::BufferEnum:
			return DT_DB_BufferText;
		case DataBuffer::StringAsciiEnum:
			return DT_DB_StringAsciiText;
		case DataBuffer::StringUTF8Enum:
			return DT_DB_StringUTF8Text;
		case DataBuffer::StringRMTESEnum:
			return DT_DB_StringRMTESText;
		case DataBuffer::OpaqueEnum:
			return DT_DB_OpaqueText;
		case DataBuffer::XMLEnum:
			return DT_DB_XMLText;
		case DataBuffer::ANSI_PageEnum:
			return DT_DB_ANSI_PageText;
		case DataBuffer::UnknownDataBufferEnum:
			return DT_DB_UnknownDataBufferText;
		case NoDataEnum:
			return DT_DB_NoDataText;
		default:
		{
			text.set( "Unknown DataType (", 0, false );
			text.append( (UInt32)type );
			text.append( ")" );
			return text;
		}
	}
}

const RFA_String& OMMStrings::mapActionToString( UInt8 action )
{
	switch ( action )
	{
		case MapEntry::Add:
			return Action_AddText;
		case MapEntry::Update:
			return Action_UpdateText;
		case MapEntry::Delete:
			return Action_DeleteText;
		default:
		{
			text.set( "Invalid map entry action (", 0, false );
			text.append( (UInt32)action );
			text.append( ")" );
			return text;
		}
	}
}

const RFA_String& OMMStrings::dictionaryVerbosityTypeToString( UInt32 dictType )
{
	switch ( dictType )
	{
		case rdm::DICTIONARY_INFO:
			return Dictionary_InfoText;
		case rdm::DICTIONARY_MINIMAL:
			return Dictionary_MinimalText;
		case rdm::DICTIONARY_NORMAL:
			return Dictionary_NormalText;
		case rdm::DICTIONARY_VERBOSE:
			return Dictionary_VerboseText;
		default:
		{
			text.set( "Invalid dictionary verbosity (", 0, false );
			text.append( dictType );
			text.append( ")" );
			return text;
		}
	}
}

const RFA_String& OMMStrings::vectorActionToString( UInt8 action )
{
	switch ( action )
	{
		case VectorEntry::Set:
			return Action_SetText;
		case VectorEntry::Update:
			return Action_UpdateText;
		case VectorEntry::Clear:
			return Action_ClearText;
		case VectorEntry::Insert:
			return Action_InsertText;
		case VectorEntry::Delete:
			return Action_DeleteText;
		default:
		{
			text.set( "Invalid vector entry action (", 0, false );
			text.append( (UInt32)action );
			text.append( ")" );
			return text;
		}
	}
}

const RFA_String& OMMStrings::filterActionToString( UInt8 action )
{
	switch ( action )
	{
		case FilterEntry::Set:
			return Action_SetText;
		case FilterEntry::Update:
			return Action_UpdateText;
		case FilterEntry::Clear:
			return Action_ClearText;
		default:
		{
			text.set( "Invalid filter entry action (", 0, false );
			text.append( (UInt32)action );
			text.append( ")" );
			return text;
		}
	}
}

const RFA_String& OMMStrings::interactionTypeTpString( UInt8 interactionType )
{
	text.clear();

	if ( interactionType & ReqMsg::InitialImageFlag )
		text.append( "InitialImageFlag" );

	if ( interactionType & ReqMsg::InterestAfterRefreshFlag )
		text.append( " | InterestAfterRefreshFlag" );

	if ( interactionType & ReqMsg::PauseFlag )
		text.append( " | PauseFlag" );

	if ( interactionType > ReqMsg::InitialImageFlag + ReqMsg::InterestAfterRefreshFlag + ReqMsg::PauseFlag )
		text.append( " | Unknown Flag" );

	return text;
}

const RFA_String& OMMStrings::eventTypeToString( int type )
{
	switch ( type )
	{
		case ConnectionEventEnum:
			return ConnectionEventText;
		case OMMItemEventEnum:
			return OMMItemEventText;
		case OMMActiveClientSessionEventEnum:
			return OMMActiveClientSessionEventText;
		case OMMInactiveClientSessionEventEnum:
			return OMMInactiveClientSessionEventText;
		case OMMSolicitedItemEventEnum:
			return OMMSolicitedItemEventText;
		case OMMCmdErrorEventEnum:
			return OMMCmdErrorEventText;
		case LoggerNotifyEventEnum :
			return LoggerNotifyEventText;
		default:
		{
			text.set( "Invalid Event Type (", 0, false );
			text.append( (Int32)type );
			text.append( ")" );
			return text;
		}
	}
}

const RFA_String OMMStrings::connectionEventToString( const rfa::sessionLayer::ConnectionEvent& event )
{
	const ConnectionStatus& connectionStatus = event.getStatus();
	const OMMConnectionEvent& connectionEvent = static_cast<const OMMConnectionEvent&>(event);

	RFA_String eventText;

	eventText.set( "Received ConnectionEvent from " );
	eventText.append( event.getConnectionName() );
	eventText.append( ". Connection is: " );
	eventText.append( ((connectionStatus.getState() == ConnectionStatus::Up) ? "Up" : "Down") );
	eventText.append( "\n\tConnection Status Text: " );
	eventText.append( connectionStatus.getStatusText() );
	eventText.append( "\n\tConnection Status Code: " );
	eventText.append( connectionStatus.getStatusCode() );
	
	ConnectionEvent::ConnectionType cType = connectionEvent.getConnectionType();

	// log information depending on the connection type
	if ( cType == ConnectionEvent::RSSL )
	{
		eventText.append( ".\n\tConnection Type: RSSL" );
		eventText.append( "\n\tConnected Host Name : " );
		eventText += connectionEvent.getConnectedHostName();
		eventText.append( "\n\tConnected Port : " );
		eventText += connectionEvent.getConnectedPort();
	}
	else if ( cType == ConnectionEvent::RSSL_PROV )
	{
		eventText.append( ".\n\tConnection Type: RSSL_PROV" );
		eventText.append( "\n\tConnected Host Name : " );
		eventText += connectionEvent.getConnectedHostName();
		eventText.append( "\n\tConnected Port : " );
		eventText += connectionEvent.getConnectedPort();
	}
	else if ( cType == ConnectionEvent::RSSL_NIPROV )
	{
		eventText.append( ".\n\tConnection Type: RSSL_NIPROV" );
		eventText.append( "\n\tConnected Host Name : " );
		eventText += connectionEvent.getConnectedHostName();
		eventText.append( "\n\tConnected Port : " );
		eventText += connectionEvent.getConnectedPort();
	}
	else if ( cType == ConnectionEvent::RSSL_NIPROV_MCAST )
	{
		eventText.append( ".\n\tConnection Type: RSSL_NIPROV_MCAST" );
		eventText.append( "\n\tConnected Recv Address : " );
		eventText += connectionEvent.getConnectedRecvAddress();
		eventText.append( "\n\tConnected Recv Port : " );
		eventText += connectionEvent.getConnectedRecvPort();
		eventText.append( "\n\tConnected Send Address : " );
		eventText += connectionEvent.getConnectedSendAddress();
		eventText.append( "\n\tConnected Send Port : " );
		eventText += connectionEvent.getConnectedSendPort();
		eventText.append( "\n\tConnected Unicast Port : " );
		eventText += connectionEvent.getConnectedUnicastPort();
	}
	else if ( cType == ConnectionEvent::RSSL_CONS_MCAST )
	{
		eventText.append( ".\n\tConnection Type: RSSL_CONS_MCAST" );
		eventText.append( "\n\tConnected Recv Address : " );
		eventText += connectionEvent.getConnectedRecvAddress();
		eventText.append( "\n\tConnected Recv Port : " );
		eventText += connectionEvent.getConnectedRecvPort();
		eventText.append( "\n\tConnected Send Address : " );
		eventText += connectionEvent.getConnectedSendAddress();
		eventText.append( "\n\tConnected Send Port : " );
		eventText += connectionEvent.getConnectedSendPort();
		eventText.append( "\n\tConnected Unicast Port : " );
		eventText += connectionEvent.getConnectedUnicastPort();
	}
	else
	{
		eventText.append( ".\n\tConnection Type: Unknown" );
	}
			
	unsigned int size = connectionEvent.getConnectedComponentVersion().size();
	if ( size > 0 )
	{
		eventText.append( "\n\tConnected Component Version : " );

		for ( unsigned int pos = 0; pos < connectionEvent.getConnectedComponentVersion().size(); ++pos )
		{
			eventText.append( "\n\t[" );
			eventText.append( (UInt32)pos );
			eventText.append( "] : " );
			eventText += connectionEvent.getConnectedComponentVersion()[pos];
		}
	}
	else
	{
		eventText.append( "\n\tThere is no connected component version info." );
	}

	eventText.append( "\n" );
	
	return eventText;
}
