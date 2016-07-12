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

#include "AES256/AES_ctr_256.hpp"
#include "SHA256/SHA_256.hpp"

#include "../XPIR/Pipeline/XPIRcPipeline.hpp"
#include "../XPIR/Sequential/XPIRcSequential.hpp"

#include "../Constants/constants.hpp"
#include "../Socket/Socket.hpp"
#include "../Tools/Tools.hpp"

class PIRClient{
protected:
	//Connection variables
	Socket m_socket;

	//Encryption and hashing variables
	AES_ctr_256* m_aes_256;
	SHA_256* m_SHA_256;

	//Time variables
	double m_RTT_start;
	double m_RTT_stop;

public:
	/**
    	Constructor (super class) for PIRClient object.

    	@param socket

    	@return
	*/
	PIRClient(Socket socket){
		m_socket=socket;
	}

	uint64_t uploadData(std::string);				//prepares and uploads the DB data to send to the server
	void initAES256();
	void initSHA256();

	uint64_t considerPacking(uint64_t,uint64_t);	//returns the position relative to the aggregation/packing value
	virtual std::string searchQuery(uint64_t,std::map<char,std::string>)=0;	//kind of the main function of all PIRClient classes (children)

	//Getters and Setters
	void setRTTStart();
	double getRTTStart();
	void setRTTStop();
	double getRTTStop();

protected:
	int symmetricEncrypt(unsigned char*,std::string,uint64_t);		//symmetric encrypt plaintext and return the result
	int symmetricDecrypt(unsigned char*,char*,uint64_t);			//symmetric decrypt ciphertext and return the result
	void sendCiphertext(int,unsigned char*);						//send ciphertext through socket (length+data)
	void sendPlaintext(int,std::string);							//send plaintext through socket (length+data)
	uint64_t sendData(std::vector<std::string>);					//encrypt and send every variant in vcf file to server
};