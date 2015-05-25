#ifndef __UDP_H__
#define __UDP_H__
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string>

class CUDP
{
	private:
		struct sockaddr_in cliAddr;
		unsigned int m_UDPSockfd;
		//char buf[8192];
	public:
		CUDP();
		~CUDP();
		int UDPCreateSocket();
		int UDPConnect(const char *ip,int port);
		int UDPSend(void * buf,int nLength);
		unsigned int GetUDPFd();
		void CloseFd();
};
#endif
