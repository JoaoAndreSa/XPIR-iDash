#pragma once

#include "../PIRClient.hpp"

class PIRClientParallel: public PIRClient {
private:
	XPIRcParallel* m_xpir;
	uint64_t m_maxFileBytesize;

public:
	PIRClientParallel(char* sname, int portNo) : PIRClient(sname,portNo){}
	
	std::string searchQuery(uint64_t,std::map<char,std::string>);
	
	void cleanup();

private:
	void downloadWorker();
	void startProcessResult();
	void uploadWorker();
	void startProcessQuery(uint64_t);

	void joinAllThreads();
};