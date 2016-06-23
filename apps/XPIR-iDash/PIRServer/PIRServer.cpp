#include "PIRServer.hpp"

//***PRIVATE METHODS***//
void PIRServer::createSocket(){
	m_listenFd = socket(AF_INET, SOCK_STREAM, 0);
	errorExit(m_listenFd<0,"Cannot open socket");
}

void PIRServer::generateServerAddress(){
	bzero((char*) &m_svrAdd, sizeof(m_svrAdd));
	m_svrAdd.sin_family = AF_INET;
	m_svrAdd.sin_addr.s_addr = INADDR_ANY;
	m_svrAdd.sin_port = htons(m_portNo);
}

void PIRServer::bindServer(){
	int n;
	n=::bind(m_listenFd, (struct sockaddr *)&m_svrAdd, sizeof(m_svrAdd));
	errorExit(n<0,"Cannot bind");

	n=listen(m_listenFd,m_max_connects);
	errorExit(n<0,"Error listening");
	m_len = sizeof(m_clntAdd);
}


//***PUBLIC METHODS***//
struct sockaddr_in PIRServer::getClntAdd(){return m_clntAdd;}
socklen_t PIRServer::getLen(){return m_len;}
struct sockaddr_in PIRServer::getSvrAdd(){return m_svrAdd;}
int PIRServer::getListenFd(){return m_listenFd;}