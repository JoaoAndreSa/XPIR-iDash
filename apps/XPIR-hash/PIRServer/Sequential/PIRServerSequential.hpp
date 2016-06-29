#pragma once

#include "../PIRServer.hpp"
#include "../../XPIR/Sequential/XPIRcSequential.hpp"

class PIRServerSequential: public PIRServer {
	XPIRcSequential* m_xpir;

public:
	PIRServerSequential(int connFd, uint64_t id) : PIRServer(connFd, id) {}
	
	void job();

private:
	vector<char*> readVector_s();

	void sendVector_s(vector<char*>, uint32_t);
	void sendReply(XPIRcSequential::REPLY, uint32_t);

};