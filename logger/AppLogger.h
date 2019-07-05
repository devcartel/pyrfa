#ifndef __rfa_AppLogger_h
#define __rfa_AppLogger_h

/**
    \class rfa::logger::ApplicationLogger AppLogger.h "Logger/AppLogger.h"
 
    \brief
    ApplicationLogger is used to log logevents to either an NT Event Logger
	or a Logger Client. 

    ApplicationLogger is used to log logevents to either an NT Event Logger
	or Logger Client. This class is an interface to provide logging infomation
	to application.  ApplicationLogger is a shareble component. It is used to
    create ComponentLogger and AppLoggerMonitor. Logger Clients register
	themselves with ApplicationLogger and receive callbacks whenever a log event
	occurs.
    
    @see    rfa::logger::ComponentLogger
    @see    rfa::logger::AppLoggerMonitor
    @see    rfa::common::InvalidUsageException
    @see    rfa::common::InvalidConfigurationException
*/

#include "Logger/Logger.h"
#include "Logger/ComponentLogger.h"
#include "Common/LogMsgMap.h"
#include "Common/InvalidConfigurationException.h"
#include "Common/InvalidUsageException.h"
#include "Common/RFA_String.h"

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4290)
#endif

namespace rfa {

	namespace logger{

class AppLoggerMonitor;

class RFA_LOGGER_API ApplicationLogger 
{
public:

///@name Operations
//@{
	/** Method to acquire ApplicationLogger with a given name.
     *  If an instance of ApplicationLogger with the same name already exists,
	 *  a pointer to it is returned; otherwise, a new ApplicationLogger is created.
		\param rcName - a string identifying the acquired object
		\return pointer to an instance of ApplicationLogger
		\remark \ref ClassLevelSafe
    */
	static ApplicationLogger* acquire( const rfa::common::RFA_String& rcName )
								throw( rfa::common::InvalidUsageException,
										rfa::common::InvalidConfigurationException );

    /** Method to release ApplicationLogger. Once the last reference
     *  is released, the ApplicationLogger will be destroyed.
		\remark \ref ClassLevelSafe and \ref ObjectLevelSafe
	 */
    virtual void release() = 0;

    /** Method to create an instance of ComponentLogger
		\param componentName - The name of the component.
		\param pLMM - Pointer to a LogMsgMap that stores a map of log messages.
		\return pointer to created ComponentLogger instance
		\remark \ref ObjectLevelSafe
    */
	virtual ComponentLogger* createComponentLogger( const rfa::common::RFA_String& componentName,
													rfa::common::LogMsgMap* pLMM = 0 )
							throw( rfa::common::InvalidUsageException,
									rfa::common::InvalidConfigurationException ) = 0;

    /** Method to create an instance of AppLoggerMonitor
		\param name - The name of the Monitor gives the identity to the event source.
        \param withCompletionEvent - Should completion events be generated.
		\return pointer to created AppLoggerMonitor
		\remark \ref ObjectLevelSafe
    */
	virtual AppLoggerMonitor* createApplicationLoggerMonitor( const rfa::common::RFA_String& name,
															bool withCompletionEvent )
								throw(rfa::common::InvalidUsageException) = 0;
//@}

///@name Attributes
//@{
    /** Get the ApplictionLogger name */
    virtual const rfa::common::RFA_String& getName() = 0;
//@}

#ifndef DOXYGEN_SKIP_PROTECTED
protected:

///@name Constructor
//@{
    ApplicationLogger();
//@}

///@name Destructor
//@{
    virtual ~ApplicationLogger();
//@}
#endif // DOXYGEN_SKIP_PROTECTED

private:
	// Don't implement
    ApplicationLogger(const ApplicationLogger&);
    ApplicationLogger& operator=(const ApplicationLogger&);

}; // end class ApplicationLogger

} // \namespace logger

} // \namespace rfa

#ifdef _WIN32
#pragma warning(pop)
#endif

#endif // __rfa_AppLogger_h

