#ifndef _rfa_logger_h
#define _rfa_logger_h


#if defined(WIN32) && !defined(_WIN32)
	#define	_WIN32
#elif defined(_WIN32) && !defined(WIN32)
	#define WIN32
#endif

// Only supported on x86 Win32 platform
#if defined( WIN32 ) && !defined( _M_IX86 )&& !defined( _WIN64)
	#error RFA LOGGER is supported only on x86 Win32 platform
#endif

// Export macros for interfaces
// 

#ifdef WIN32
	#ifdef RFA_LOGGER_LIBRARY_EXPORTS 
		#ifdef __RFA_STATIC_BUILD__
			#define RFA_LOGGER_API 
		#else
			#define RFA_LOGGER_API		__declspec(dllexport)
		#endif
	#else
		#ifdef __RFA_STATIC_BUILD__
			#define RFA_LOGGER_API 
		#else
			#define RFA_LOGGER_API		__declspec(dllimport)
		#endif
	#endif
#else 
	#define RFA_LOGGER_API
#endif


// Make sure that WINVER and _WIN32_WINNT are set up
#ifdef WIN32
	#ifndef WINVER
		#define WINVER			0x0400
	#endif

	#ifndef _WIN32_WINNT
		#define _WIN32_WINNT	WINVER
	#endif
#endif


#ifdef WIN32
	#pragma warning (disable: 4786) //  identifier was truncated in the debug information
	#pragma warning (disable: 4800) // forcing value to bool 'true' or 'false' (performance warning)
#endif 
 

// Standard includes used internally
#include <time.h>


#include "Common/Common.h"
#include "Common/CommonEnums.h"

#define  RFA_LOGGER_PACKAGE		RFA_PACKAGE_3	

namespace rfa {
/**
	\brief The RFA Logger package includes a generic set of interfaces that is used by 
	the RFA Session Layer to log messages. An application that uses the RFA Session Layer 
	can receive the log messages produced by it. Additionally, applications can use the 
	RFA Logger package to log application-specific messages. The package also provides 
	some support for message internationalization.
*/
namespace logger {
}
}

#endif //_rfa_logger_logger_h
