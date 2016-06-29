#pragma once

#include "../PIRServer.hpp"
#include "../../XPIR/Parallel/XPIRcParallel.hpp"

class PIRServerParallel: public PIRServer {

private:
	boost::thread m_upThread;
	boost::thread m_downThread;
	XPIRcParallel* m_xpir;

public:
	PIRServerParallel(int connFd, uint64_t id) : PIRServer(connFd, id) {}
	
	void job();

private:
	void uploadWorker();
	void downloadWorker();
};