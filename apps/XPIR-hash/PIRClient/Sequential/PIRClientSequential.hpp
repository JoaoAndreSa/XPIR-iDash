/**
    XPIR-hash
    PIRClientSequential.hpp
    Purpose: Child class that executes client using sequential PIR.
			 NOTE: In sequential PIR, client generates the entire query and only then does he send it to the server.
             Furthermore, he has to wait and get all reply elements before starting the reply extraction.

    @author Joao Sa
    @version 1.0 18/01/17
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
public:
	/**
    	Constructor for PIRClientSequential object.

    	@param socket (check parent class)

    	@return
	*/
	PIRClientSequential(Socket socket) : PIRClient(socket){}

	bool searchQuery(std::map<char,std::string>);						//main function for the class -> query variant(s)

private:
	//QUERY GENERATION & SEND QUERY
	void sendQuery(std::vector<char*>,XPIRcSequential*);
	std::vector<char*> queryGeneration(uint64_t,XPIRcSequential*);		//generate and return query

	//READ REPLY
	std::vector<char*> readReplyData();
	XPIRcSequential::REPLY readReply();									/*read all elements of reply (not just data but also the number of reply
																	  	elements generated and the max file size)*/

	//REPLY EXTRACTION
	char* replyExtraction(XPIRcSequential::REPLY,XPIRcSequential*);
};