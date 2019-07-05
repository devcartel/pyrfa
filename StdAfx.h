// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently
#pragma once

#include <iostream>
//#include <tchar.h>
#include <cassert>
#include <string>

// TODO: reference additional headers your program requires here
#include "Common/Client.h"
#include "Common/Common.h"
#include "Common/Data.h"
#include "Common/EventQueue.h"
#include "Common/CommonEnums.h"
#include "Common/Context.h"
#include "Common/LogMsgMap.h"
#include "Common/QualityOfService.h"
#include "Common/QualityOfServiceInfo.h"
#include "Common/RespStatus.h"
#include "Common/RMTESConverter.h"
#include "Common/RFA_String.h"

#include "Logger/AppLogger.h"
#include "Logger/LoggerNotifyEvent.h"
#include "Logger/ComponentLogger.h"
#include "Logger/AppLoggerMonitor.h"
#include "Logger/AppLoggerInterestSpec.h"

#include "logger/ClientLog.h"
#include "logger/LogMessages.h"
#include "logger/LogMsgMapImpl.h"

#include "RDM/RDM.h"

#include "Config/ConfigDatabase.h"
#include "Config/StagingConfigDatabase.h"
#include "Config/ConfigTree.h"
#include "Config/ConfigNode.h"
#include "Config/ConfigNodeIterator.h"


#include "Data/Array.h"
#include "Data/ArrayEntry.h"
#include "Data/ArrayReadIterator.h"
#include "Data/ArrayWriteIterator.h"
#include "Data/DataDefWriteIterator.h"
#include "Data/ElementList.h"
#include "Data/ElementListReadIterator.h"
#include "Data/ElementListWriteIterator.h"
#include "Data/ElementEntry.h"
#include "Data/ElementList.h"
#include "Data/FieldList.h"
#include "Data/FieldEntry.h"
#include "Data/FieldListReadIterator.h"
#include "Data/FilterList.h"
#include "Data/FilterEntry.h"
#include "Data/FilterListReadIterator.h"
#include "Data/Map.h"
#include "Data/MapEntry.h"
#include "Data/MapReadIterator.h"
#include "Data/MapWriteIterator.h"
#include "Data/PartialUpdateReadIterator.h"
#include "Data/Series.h"
#include "Data/SeriesEntry.h"
#include "Data/SeriesReadIterator.h"
#include "Data/SeriesWriteIterator.h"

#include "Message/AttribInfo.h"
#include "Message/Manifest.h"
#include "Message/ReqMsg.h"
#include "Message/RespMsg.h"
#include "Message/PostMsg.h"
#include "Message/AckMsg.h"

#include "SessionLayer/OMMConsumer.h"
#include "SessionLayer/OMMConnectionIntSpec.h"
#include "SessionLayer/OMMProvider.h"
#include "SessionLayer/OMMListenerConnectionIntSpec.h"
#include "SessionLayer/OMMClientSessionListenerIntSpec.h"
#include "SessionLayer/OMMErrorIntSpec.h"
#include "SessionLayer/OMMActiveClientSessionEvent.h"
#include "SessionLayer/OMMInactiveClientSessionEvent.h"
#include "SessionLayer/OMMItemEvent.h"
#include "SessionLayer/OMMItemIntSpec.h"
#include "SessionLayer/OMMItemCmd.h"
#include "SessionLayer/OMMSolicitedItemEvent.h"
#include "SessionLayer/OMMSolicitedItemCmd.h"
#include "SessionLayer/OMMCmdErrorEvent.h"
#include "SessionLayer/ClientSessionStatus.h"
#include "SessionLayer/OMMClientSessionCmd.h"
#include "SessionLayer/OMMClientSessionIntSpec.h"
#include "SessionLayer/RequestToken.h"
#include "SessionLayer/ConnectionEvent.h"
#include "SessionLayer/Session.h"
#include "SessionLayer/OMMHandleItemCmd.h"

#ifdef WIN32
#include <sys/timeb.h>
#include <math.h>  //pow()
#define STRICMP(X,Y) _stricmp(X,Y)
#else
#include <strings.h>
#include <sys/time.h>
#define STRICMP(X,Y) strcasecmp(X,Y)
#endif

#include "TS1/TS1Series.h"
#include "TS1/TS1DefDb.h"
#include "TS1/TS1NameIterator.h"
#include "TS1/TS1Sample.h"
#include "TS1/TS1Fields.h"
#include "TS1/TS1SampleIterator.h"
#include "TS1/TS1EventIterator.h"

#ifdef _ASSERTE
#undef _ASSERTE
#endif

#define _ASSERTE    assert

using namespace std;
using namespace rfa::common;

