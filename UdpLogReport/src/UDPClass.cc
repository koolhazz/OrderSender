#include "UDPClass.h"

CUDP::CUDP()
{
	bzero(&cliAddr, sizeof(cliAddr));
}

CUDP::~CUDP()
{
}

int CUDP::UDPCreateSocket()
{
	printf("UDPCreateSocket\n");
	if((m_UDPSockfd=socket(AF_INET,SOCK_DGRAM,0)) < 0)
	{
		return -1;
	}
	return 0;
}

int CUDP::UDPConnect(const char *ip,int port)
{
	cliAddr.sin_family = AF_INET;
	cliAddr.sin_port = htons(port);
	cliAddr.sin_addr.s_addr = inet_addr(ip);
	printf("xxx ip:%s,port:%d\n",ip,port);
	/*
	if(bind(m_UDPSockfd,(struct sockaddr*)&cliAddr,sizeof(struct sockaddr))<0)
	{
		close(m_UDPSockfd);
		return -1;
	}
	*/
	return 0;
}

int CUDP::UDPSend(void * buf,int nLength)
{
	printf("buf:%s,nLength:%d\n",(char *)buf,nLength);
	int nSendLen = sendto(m_UDPSockfd,buf,nLength,0,(struct sockaddr*)&cliAddr,sizeof(cliAddr));
	printf("nSendLen:%d\n",nSendLen);
	if(nSendLen < 0)
	{
		return -1;
	}
	return 0;
}

unsigned int CUDP::GetUDPFd()
{
	return m_UDPSockfd;
}

void CUDP::CloseFd()
{
	close(m_UDPSockfd);
}
