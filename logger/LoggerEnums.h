#ifndef __rfa_logger_Logger_Enums_h
#define __rfa_logger_Logger_Enums_h


#include "Logger/Logger.h"

// \namespace rfa
namespace rfa {

// \namespace logger
namespace logger {


enum LoggerEventSourceType
{
	AppLoggerMonitorEnum = RFA_LOGGER_PACKAGE
};

enum LoggerInterestSpecType
{
	AppLoggerInterestSpecEnum = RFA_LOGGER_PACKAGE
};

enum LoggerEventType
{
	LoggerNotifyEventEnum = RFA_LOGGER_PACKAGE
};


} // \namespace logger
} // \namespace rfa

#endif  //  __rfa_logger_Logger_Enums_h
