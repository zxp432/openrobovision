//
// OpenRoboVision
//
// working with network
//
//
// robocraft.ru
//

#ifndef _ORV_NETWORK_H_
#define _ORV_NETWORK_H_

#include "types.h"
#include "stream.h" // (may be removed without after-effects)
#include <string>
#include <string.h>

// in Socket.server() set SO_REUSEADDR
#define SET_SO_REUSEADDR 1

#if defined(WIN32)
# include <windows.h>

# ifdef _MSC_VER
#  pragma comment(lib, "ws2_32")
# endif

// RFC 792 define codes for ICMP

// An ICMP echo message
#ifndef ICMP_ECHO
# define ICMP_ECHO			8
#endif
// An ICMP echo reply message
#ifndef ICMP_ECHOREPLY
# define ICMP_ECHOREPLY		0
#endif

#ifndef ICMP_MINLEN
# define ICMP_MINLEN		8
#endif

#ifndef MAXHOSTNAMELEN
# define MAXHOSTNAMELEN		64
#endif

#pragma pack (push, 1)

// from /usr/include/netinet/ip.h

/*
* Definitions for internet protocol version 4.
* Per RFC 791, September 1981.
*/

/*
* Structure of an internet header, naked of options.
*/
struct ip
{
#if 1
	u_int8_t ip_vhl;           // version & header length
#else
	// linux
#if __BYTE_ORDER == __LITTLE_ENDIAN
	unsigned int ip_hl:4;		// header length
	unsigned int ip_v:4;		// version
#endif
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned int ip_v:4;		// version
	unsigned int ip_hl:4;		// header length
#endif
#endif
	u_int8_t ip_tos;			/* type of service */
	u_short ip_len;			/* total length */
	u_short ip_id;			/* identification */
	u_short ip_off;			/* fragment offset field */
#define	IP_RF 0x8000			/* reserved fragment flag */
#define	IP_DF 0x4000			/* dont fragment flag */
#define	IP_MF 0x2000			/* more fragments flag */
#define	IP_OFFMASK 0x1fff		/* mask for fragmenting bits */
	u_int8_t ip_ttl;			/* time to live */
	u_int8_t ip_p;			/* protocol */
	u_short ip_sum;			/* checksum */
	struct in_addr ip_src, ip_dst;	/* source and dest address */
};

// from /usr/include/netinet/ip_icmp.h

/*
* Internal of an ICMP Router Advertisement
*/
struct icmp_ra_addr
{
	u_int32_t ira_addr;
	u_int32_t ira_preference;
};

struct icmp
{
	u_int8_t  icmp_type;	/* type of message, see below */
	u_int8_t  icmp_code;	/* type sub code */
	u_int16_t icmp_cksum;	/* ones complement checksum of struct */
	union
	{
		u_char ih_pptr;		/* ICMP_PARAMPROB */
		struct in_addr ih_gwaddr;	/* gateway address */
		struct ih_idseq		/* echo datagram */
		{
			u_int16_t icd_id;
			u_int16_t icd_seq;
		} ih_idseq;
		u_int32_t ih_void;

		/* ICMP_UNREACH_NEEDFRAG -- Path MTU Discovery (RFC1191) */
		struct ih_pmtu
		{
			u_int16_t ipm_void;
			u_int16_t ipm_nextmtu;
		} ih_pmtu;

		struct ih_rtradv
		{
			u_int8_t irt_num_addrs;
			u_int8_t irt_wpa;
			u_int16_t irt_lifetime;
		} ih_rtradv;
	} icmp_hun;
#define	icmp_pptr	icmp_hun.ih_pptr
#define	icmp_gwaddr	icmp_hun.ih_gwaddr
#define	icmp_id		icmp_hun.ih_idseq.icd_id
#define	icmp_seq	icmp_hun.ih_idseq.icd_seq
#define	icmp_void	icmp_hun.ih_void
#define	icmp_pmvoid	icmp_hun.ih_pmtu.ipm_void
#define	icmp_nextmtu	icmp_hun.ih_pmtu.ipm_nextmtu
#define	icmp_num_addrs	icmp_hun.ih_rtradv.irt_num_addrs
#define	icmp_wpa	icmp_hun.ih_rtradv.irt_wpa
#define	icmp_lifetime	icmp_hun.ih_rtradv.irt_lifetime
	union
	{
		struct
		{
			u_int32_t its_otime;
			u_int32_t its_rtime;
			u_int32_t its_ttime;
		} id_ts;
		struct
		{
			struct ip idi_ip;
			/* options and then 64 bits of data */
		} id_ip;
		struct icmp_ra_addr id_radv;
		u_int32_t   id_mask;
		u_int8_t    id_data[1];
	} icmp_dun;
#define	icmp_otime	icmp_dun.id_ts.its_otime
#define	icmp_rtime	icmp_dun.id_ts.its_rtime
#define	icmp_ttime	icmp_dun.id_ts.its_ttime
#define	icmp_ip		icmp_dun.id_ip.idi_ip
#define	icmp_radv	icmp_dun.id_radv
#define	icmp_mask	icmp_dun.id_mask
#define	icmp_data	icmp_dun.id_data
};

#pragma pack (pop)

#elif defined(LINUX)
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>

#include <sys/param.h>
#include <sys/file.h>
#include <sys/time.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <strings.h>

#include <sys/ioctl.h>
#include <sys/select.h>

typedef int SOCKET;
typedef struct hostent HOSTENT;

# ifndef SOCKET_ERROR
#  define SOCKET_ERROR            (-1)
# endif

# ifndef INVALID_SOCKET
#  define INVALID_SOCKET  (SOCKET)(~0)
# endif

#endif //#if defined(WIN32)

namespace orv
{
	namespace network
	{
		// init network working
		int init();

		// end network working
		int end();
	}; // namespace network
}; // namespace orv

class Socket : public Stream
{
public:

	enum Protocol {TCP=1, UDP=2};
	enum Mode {CLIENT=0, SERVER};

	Socket();
	Socket(Protocol protocol_type);
	~Socket();

	int open(void);
	int close(void);

	int read(void *ptr, int count);
	int write(const void *ptr, int len);

	int available();

	int waitInput(int msec);

	// create socket
	SOCKET create(const Protocol protocol_type=TCP);
	
	// connect to IP address
	SOCKET connect(const char* ip_address, const int port);

	// create server socket
	SOCKET server(int port, Protocol protocol_type=TCP);

	// wait client connection
	int wait_connection(SOCKET& clientsocket);

	// check socket validation
	bool valid();

	//
	// get/set params
	//
	SOCKET get_socket() const { return sock; }
	void set_socket(SOCKET val) { sock = val; }
	struct sockaddr_in* get_sockaddr_in() {return &sock_addr;}
	int get_protocol() const { return protocol; }
	int get_mode() const { return mode; }
	const char* get_ip_address() const { return ip_address.c_str(); }
	void set_ip_address(const char* val) { ip_address = val; }
	int get_port() const { return ip_port; }
	void set_port(const int val) { ip_port = val; }

private:
	SOCKET sock;
	struct sockaddr_in sock_addr;
	int sock_type;
	int sock_protocol;

	Protocol protocol;
	Mode mode;

	std::string ip_address;
	int ip_port;
};

#endif //#ifndef _ORV_NETWORK_H_
