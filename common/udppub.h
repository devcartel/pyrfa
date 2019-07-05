/**
 * Interface for UDP latency functions
 */

#ifndef _UDP_LATENCY_H_
#define _UDP_LATENCY_H_ 

extern int init_udp_pub(const char *config);
extern int write_udp_pub(const char *name, const char *pkt, int pktlen);
extern int LatItemNameMatch(const char* name, int percentage);

#endif  
