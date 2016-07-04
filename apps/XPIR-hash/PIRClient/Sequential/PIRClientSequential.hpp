/**
    XPIR-hash
    PIRClientSequential.hpp
    Purpose: Child class that executes client using sequential PIR.
			 NOTE: In sequential PIR, client generates the entire query and only then does he send it to the server.
             Furthermore, he has to wait and get all reply elements before starting the reply extraction.

    @author Joao Sa
    @version 1.0 01/07/16
*/

/**

                  PIRClient
                      |
           ----------- -----------
           |                     |
  PIRClientSequential(*)   PIRClientPipeline

*/

#pragma once

#include "../PIRClient.hpp"

class PIRClientSequential: public PIRClient {
private:
	XPIRcSequential* m_xpir;

public:
	/**
    	Constructor for PIRClientSequential object.

    	@param socket (check parent class)

    	@return
	*/
	PIRClientSequential(Socket socket) : PIRClient(socket){}

	std::string searchQuery(uint64_t,std::map<char,std::string>);	//main function for the class -> query variant(s)

private:
	//QUERY GENERATION & SEND QUERY
	void sendQuery(std::vector<char*>);
	std::vector<char*> queryGeneration(uint64_t);					//generate and return query

	//READ REPLY
	std::vector<char*> readReplyData();
	XPIRcSequential::REPLY readReply();								/*read all elements of reply (not just data but also the number of reply
																	  elements generated and the max file size)*/

	//REPLY EXTRACTION
	char* replyExtraction(XPIRcSequential::REPLY);
	std::string extractCiphertext(char*, uint64_t, uint64_t);		//extract the exact ciphertext (with aggregation the reply contains more than one element)
	std::string extractPlaintext(char*, uint64_t, uint64_t);		//extract the exact plaintext (with aggregation the reply contains more than one element)
};