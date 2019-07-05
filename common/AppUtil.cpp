#include "AppUtil.h"

#include <stdlib.h>

#ifdef WIN32
#include <Winsock2.h>
#else
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#endif // WIN32

#ifdef SOL_SPARC_64
#include <memory.h>	// for memcpy
#endif

using namespace rfa::common;

// This function generates the position (local machine IP/net of the program)
void getDefPosition( RFA_String& pos )
{
	char hostname[256];
	if ( gethostname( hostname, (int)sizeof( hostname ) ) )
	{
		pos.set( "127.0.0.1/net", 0, true );
		return;
	}

	struct hostent *hep;
	char * szLocalIP = 0;

#ifndef _WIN32
	struct hostent 	hostEntry;
	int				h_errnop;
	char			*hostBuff;
	int				buflen = 255;
#endif
	

#ifndef _WIN32
	hostBuff = (char*)malloc(255);

#ifndef Linux
	while ( !(hep = gethostbyname_r( hostname, &hostEntry, hostBuff, buflen, &h_errnop ) ) )
	{
		if ( errno == ERANGE )
		{
			hostBuff = (char*)realloc(hostBuff, (buflen*2));
			buflen *= 2;
		}
		else
			break;
	}

#else
	
	while ( gethostbyname_r( hostname, &hostEntry, hostBuff, buflen, &hep, &h_errnop ) )
	{
		if (h_errnop == NETDB_INTERNAL && errno == ERANGE)
		{
			hostBuff = (char*)realloc(hostBuff, (buflen*2));
			buflen *= 2;
		}
		else
			break;

	}
#endif  // end of not win32 section

#else
	hep = gethostbyname( hostname );
#endif

	if ( hep )
	{
		/* convert the address to our string */
			
		szLocalIP = inet_ntoa( *(struct in_addr *)*hep->h_addr_list );
		
		pos.set( szLocalIP, 0, true );
		pos.append( "/net" );
	
#ifndef _WIN32
		free( hostBuff );
#endif

		return;
	}

#ifndef _WIN32
	free( hostBuff );
#endif
}

RFA_String getHostStringFromNetworkBuffer( const unsigned char* buf, const int size )
{
	RFA_String str;

	// SPARC 64-bit is compiled with -xmemalign and will SIGBUS if it tries to dereference
	// a pointer on non-aligned memory.  New'ing a new buffer will ensure that it's aligned.
#ifdef SOL_SPARC_64
	unsigned char* data = new unsigned char[ size ];
	memcpy( data, buf, size );
#else
	const unsigned char* data = buf;
#endif // SOL_SPARC_64

	unsigned short source = 0;
	unsigned short dest = 0;
	// we don't handle an odd number of bytes: stop 1 byte short
	for ( int i = 0; i < size - 1; i += 2 )
	{
		// Reassemble the buffer two bytes at a time
		source = *(reinterpret_cast<const unsigned short*>( data + i ));
		dest = ntohs( source );
		str.append( (UInt16)dest );
	}

#ifdef SOL_SPARC_64
	delete [] data;
#endif

	return str;
}
