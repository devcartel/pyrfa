#ifndef __RDM_OMMStrings_H__
#define __RDM_OMMStrings_H__

//
// OMMStrings class
//
// this class provides example code converting OMM data and message
// values (e.g., message type, domain, status, etc) to string.
//
// note: this class implementation is not thread safe.
//

#include "Common/CommonPrimitiveTypes.h"
#include "Common/QualityOfServiceRequest.h"
#include "Common/QualityOfService.h"
#include "Common/RespStatus.h"
#include "Common/RFA_String.h"
#include "SessionLayer/OMMConnectionEvent.h"

class OMMStrings
{
public :

	static const rfa::common::RFA_String& msgTypeToString( rfa::common::UInt32 mt );
	static const rfa::common::RFA_String& msgModelTypeToString( rfa::common::UInt mmt );
	static const rfa::common::RFA_String& msgRespTypeToString( rfa::common::UInt32 rt );
	static const rfa::common::RFA_String& statusCodeToString( rfa::common::UInt8 sc );
	static const rfa::common::RFA_String& qosPropertyToString( rfa::common::QualityOfServiceRequest::StreamPropertyEnum e );
	static const rfa::common::RFA_String& qosRateToString( long rate );
	static const rfa::common::RFA_String& qosTimelinessToString( long timeliness );
	static const rfa::common::RFA_String& dataStateToString( rfa::common::RespStatus::DataState data );
	static const rfa::common::RFA_String& streamStateToString( rfa::common::RespStatus::StreamState stream );
	static const rfa::common::RFA_String& itemAttribInfoNameTypeToString( rfa::common::UInt8 nameType );
	static const rfa::common::RFA_String& msgUpdateRespTypeNumToString( rfa::common::UInt8 respTypeNum );
	static const rfa::common::RFA_String& msgRefreshRespTypeNumToString( rfa::common::UInt8 respTypeNum );
	static const rfa::common::RFA_String& dataTypeToString( rfa::common::UInt8 dataType );
	static const rfa::common::RFA_String& mapActionToString( rfa::common::UInt8 action );
	static const rfa::common::RFA_String& filterActionToString( rfa::common::UInt8 action );
	static const rfa::common::RFA_String& vectorActionToString( rfa::common::UInt8 action );
	static const rfa::common::RFA_String& dictionaryVerbosityTypeToString( rfa::common::UInt32 type );
	static const rfa::common::RFA_String& connectionStatusCodeToString( rfa::common::UInt8 sc );
	static const rfa::common::RFA_String& loginAttribInfoNameTypeToString( rfa::common::UInt8 nameType );
	static const rfa::common::RFA_String& interactionTypeTpString( rfa::common::UInt8 interactionType );
	static const rfa::common::RFA_String& eventTypeToString( int type );
	static const rfa::common::RFA_String connectionEventToString( const rfa::sessionLayer::ConnectionEvent& event );

private :

	OMMStrings();
	~OMMStrings();

	OMMStrings& operator=( const OMMStrings& );
	OMMStrings( const OMMStrings& );

private :

	static rfa::common::RFA_String		text;

	static rfa::common::RFA_String		RespMsgEnumText;
	static rfa::common::RFA_String		ReqMsgEnumText;
	static rfa::common::RFA_String		GenericMsgEnumText;
	static rfa::common::RFA_String		PostMsgEnumText;
	static rfa::common::RFA_String		AckMsgEnumText;

	static rfa::common::RFA_String		MmtLoginText;
	static rfa::common::RFA_String		MmtDirectoryText;
	static rfa::common::RFA_String		MmtDictionaryText;
	static rfa::common::RFA_String		MmtMarketPriceText;
	static rfa::common::RFA_String		MmtMarketByOrderText;
	static rfa::common::RFA_String		MmtMarketByPriceText;
	static rfa::common::RFA_String		MmtMarketMakerText;
	static rfa::common::RFA_String		MmtSymbolListText;
	static rfa::common::RFA_String		MmtHistorText;
	static rfa::common::RFA_String		MmtYieldCurveText;
	static rfa::common::RFA_String		MmtContributionText;

	static rfa::common::RFA_String		RefreshEnumText;
	static rfa::common::RFA_String		StatusEnumText;
	static rfa::common::RFA_String		UpdateEnumText;

	static rfa::common::RFA_String		RS_NoneEnumText;
	static rfa::common::RFA_String		RS_NotFoundEnumText;
	static rfa::common::RFA_String		RS_TimeoutEnumText;
	static rfa::common::RFA_String		RS_NotAuthorizedEnumText;
	static rfa::common::RFA_String		RS_InvalidArgumentEnumText;
	static rfa::common::RFA_String		RS_UsageErrorEnumText;
	static rfa::common::RFA_String		RS_PreemptedEnumText;
	static rfa::common::RFA_String		RS_JustInTimeFilteringStartedEnumText;
	static rfa::common::RFA_String		RS_TickByTickResumedEnumText;
	static rfa::common::RFA_String		RS_FailoverStartedEnumText;
	static rfa::common::RFA_String		RS_FailoverCompletedEnumText;
	static rfa::common::RFA_String		RS_GapDetectedEnumText;
	static rfa::common::RFA_String		RS_NoResourcesEnumText;
	static rfa::common::RFA_String		RS_TooManyItemsEnumText;
	static rfa::common::RFA_String		RS_AlreadyOpenEnumText;
	static rfa::common::RFA_String		RS_SourceUnknownEnumText;
	static rfa::common::RFA_String		RS_NotOpenEnumText;
	static rfa::common::RFA_String		RS_NonUpdatingItemEnumText;
	static rfa::common::RFA_String		RS_UnsupportedViewTypeEnumText;
	static rfa::common::RFA_String		RS_InvalidViewEnumText;
	static rfa::common::RFA_String		RS_FullViewProvidedEnumText;
	static rfa::common::RFA_String		RS_UnableToRequestAsBatchEnumText;
	static rfa::common::RFA_String		RS_NoBatchViewSupportInReqEnumText;
	static rfa::common::RFA_String		RS_ExceededMaxMountsPerUserEnumText;	
	static rfa::common::RFA_String		RS_ErrorEnumText; 
	static rfa::common::RFA_String		RS_DacsDownEnumText;	
	static rfa::common::RFA_String		RS_UserUnknownToPermSysEnumText; 
	static rfa::common::RFA_String		RS_DacsMaxLoginsReachedEnumText; 
	static rfa::common::RFA_String		RS_DacsUserAccessToAppDeniedEnumText;	

	static rfa::common::RFA_String		RS_OkEnumText;
	static rfa::common::RFA_String		RS_SuspectEnumText;
	static rfa::common::RFA_String		RS_NoChangeEnumText;

	static rfa::common::RFA_String		RS_UnspecifiedStreamStateEnumText;
	static rfa::common::RFA_String		RS_OpenEnumText;
	static rfa::common::RFA_String		RS_NonStreamingEnumText;
	static rfa::common::RFA_String		RS_ClosedRecoverEnumText;
	static rfa::common::RFA_String		RS_ClosedEnumText;
	static rfa::common::RFA_String		RS_RedirectedEnumText;

	static rfa::common::RFA_String		CS_NoneEnumText;
	static rfa::common::RFA_String		CS_AccessDeniedEnumText;
	static rfa::common::RFA_String		CS_IntermittentProblemsEnumText;
	static rfa::common::RFA_String		CS_ServerSwitchedEnumText;
	static rfa::common::RFA_String		CS_ClosedEnumText;

	static rfa::common::RFA_String		QOS_DynamicStreamEnumText;
	static rfa::common::RFA_String		QOS_StaticStreamEnumText;

	static rfa::common::RFA_String		QOS_tickByTickEnumText;
	static rfa::common::RFA_String		QOS_justInTimeFilteredRateEnumText;
	static rfa::common::RFA_String		QOS_unspecifiedRateEnumText;

	static rfa::common::RFA_String		QOS_realTimeEnumText;
	static rfa::common::RFA_String		QOS_unspecifiedTimelinessEnumText;
	static rfa::common::RFA_String		QOS_unspecifiedDelayedTimelinessEnumText;

	static rfa::common::RFA_String		AI_IN_UnspecifiedText;
	static rfa::common::RFA_String		AI_IN_RicText;
	static rfa::common::RFA_String		AI_IN_NameMaxReservedText;

	static rfa::common::RFA_String		AI_UserEmailAddressText;
	static rfa::common::RFA_String		AI_UserNameText;
	static rfa::common::RFA_String		AI_UserTokenText;

	static rfa::common::RFA_String		UpdateType_UnspecifiedText;
	static rfa::common::RFA_String		UpdateType_QuoteText;
	static rfa::common::RFA_String		UpdateType_TradeText;
	static rfa::common::RFA_String		UpdateType_NewsAlertText;
	static rfa::common::RFA_String		UpdateType_VolumeAlertText;
	static rfa::common::RFA_String		UpdateType_OrderIndicationText;
	static rfa::common::RFA_String		UpdateType_ClosingRunText;
	static rfa::common::RFA_String		UpdateType_CorrectionText;
	static rfa::common::RFA_String		UpdateType_MarketDigestText;
	static rfa::common::RFA_String		UpdateType_QuotesTradeText;
	static rfa::common::RFA_String		UpdateType_MultipleText;
	static rfa::common::RFA_String		UpdateType_VerifyText;

	static rfa::common::RFA_String		RefreshType_SolicitedText;
	static rfa::common::RFA_String		RefreshType_UnsolicitedText;

	static rfa::common::RFA_String		DT_VectorText;
	static rfa::common::RFA_String		DT_ElementListText;
	static rfa::common::RFA_String		DT_FieldListText;
	static rfa::common::RFA_String		DT_SeriesText;
	static rfa::common::RFA_String		DT_ArrayText;
	static rfa::common::RFA_String		DT_MapText;
	static rfa::common::RFA_String		DT_FilterListText;
	static rfa::common::RFA_String		DT_DataBufferText;
	static rfa::common::RFA_String		DT_DB_EnumerationText;
	static rfa::common::RFA_String		DT_DB_Int32EnumText;
	static rfa::common::RFA_String		DT_DB_UInt32EnumText;
	static rfa::common::RFA_String		DT_DB_IntEnumText;
	static rfa::common::RFA_String		DT_DB_UIntEnumText;
	static rfa::common::RFA_String		DT_DB_FloatText;
	static rfa::common::RFA_String		DT_DB_DoubleText;
	static rfa::common::RFA_String		DT_DB_Real32Text;
	static rfa::common::RFA_String		DT_DB_RealText;
	static rfa::common::RFA_String		DT_DB_DateText;
	static rfa::common::RFA_String		DT_DB_TimeText;
	static rfa::common::RFA_String		DT_DB_DateTimeText;
	static rfa::common::RFA_String		DT_DB_QualityOfServiceInfoText;
	static rfa::common::RFA_String		DT_DB_BufferText;
	static rfa::common::RFA_String		DT_DB_StringAsciiText;
	static rfa::common::RFA_String		DT_DB_StringUTF8Text;
	static rfa::common::RFA_String		DT_DB_StringRMTESText;
	static rfa::common::RFA_String		DT_DB_OpaqueText;
	static rfa::common::RFA_String		DT_DB_XMLText;
	static rfa::common::RFA_String		DT_DB_ANSI_PageText;
	static rfa::common::RFA_String		DT_DB_UnknownDataBufferText;
	static rfa::common::RFA_String		DT_DB_NoDataText;

	static rfa::common::RFA_String		Action_AddText;
	static rfa::common::RFA_String		Action_UpdateText;
	static rfa::common::RFA_String		Action_DeleteText;
	static rfa::common::RFA_String		Action_SetText;
	static rfa::common::RFA_String		Action_ClearText;
	static rfa::common::RFA_String		Action_InsertText;

	static rfa::common::RFA_String		Dictionary_InfoText;
	static rfa::common::RFA_String		Dictionary_MinimalText;
	static rfa::common::RFA_String		Dictionary_NormalText;
	static rfa::common::RFA_String		Dictionary_VerboseText;

	static rfa::common::RFA_String		ConnectionEventText;
	static rfa::common::RFA_String		OMMItemEventText;
	static rfa::common::RFA_String		OMMActiveClientSessionEventText;
	static rfa::common::RFA_String		OMMInactiveClientSessionEventText;
	static rfa::common::RFA_String		OMMSolicitedItemEventText;
	static rfa::common::RFA_String		OMMCmdErrorEventText;
	static rfa::common::RFA_String		LoggerNotifyEventText;
};

#endif
