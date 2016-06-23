#pragma once

#include <iostream>
#include <netdb.h>
#include <cstring>
#include <thread>
#include <cstdlib>

#include "../Constants/constants.hpp"
#include "libpir.hpp"
#include "../XPIR/XPIRc.hpp"
#include "pir/dbhandlers/DBDirectoryProcessor.hpp"

class PIRServer{
private:
	int m_pId, m_portNo, m_listenFd, m_n_threads, m_max_connects;
    socklen_t m_len; //store size of the address
    struct sockaddr_in m_svrAdd, m_clntAdd;

public:
	PIRServer(int portNo, int n_threads, int max_connects){
		errorExit((portNo > 65535) || (portNo < 2000),"Please enter a port number between 2000 - 65535");
		m_portNo=portNo;
		m_n_threads=n_threads;
		m_max_connects=max_connects;

		createSocket();
		generateServerAddress();
		bindServer();
	}

	static void errorExit(int cond, std::string err){if(cond==1){std::cerr << err << "\n"; exit(1);}}
	static void error(int tid,int cond, std::string err){if(cond==1){std::cerr << err << "\n" << "THREAD [" << tid << "] EXITED" << "\n"; pthread_exit(nullptr);}}
	static void errorWriteSocket(int cond){if(cond==1){std::cerr << "ERROR writing to socket" << "\n";}}
	static void errorReadSocket(int cond){if(cond==1){std::cerr << "ERROR reading socket"<< "\n";}}

	struct sockaddr_in getClntAdd();
	socklen_t getLen();
	struct sockaddr_in getSvrAdd();
	int getListenFd();


private:
	void createSocket();
	void generateServerAddress();
	void bindServer();
};