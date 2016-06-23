#pragma once

#include "../PIRClient.hpp"

class PIRClientParallel: public PIRClient {
private:
	XPIRcParallel* m_xpir;

public:
	PIRClientParallel(char* sname, int portNo) : PIRClient(sname,portNo){}

	void initXPIR(uint64_t);
	std::string searchQuery(uint64_t,std::map<char,std::string>);
	
	void cleanup();

private:
	void downloadWorker();
	void startProcessResult(uint64_t);
	void uploadWorker();
	void startProcessQuery(uint64_t);

	void joinAllThreads();
};