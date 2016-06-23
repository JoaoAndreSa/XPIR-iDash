#pragma once

#include <netdb.h>

#include "AES/AES_cbc_256.hpp"
#include "SHA256/SHA_256.hpp"

#include "../XPIR/Parallel/XPIRcParallel.hpp"
#include "../XPIR/Sequential/XPIRcSequential.hpp"

#include "../Constants/constants.hpp"

class PIRClient{
protected:
	//Connection variables
	int m_listenFd, m_portNo;
	char* m_sname;
	struct hostent *m_server;
	struct sockaddr_in m_svrAdd;
	AES_cbc_256 m_cbc;
	SHA_256* m_SHA_256;
	XPIRcSequential* m_xpir;

	//Time variables
	double m_RTT_start;
	double m_RTT_stop;

public:
	PIRClient(char* sname, int portNo){
		srand(static_cast <unsigned int> (time(0)));
		//Start Server connection
		errorExit((portNo > 65535) || (portNo < 2000),"Please enter port number between 2000 - 65535");
		
		m_portNo=portNo;
		m_sname=sname;

		createSocket();
		getServerAddress();
		connectToServer();
	}
	static void errorExit(int cond, std::string err){if(cond==1){std::cerr << err << "\n"; exit(1);}}
	static void errorWriteSocket(int cond){if(cond==1){std::cerr << "ERROR writing to socket" << "\n";}}
	static void errorReadSocket(int cond){if(cond==1){std::cerr << "ERROR reading socket"<< "\n";}}

protected:
	void createSocket();
	void connectToServer();
	void getServerAddress();

	void sleepForBytes(unsigned int);
	void sendXBytes(uint64_t, void*);
	void senduChar_s(unsigned char*,int);
	void sendInt(int);
	void senduInt(unsigned int);
	void senduInt32(uint32_t);
	void senduInt64(uint64_t);
	void sendVector_s(std::vector<char*>);
	void readXBytes(uint64_t, void*);
	uint64_t readuInt64();
	uint32_t readuInt32();
	std::vector<char*> readVector_s();
	XPIRcSequential::REPLY readReply();
	std::vector<char*> queryGeneration(uint64_t);
	char* replyExtraction(XPIRcSequential::REPLY);
	int compareSNPs(std::string, std::map<char,std::string>);
	int verifyParams(uint64_t,uint64_t,uint64_t,unsigned int*);
	PIRParameters readParamsPIR(uint64_t);
	int readParamsSHA();
	int symmetricEncrypt(unsigned char*,std::string);
	int symmetricDecrypt(unsigned char*,char*);
	void sendCiphertext(int,unsigned char*);
	void sendPlaintext(int,std::string);
	uint64_t sendData(std::vector<std::string>);
	uint64_t considerPacking(uint64_t);
	std::string extractCiphertext(char*, uint64_t, uint64_t);
	std::string extractPlaintext(char*, uint64_t, uint64_t);

public:
	uint64_t uploadData(std::string);
	void initXPIR(uint64_t);
	void initSHA256();
	std::string searchQuery(uint64_t,std::map<char,std::string>);
	void cleanupVector(std::vector<char*>);
	void cleanup();

	//Getters and Setters
	void setRTTStart();
	double getRTTStart();
	void setRTTStop();
	double getRTTStop();
};