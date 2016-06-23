#pragma once

#include "../PIRClient.hpp"

class PIRClientSequential: public PIRClient {
private:
	XPIRcSequential* m_xpir;

public:
	PIRClientSequential(char* sname, int portNo) : PIRClient(sname,portNo){}

	void initXPIR(uint64_t);

	std::string searchQuery(uint64_t,std::map<char,std::string>);

	void cleanupVector(std::vector<char*>);
	void cleanup();

private:
	void sendVector_s(std::vector<char*>);
	std::vector<char*> queryGeneration(uint64_t);

	std::vector<char*> readVector_s();
	XPIRcSequential::REPLY readReply();

	char* replyExtraction(XPIRcSequential::REPLY);
	std::string extractCiphertext(char*, uint64_t, uint64_t);
	std::string extractPlaintext(char*, uint64_t, uint64_t);
};