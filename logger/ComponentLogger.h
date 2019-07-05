#ifndef __rfa_ComponentLogger_h
#define __rfa_ComponentLogger_h

/**
    \class rfa::logger::ComponentLogger ComponentLogger.h "Logger/ComponentLogger.h"
 
    \brief
    The ComponentLogger class is an interface used to log received log messages to 
    either an NT Event Logger or Logger client. 

    The ComponentLogger class is an interface used to log received log messages to 
    either an NT Event Logger or Logger client. 

	The maximum length of a log message is 8192 bytes.

    @see rfa::common::CommonErrorSeverityType
*/

#include "Logger/Logger.h"
#include "Common/RFA_String.h"

// \namespace rfa
namespace rfa {

// \namespace logger
namespace logger{

class RFA_LOGGER_API ComponentLogger
{
public:

///@name Operations
//@{
    /** Method to destroy  ComponentLogger
		\warning After calling destroy() application must not refer to this instance of ComponentLogger
		\remark \ref ObjectLevelSafe
	*/
    virtual void destroy() = 0;
    
    /** Method used by application to create the log messages
		\remark \ref ObjectLevelSafe
	*/
    virtual bool log( long logID, rfa::common::CommonErrorSeverityType severity, 
                            const char* arg1=0, 
                            const char* arg2=0,
                            const char* arg3=0,
                            const char* arg4=0,
                            const char* arg5=0,
                            const char* arg6=0,
                            const char* arg7=0,
                            const char* arg8=0,
                            const char* arg9=0,
                            const char* arg10=0 ) = 0;
//@}

///@name Attributes
//@{
    /** Get the ComponentLogger name
	*/
    virtual const rfa::common::RFA_String& getName() const = 0;
//@}

#ifndef DOXYGEN_SKIP_PROTECTED
protected:
    ///@name Constructor
    //@{
        ComponentLogger();
    //@}

    ///@name Destructor
    //@{
        virtual ~ComponentLogger();
    //@}
#endif // DOXYGEN_SKIP_PROTECTED

public:

///@name Operations
    /** Method used by application to create the log messages
		\remark \ref ObjectLevelSafe
	*/
    virtual bool logEx( long logID, 
                        bool logging, rfa::common::RFA_String& formattedMsg, 
                        rfa::common::CommonErrorSeverityType severity, 
                        const char* arg1=0, 
                        const char* arg2=0,
                        const char* arg3=0,
                        const char* arg4=0,
                        const char* arg5=0,
                        const char* arg6=0,
                        const char* arg7=0,
                        const char* arg8=0,
                        const char* arg9=0,
                        const char* arg10=0 ) = 0;
//@}

}; //end of class ComponentLogger

} // \namespace logger

} // \namespace rfa

#endif // __rfa_ComponentLogger_h
 
