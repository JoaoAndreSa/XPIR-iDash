/**
    XPIR-hash
    PIRClient.hpp
    Purpose: Parent class (abstract) that binds to each client. Can have to modes of operation: Sequential or Pipeline (child classes).

    @author Joao Sa
    @version 1.0 01/07/16
*/

/**

                  PIRClient(*)
                      |
           ----------- -----------
           |                     |
  PIRClientSequential    PIRClientPipeline

*/

#pragma once

#include <netdb.h>

#include "AES/AES_cbc_256.hpp"
#include "SHA256/SHA_256.hpp"

#include "../XPIR/Pipeline/XPIRcPipeline.hpp"
#include "../XPIR/Sequential/XPIRcSequential.hpp"

#include "../Constants/constants.hpp"

class PIRClient{
protected:
	//Connection variables
	int m_listenFd, m_portNo;
	char* m_sname;
	struct hostent *m_server;
	struct sockaddr_in m_svrAdd;

	//Encryption and hashing variables
	AES_cbc_256 m_cbc;
	SHA_256* m_SHA_256;

	//Time variables
	double m_RTT_start;
	double m_RTT_stop;

public:
	/**
    	Constructor (super class) for PIRServer object.

    	@param sname server address name.
    	@param portNo port to connect.

    	@return
	*/
	PIRClient(char* sname, int portNo){
		srand(static_cast <unsigned int> (time(0)));
		//Start server connection
		errorExit((portNo > 65535) || (portNo < 2000),"Please enter port number between 2000 - 65535");
		
		m_portNo=portNo;
		m_sname=sname;

		createSocket();
		getServerAddress();
		connectToServer();
	}

	static void errorExit(int cond, std::string err){if(cond==1){std::cerr << err << "\n"; exit(1);}}		//For errors on main thread
	static void errorWriteSocket(int cond){if(cond==1){std::cerr << "ERROR writing to socket" << "\n";}}	//For errors while writing in socket
	static void errorReadSocket(int cond){if(cond==1){std::cerr << "ERROR reading socket"<< "\n";}}			//For errors while reading from socket

	uint64_t uploadData(std::string);				//prepares and uploads the DB data to send to the server
	void initSHA256();

	uint64_t considerPacking(uint64_t,uint64_t);	//returns the position relative to the aggregation/packing value
	virtual std::string searchQuery(uint64_t,std::map<char,std::string>)=0;	//kind of the main function of the class

	virtual void cleanup()=0;						//clean 'tools'

	//Getters and Setters
	void setRTTStart();
	double getRTTStart();
	void setRTTStop();
	double getRTTStop();

protected:
	void createSocket();											//initializes socket descriptor
	void connectToServer();											//
	void getServerAddress();										//

	void sleepForBytes(unsigned int);								//

	void sendXBytes(uint64_t, void*);								//send X amount of bytes to the socket (we convert any type to a bunch of bytes)
	void senduChar_s(unsigned char*,int);							//sends an unsigned char array to the socket;
	void sendInt(int);												//sends an integer to the socket;
	void senduInt(unsigned int);									//sends an unsigned integer to the socket;
	void senduInt32(uint32_t);										//sends an unsigned integer (32 bits) to the socket;
	void senduInt64(uint64_t);										//sends an unsigned integer (64 bits) to the socket;

	void readXBytes(uint64_t, void*);								//
	uint64_t readuInt64();											//
	uint32_t readuInt32();											//

	int compareSNPs(std::string, std::map<char,std::string>);		//
	int verifyParams(uint64_t,uint64_t,uint64_t,unsigned int*);		//
	PIRParameters readParamsPIR(uint64_t);							//
	int readParamsSHA();											//

	int symmetricEncrypt(unsigned char*,std::string);				//
	int symmetricDecrypt(unsigned char*,char*);						//
	void sendCiphertext(int,unsigned char*);						//
	void sendPlaintext(int,std::string);							//
	uint64_t sendData(std::vector<std::string>);					//
};