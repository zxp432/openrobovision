//
// OpenRoboVision
//
// working with network
//
//
// robocraft.ru
//

#include "orv/system/network.h"

#if defined(WIN32)
static WSADATA wsadata;
#endif //#if defined(WIN32)

// init network working
int orv::network::init()
{
#if defined(WIN32)
	WSAStartup(MAKEWORD(2,0), &wsadata);
#endif //#if defined(WIN32)
	return 0;
}

// end network working
int orv::network::end()
{
#if defined(WIN32)
	WSACleanup();
#endif //#if defined(WIN32)
	return 0;
}

//
// Socket
//

Socket::Socket():
sock(SOCKET_ERROR),
ip_port(0)
{
}

Socket::Socket(Protocol protocol_type):
sock(SOCKET_ERROR),
ip_port(0)
{
	this->create(protocol_type);
}

Socket::~Socket()
{
	this->close();
}

int Socket::open(void)
{
	return this->create(protocol);
}

int Socket::close(void)
{
	if(sock != SOCKET_ERROR)
	{
		
#if defined(WIN32)
		::shutdown(sock, 2); // SD_BOTH
		::closesocket(sock);
#elif defined(LINUX)
		::shutdown(sock, SHUT_RDWR);
		::close(sock);
#endif //#if defined(WIN32)

		sock = SOCKET_ERROR;
		memset(&sock_addr, 0, sizeof(sock_addr));
		ip_address = "";
		ip_port = 0;
	}
	return 0;
}

int Socket::read(void *ptr, int count)
{
	if(!ptr || count <= 0 || sock == SOCKET_ERROR)
		return 0;

	int res = 0;
	if(protocol == TCP)
	{
#if defined(WIN32)
		res = ::recv(sock, (char*)ptr, count, 0);
#elif defined(LINUX)
		res = ::recv(sock, (char*)ptr, count, MSG_NOSIGNAL);
#endif
	}
	else if(protocol == UDP)
	{
		socklen_t fromlen = sizeof(sock_addr);
		res = ::recvfrom(sock, (char*)ptr, count, 0, (struct sockaddr *)&sock_addr, &fromlen);
	}

	return res;
}

int Socket::write(const void *ptr, int len)
{
	if(!ptr || len <= 0 || sock == SOCKET_ERROR)
		return 0;

	int res = 0;

	if(protocol == TCP)
	{
#if defined(WIN32)
		res = ::send(sock, (const char*)ptr, len, 0);
#elif defined(LINUX)
		res = ::send(sock, (const char*)ptr, len, MSG_NOSIGNAL);
#endif
	}
	else if(protocol == UDP)
	{
		res = ::sendto(sock, (const char*)ptr, len, 0, (struct sockaddr *)&sock_addr, sizeof(sock_addr));
	}

	return res;
}

int Socket::available()
{
	if(sock==SOCKET_ERROR)
		return 0;

#if defined(WIN32)

	// FIONREAD 
	//
	// Use to determine the amount of data pending in the network's input buffer that can be read from socket s. 
	// The argp parameter points to an unsigned long value in which ioctlsocket stores the result. 
	// FIONREAD returns the amount of data that can be read in a single call to the recv function, 
	// which may not be the same as the total amount of data queued on the socket. 
	// If s is message oriented (for example, type SOCK_DGRAM), FIONREAD still returns 
	// the amount of pending data in the network buffer, however, the amount that can 
	// actually be read in a single call to the recv function is limited to 
	// the data size written in the send or sendto function call.

	u_long bytes = 0;
	ioctlsocket(sock, FIONREAD, &bytes);
	return bytes;

#elif defined(LINUX)

	int bytes = 0;
	ioctl(sock, FIONREAD, &bytes);
	return bytes; 

#endif

	return 0;
}

int Socket::waitInput(int msec)
{
	if(sock==SOCKET_ERROR)
		return 0;

	fd_set rfds;
	struct timeval tv;
	tv.tv_sec = msec / 1000;
	tv.tv_usec = (msec % 1000) * 1000;

	if( tv.tv_usec > 1000000 ){
		tv.tv_sec++;
		tv.tv_usec -= 1000000;
	}

	FD_ZERO(&rfds);
	FD_SET(sock, &rfds);
	return ::select(sock+1, &rfds, NULL, NULL, &tv);
}

// create socket
SOCKET Socket::create(const Protocol protocol_type)
{
	memset(&sock_addr, 0, sizeof(sock_addr));
	ip_address = "";
	ip_port = 0;
	protocol = protocol_type;

	sock_type = SOCK_STREAM;
	sock_protocol = 0;
	switch(protocol)
	{
		case TCP:
			sock_type = SOCK_STREAM;
			sock_protocol = 0;
			break;
		case UDP:
			sock_type = SOCK_DGRAM;
			sock_protocol = IPPROTO_UDP;
			break;
		default:
			printf("[!][Socket][create] Error: unknown protocol! (set TCP)\n");
			break;
	}

	sock = ::socket(AF_INET, sock_type, sock_protocol);
    if(sock == SOCKET_ERROR)
        printf("[!][Socket][create] Error: cant create socket!\n");
	return sock;
}
	
// connect to IP address
SOCKET Socket::connect(const char* _ip_address, const int _port)
{
	if(!_ip_address || _port<=0)
	{
		printf("[!][Socket][connect] Error: bad param!\n");
		return SOCKET_ERROR;
	}
	
	if(sock == SOCKET_ERROR)
		create(protocol);

	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(_port);

	//
	// transformation of IP address from symbols into net-format
	//
	if (inet_addr(_ip_address) != INADDR_NONE)
	{
		sock_addr.sin_addr.s_addr=inet_addr(_ip_address);
	}
	else
	{
		//
		// try to get IP from domen name
		//
		
		HOSTENT *hst;
		if (hst=gethostbyname(_ip_address))
		{
			((unsigned long *)&sock_addr.sin_addr)[0]=
			  ((unsigned long **)hst->h_addr_list)[0][0];
		}
		else
		{
			printf("[!][Socket][connect] Error: invalid IP: %s !\n", _ip_address);
			return SOCKET_ERROR;
		}
	}


	if(::connect(sock, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) == SOCKET_ERROR)
	{
		printf("[!][Socket][connect] Error: cant connect: %s !\n", _ip_address);
		return SOCKET_ERROR;
	}

	ip_address = _ip_address;
	ip_port = _port;

	return sock;
}

// create server socket
SOCKET Socket::server(int _port, Protocol protocol_type)
{
	if(_port<=0)
	{
		printf("[!][Socket][create_server] Error: bad param!\n");
		return SOCKET_ERROR;
	}

	if(sock == SOCKET_ERROR || protocol != protocol_type)
	{
		this->close();
		this->create(protocol_type);
	}

#if SET_SO_REUSEADDR
	// for reuse socket address
	int optval = 1;
	if ( setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval)) )
	{
		printf("[!][Socket][server] Error: Can't set SO_REUSEADDR on socket!\n");
		return SOCKET_ERROR;
	}
#endif //#if SET_SO_REUSEADDR

	// set server address and port
	memset(&sock_addr, 0, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;					// adress family - Internet
	sock_addr.sin_port = htons( _port );			// port
	sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// without address

	// bind() want sudo
	if (::bind(sock, (const struct sockaddr *)&sock_addr, sizeof(sock_addr)) == -1)
	{
		printf("[!][Socket][server] Error: bind() failed!\n");
		return SOCKET_ERROR;
	}

	// ждём подключения
	if (::listen(sock, SOMAXCONN) == -1)
	{
		printf("[!][CSocket][server] Error: listen() failed!\n");
		return SOCKET_ERROR;
	}

	ip_port = _port;

	return sock;
}

// wait client connection
int Socket::wait_connection(SOCKET& clientsocket)
{
	if ((clientsocket = ::accept(sock, NULL, NULL)) == SOCKET_ERROR)
	{
		printf("[!][Socket][wait_connection] Error: accept() failed!\n");
		this->server(ip_port, protocol);
		return -1;
	}
	return 0;
}

// check socket validation
bool Socket::valid()
{
	if(sock == SOCKET_ERROR || sock == INVALID_SOCKET)
		return false;
	return true;
}
