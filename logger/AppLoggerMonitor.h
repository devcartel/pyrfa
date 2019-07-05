#ifndef	__rfa_AppLoggerMonitor_h
#define __rfa_AppLoggerMonitor_h
/**
    \class rfa::logger::AppLoggerMonitor AppLoggerMonitor.h "Logger/AppLoggerMonitor.h"
    \brief 
    Interface used to monitor and generate events.

    The AppLoggerMonitor class is an interface used to monitor and generate
    events for log messages that meet the application interest specifications. 
    It creats the logger client as the listener, event queue as the sink, and
    register the logger client.
 
    @see    rfa::logger::ApplicationLogger,
    @see    rfa::logger::AppLoggerInterestSpec,
    @see    rfa::common::EventSource,
    @see    rfa::common::Client,
    @see    rfa::common::EventQueue,
    @see    rfa::common::Handle,
    @see    rfa::common::InvalidUsageException,
*/

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4290)
#endif

#include "Logger/Logger.h"
#include "Logger/AppLoggerInterestSpec.h"
#include "Common/Handle.h"
#include "Common/Client.h"
#include "Common/EventSource.h"
#include "Logger/LoggerEnums.h"
#include "Common/InvalidUsageException.h"

// \namespace rfa
namespace rfa {

// \namespace logger
namespace logger{

class RFA_LOGGER_API AppLoggerMonitor : public rfa::common::EventSource  
{
public:

///@name Operations
//@{
    /** Method to register Logger Client. 
     *  Once the client is registered, every message will log into
     *  the queue and LoggerNotify() will be dispatched to the client
     *  for the processing through the client's processEvent() method.
		\param queue - EventQueue to which post events
		\param interestSpec - Interest Specification for registered client
		\param LoggerClient - Client callback
		\param closure - An optional pointer to user-defined data. This Closure
		is contained in each 'Event' corresponding to this interest specification.
		\return handle identifying registered event
		\remark \ref ClassLevelSafe
	*/
    virtual rfa::common::Handle* registerLoggerClient( rfa::common::EventQueue& queue,
												      AppLoggerInterestSpec& interestSpec,
												      rfa::common::Client& LoggerClient,
												      void* closure )
									throw(rfa::common::InvalidUsageException) = 0 ;

    /** Unregisters Logger client. 
     *  If the client is intestested in CompletionEvent, the CompletionEvent
     *  will be sent to the client, and the application can do the cleanup.
		\param handle - Handle pointer returned from registerClient()
		\remark \ref ClassLevelSafe
	*/
    virtual void unregisterLoggerClient( rfa::common::Handle* handle ) = 0;
//@}

#ifndef DOXYGEN_SKIP_PROTECTED
protected:
    ///@name Constructor
    //@{
	AppLoggerMonitor();
    //@}

    ///@name Destructor
    //@{
    virtual ~AppLoggerMonitor();
    //@}
#endif // DOXYGEN_SKIP_PROTECTED

};

} // \namespace logger

} // \namespace rfa

#ifdef _WIN32
#pragma warning(pop)
#endif

#endif 

