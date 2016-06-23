#pragma once

#include <netdb.h>
#include "libpir.hpp"
#include "../Constants/constants.hpp"

class PIRServer{

protected:
	int m_connFd;
	uint64_t m_id;
	uint64_t m_num_entries;

public:
	PIRServer(int connFd, uint64_t id){
		m_connFd=connFd;
		m_id=id;
	}

	static void errorExit(int cond, std::string err){if(cond==1){std::cerr << err << "\n"; exit(1);}}
	static void error(int tid,int cond, std::string err){if(cond==1){std::cerr << err << "\n" << "THREAD [" << tid << "] EXITED" << "\n"; pthread_exit(nullptr);}}
	static void errorWriteSocket(int cond){if(cond==1){std::cerr << "ERROR writing to socket" << "\n";}}
	static void errorReadSocket(int cond){if(cond==1){std::cerr << "ERROR reading socket"<< "\n";}}

protected:
	void readXBytes(uint64_t, void*);
	int readInt();
	unsigned int readuInt();
	uint32_t readuInt32();
	uint64_t readuInt64();

	char* readCiphertext(int);
	char* readPlaintext(int);
	PIRParameters readParamsPIR();
	int verifyParams(uint64_t, uint64_t, unsigned int*);
	void removeDB();
	void downloadData();

	void sleepForBytes(unsigned int);
	void sendXBytes(uint64_t,void*);
	void senduInt64(uint64_t);
	void senduInt32(uint32_t);

	void cleanupVector(vector<char*>);
};