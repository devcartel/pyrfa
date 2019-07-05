#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>

#ifndef _WIN32
#include <sys/time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#else
#include <windows.h>
#include <time.h>
#include <sys/timeb.h>
#endif

#include <errno.h>
#include <fcntl.h>

//###
//#define TESTME

#ifndef _REENTRANT
#define strtok_r(a,b,c) strtok(a,b)
#else 		// for now always use strtok()
#define strtok_r(a,b,c) strtok(a,b)
#endif

#ifdef _WIN32
#define strtok_r(a,b,c) strtok(a,b)
#endif

#define	MULTICAST
#define DEFAULT_MTTL 2

#ifndef IN_CLASSD
#define IN_CLASSD(X) ((((long int) (X)) & 0xf0000000) == 0xe0000000)
#endif

static int w_initialized = 0;
static char *options = 0;
static int wbuflen = 1024 * 50;
static int percentage = 0;	// (1-1000)/1000 subset of items
static char *latItemName = 0;  // explicit rather than implicit set of items

static struct sockaddr_in  dest_sock;
static struct in_addr dest_addr;
static int dest_port;

static struct sockaddr_in  my_sock;
static struct in_addr my_addr;
static int my_port;

static int wfd = -1;

unsigned long LatHash(const char *s)
{
	unsigned long hashsum = 0;
	const char *ptr = s;
	for (int cnt = strlen(s); cnt-- > 0; )
	{
		hashsum = (hashsum << 4) + (unsigned long) *ptr++;
		hashsum  ^= (hashsum >> 12);
	}
	return hashsum;
};

int LatItemNameMatch(const char* name, unsigned int percentage)
{
	if (latItemName)
		return (strcmp(name, latItemName) == 0);
	else if (percentage == 1001)
		return 1;
	else
	{
		unsigned long hashsum = LatHash(name);
		unsigned indx = (hashsum%1001)+1;
		if (indx > percentage)
			return 0;
		else
			return 1;
	}
}

static void usleep(int usec)
{
	static int nullfd = -1;
	if (nullfd == -1)
		nullfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (nullfd == -1)
		return;

	int sel;
	struct timeval ti;
	fd_set set;
	FD_ZERO(&set);
	FD_SET(nullfd, &set);
	ti.tv_sec = 0;
	ti.tv_usec = usec;
	if ((sel=select(FD_SETSIZE, &set, NULL, NULL, &ti)) < 0)
	{
		fprintf(stderr, "usleep select(s=%d, us=%ld) %d, error %s\n", ti.tv_sec,
		    ti.tv_usec,sel, strerror(errno));
	}
}

static unsigned long ipaddr(char *str)
{
	int name = 0;
	unsigned long addr;
	struct hostent *hent;
#ifndef _WIN32
	struct hostent h_ent;
	char h_buf[1024];
	int h_buflen = 1024;
	int herrno;
#endif
	char *s;
	char *e = str + strlen(str);

	for(s = str; *s != '.' && s < e; s ++)
			if (!isdigit(*s))
				name=1;

	if (name) {
//#ifdef _WIN32
#if 1
		if ((hent = gethostbyname(str) ) == 0|| hent->h_length != sizeof(addr)) {
#else
		if (((hent = gethostbyname_r(str, &h_ent, h_buf, h_buflen, &herrno) ) == 0) || 
				(hent->h_length != sizeof(addr))) {
#endif
			fprintf(stderr, "Can't resolve %s\n", str);
			exit(1);
		}
		memcpy((char *) &addr, hent->h_addr, hent->h_length);
	} else {
		addr = inet_addr(str);
	}
	return addr;
}



static int init_udp_wsocket(struct in_addr addr, int port, struct in_addr to_addr, char mttl, int buflen)
{
	int ret;
	struct sockaddr_in  from_addr;
	int on = 1;
	int s = -1;

	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s < 0)
		return s; 

	ret = setsockopt(s,SOL_SOCKET,SO_SNDBUF,(char *)&buflen,sizeof(buflen));
	if (ret != 0)
	{
		fprintf(stderr, "setsockopt() -1-  call failed: %s\n", strerror(errno));
		return -1;
	}

	if (setsockopt (s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof (on))) {
		fprintf(stderr, "setsockopt(SO_REUSEADDR) failed: %d (%s)\n", errno, strerror(errno));
		return -1;
	}


	ret = setsockopt(s,SOL_SOCKET,SO_BROADCAST,(char *)&on,sizeof(on));
	if (ret != 0)
	{
		fprintf(stderr, "setsockopt() -3-  call failed: %s\n", strerror(errno));
		return -1;
	}


	memset(&from_addr, 0, sizeof(struct sockaddr_in));
	from_addr.sin_family    = AF_INET;
	from_addr.sin_port      = htons(port);
	from_addr.sin_addr.s_addr = addr.s_addr;


	if ((ret = bind(s, (struct sockaddr *)&from_addr, sizeof(from_addr))) < 0)  {
		fprintf(stderr, "Binding error - %s .\n", strerror(errno));
		return -1;
	}
	if (IN_CLASSD(ntohl(to_addr.s_addr))) {

#ifdef MULTICAST
#ifdef _WIN32
		int xxx = mttl;
		ret = setsockopt(s, IPPROTO_IP, IP_MULTICAST_TTL, (char *) &xxx, sizeof(xxx));
#else
		ret = setsockopt(s, IPPROTO_IP, IP_MULTICAST_TTL, &mttl, sizeof(mttl));
#endif
		if (ret != 0) {
			fprintf(stderr, "setsockopt(IP_MULTICAST_TTL) returned %d (%s)\n", errno, strerror(errno));
			return -1;
		}

		ret = setsockopt(s, IPPROTO_IP, IP_MULTICAST_IF, (char *) &addr, sizeof(addr));
		if (ret != 0) {
			fprintf(stderr, "setsockopt(IP_MULTICAST_IF) returned %d (%s)\n", errno, strerror(errno));
			return -1;
		}

#else
		fprintf(stderr, "Multicast addresses not supported on this platform\n");
		return s;
#endif
	}
	return s;
}

int isNumber(char *str)
{
	char ch;
	while (ch = *str++)
	{
		if (!isdigit(ch)) return 0;
	}
	return 1;
}

int init_udp_pub(const char *config)
{
	char *fromNetAddr;
	char *toNetAddr;
	char *port_str;
	char *percentage_str;
	char *last;

	if (w_initialized)
		return wfd;

	if (options)
		delete [] options;
	options = new char[strlen(config)+1];
	strcpy(options, config);

	fromNetAddr = strtok_r(options,";", &last);
	if (fromNetAddr == 0)
	{
		fprintf(stderr,"Invalid udp publisher config\n");
		delete [] options;
		options = 0;
		return -1;
	}

	toNetAddr = strtok_r(0,";", &last);
	if (toNetAddr == 0)
	{
		fprintf(stderr,"Invalid udp publisher config\n");
		delete [] options;
		options = 0;
		return -1;
	}

	port_str = strtok_r(0,";", &last);
	if (port_str == 0)
	{
		fprintf(stderr,"Invalid udp publisher config\n");
		delete [] options;
		options = 0;
		return -1;
	}

	percentage_str = strtok_r(0,";", &last);
	if (percentage_str == 0)
	{
		fprintf(stderr,"Invalid udp publisher config\n");
		delete [] options;
		options = 0;
		return -1;
	}

	int toPort = atoi(port_str);
	if (isNumber(percentage_str))
	{
		percentage = atoi(percentage_str);
		if (percentage >= 1000)
			percentage = 1001;
	}
	else
	{
		latItemName = new char[strlen(percentage_str)+1];
		strcpy(latItemName, percentage_str);
		percentage = 0;
	}

	char mttl = DEFAULT_MTTL;

	wfd = -1;

	my_addr.s_addr = ipaddr(fromNetAddr);
	dest_addr.s_addr = ipaddr(toNetAddr);

	dest_port = toPort;
	my_port = toPort;

#ifdef _WIN32
		WSADATA	wsadata;
		(void) WSAStartup(MAKEWORD(1,1), &wsadata);
#endif

	wfd = init_udp_wsocket(my_addr, my_port, dest_addr, mttl, wbuflen);
	if (wfd == -1)
	{
		delete [] options;
		options = 0;
		return -1;
	}

	memset(&dest_sock, 0, sizeof(struct sockaddr_in));
	dest_sock.sin_family    = AF_INET;
	dest_sock.sin_port      = htons(dest_port);
	dest_sock.sin_addr.s_addr = dest_addr.s_addr;

	w_initialized = 1;

	delete [] options;

	return wfd;
}

char *udp_pub_item_name()
{
	return latItemName;
}

int write_udp_pub(const char *name, const char *pkt, int pktlen)
{
	if (!w_initialized)
		return -1;

	if (!LatItemNameMatch(name, percentage))
		return pktlen;

#ifdef TESTME
	fprintf(stdout, "%s", pkt);
#endif

	int erc = sendto(wfd, pkt, pktlen, 0,
			(struct sockaddr *)&dest_sock,sizeof(dest_sock));
	if (erc == -1) {
		fprintf(stderr,"write_udp_pub error %d (%s)\n",errno, strerror(errno));
	}
	return erc;
}

#ifdef TESTME

int main(int argc, char **argv)
{
	char name[1024] = {0};
	char buf[1024] = {0};

	char *options = argv[1];

	int fd = init_udp_pub(options);

	for (int ii = 0; ii >= 0 ; ii++)
	{
		sprintf(name, "ITEM%d", ii);
		sprintf(buf, "%d UDP Publisher Test\n", ii);
		write_udp_pub(name, buf, strlen(buf));
		//fprintf(stdout, "%s", buf);
		usleep(100000);
	}
	return 1;
}

#endif
