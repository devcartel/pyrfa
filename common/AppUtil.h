#ifndef __APPUTIL__H__
#define __APPUTIL__H__

#include "Common/RFA_String.h"

// This function generates the position (local machine IP/ProcessId of the program)
void getDefPosition( rfa::common::RFA_String& pos);

// Return a string of data in host byte order from a buffer in network byte order
rfa::common::RFA_String getHostStringFromNetworkBuffer( const unsigned char* buf, const int size );

#endif // __APPUTIL__H__
