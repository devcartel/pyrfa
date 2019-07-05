#ifndef __rfa_LoggerNotifyEvent_h
#define __rfa_LoggerNotifyEvent_h

/**
    \class rfa::logger::LoggerNotifyEvent LoggerNotifyEvent.h "Logger/LoggerNotifyEvent.h"
 
    \brief
    The logger event.

    The LoggerNotifyEvent is an interface class to the wrap class of event for
    specific log event and also can be used to get the relevent log 
    information (e.g. component name, log ID, severity, and mesage text)
 
    @see    rfa::common::Event 
*/

#include "Logger/Logger.h"
#include "Common/Event.h"
#include "Logger/LoggerEnums.h"
#include "Common/RFA_String.h"
#include "Common/RFA_WString.h"

// \namespace rfa
namespace rfa {

// \namespace logger
namespace logger{

class RFA_LOGGER_API LoggerNotifyEvent : public rfa::common::Event  
{
public:

///@name Attributes
//@{
    /** Get name of the component  */
    virtual const rfa::common::RFA_String& getComponentName() const = 0;

    /** Get name of the AppLogger */
    virtual const rfa::common::RFA_String& getAppLoggerName() const = 0;

    /** Get the LogID which is the Symbolic Name in the message file entries */
    virtual long getLogID()  const = 0;

    /** Get severity, which could be Informational, Warning, Error or Success */
    virtual long getSeverity()  const = 0;

    /** Get Message text,which is input to the substitution string defined in the message file. */
    virtual const rfa::common::RFA_String& getMessageText() const   = 0;
    
#ifndef RFA_NO_WSTRING
	/** Get Message text which is convert to wide char */
    virtual const rfa::common::RFA_WString& getMessageTextW() const  = 0;
#endif // RFA_NO_WSTRING

//@}
#ifndef DOXYGEN_SKIP_PROTECTED
protected:

///@name Constructor
//@{
    LoggerNotifyEvent();
//@}

///@name Destructor
//@{
    virtual ~LoggerNotifyEvent();
//@}
#endif // DOXYGEN_SKIP_PROTECTED
}; 
} // \namespace logger
} // \namespace rfa
#endif
