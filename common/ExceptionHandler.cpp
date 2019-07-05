#ifdef WIN32
#pragma warning( disable : 4786)
#endif

/**
 *  This section of code is a basic exception handler that is 
 *  shared between sessionLayer examples.
 * 
 *  This function is called by the example catch{} clause
 *  When executed, this will output the exception information
 *  and exit.
 **/

#include <iostream>
#include "Common/SystemException.h"			
#include "Common/GeneralExceptionStatus.h"
#include "Common/InvalidConfigurationException.h"
#include "Common/InvalidUsageException.h"
#include "Common/OutOfRangeException.h"

using namespace rfa::common;
using namespace std;

extern "C" void checkException( Exception& e )
{
	// Determine exception Severity
	RFA_String excpSeverityStr;
	switch ( e.getServerity() )
	{
		case Exception::Error:
			excpSeverityStr = RFA_String( "Error", 0, false );
			break;
		case Exception::Warning:
			excpSeverityStr = RFA_String( "Warning", 0, false );
			break;
		case Exception::Information:
			excpSeverityStr = RFA_String( "Information", 0, false );
			break;
		default:
			excpSeverityStr = RFA_String( "UNKNOWN Severity", 0, false );
			break;
	}

	// Determine exception Classification
	RFA_String excpClassificationStr;
	switch ( e.getClassification() )
	{
		case Exception::Anticipated:
			excpClassificationStr = RFA_String( "Anticipated", 0, false );
			break;
		case Exception::Internal:
			excpClassificationStr = RFA_String( "Internal", 0, false );
			break;
		case Exception::External:
			excpClassificationStr = RFA_String( "External", 0, false );
			break;
		case Exception::IncorrectAPIUsage:
			excpClassificationStr = RFA_String( "IncorrectAPIUsage", 0, false );
			break;
		case Exception::ConfigurationError:
			excpClassificationStr = RFA_String( "ConfigurationError", 0, false );
			break;
		default:
			excpClassificationStr = RFA_String( "UNKNOWN Classification", 0, false );
			break;
	}

	// Determine exception Type
	RFA_String excpTypeStr;
	RFA_String excpStatusText;
	RFA_String	excpDetails;

	switch ( e.getErrorType() )
	{
		case Exception::InvalidUsageException:
			{
			excpTypeStr = RFA_String( "InvalidUsageException", 0, false );
			InvalidUsageException& actualException = static_cast<InvalidUsageException&>(e);
			excpStatusText = actualException.getStatus().getStatusText();
			break;
			}
		case Exception::InvalidConfigurationException:
			{
			excpTypeStr = RFA_String( "InvalidConfigurationException", 0, false );
			InvalidConfigurationException& actualException = static_cast<InvalidConfigurationException&>(e);
			excpStatusText = actualException.getStatus().getStatusText();
			excpDetails = actualException.getParameterName() + RFA_String( " ", 0, false );
			excpDetails += actualException.getParameterValue();
			break;
			}
#ifdef WIN32
		case Exception::SystemException:
			{
			excpTypeStr = RFA_String( "SystemException", 0, false );
			SystemException& actualException = static_cast<SystemException&>(e);
			excpStatusText = actualException.getStatus().getStatusText();
			break;
			}
#endif
		default:
			{
			excpStatusText = RFA_String( "Unknown Exception Type!", 0, false );
			break;
			}
	}

	// output exception information
	RFA_String tmpstr;
	tmpstr = RFA_String( "AN EXCEPTION HAS BEEN THROWN!  The following information describes the exception:", 0, false );
	tmpstr.append("\n");
	tmpstr.append("Exception Type: ");
	tmpstr.append(excpTypeStr);
	tmpstr.append("\n");
	tmpstr.append("Exception Severity: ");
	tmpstr.append(excpSeverityStr);
	tmpstr.append("\n");
	tmpstr.append("Exception Classification: ");
	tmpstr.append(excpClassificationStr);
	tmpstr.append("\n");
	tmpstr.append("Exception Status Text: ");
	tmpstr.append(excpStatusText);
	tmpstr.append("\n");
	if ( !excpDetails.empty() )
	{
		tmpstr.append(excpDetails);
		tmpstr.append("\n");
	}

#ifdef WIN32
	MessageBox( NULL, tmpstr.c_str(), "Exception Error", MB_APPLMODAL | MB_SETFOREGROUND | MB_ICONERROR | MB_OK );
#else
	cout << endl << endl;
	cout << tmpstr.c_str();
#endif
}

