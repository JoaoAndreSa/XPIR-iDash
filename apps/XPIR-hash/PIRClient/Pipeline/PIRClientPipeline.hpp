/**
    XPIR-hash
    PIRClientPipeline.hpp
    Purpose: Child class that executes client using pipelien PIR.
			 NOTE: In pipeline PIR, client sends query to the server while he is generating it server.
             Furthermore, he does not need to wait and get all reply elements before starting the reply extraction.

    @author Joao Sa
    @version 1.0 01/07/16
*/

/**

                  PIRClient
                      |
           ----------- -----------
           |                     |
  PIRClientSequential    PIRClientPipeline(*) 

*/

#pragma once

#include "../PIRClient.hpp"

class PIRClientPipeline: public PIRClient {
private:
	XPIRcPipeline* m_xpir;
	uint64_t m_maxFileBytesize;

public:
	/**
    	Constructor for PIRClientSequential object.

    	@param socket (check parent class)

    	@return
	*/
	PIRClientPipeline(Socket socket) : PIRClient(socket){}

	std::string searchQuery(uint64_t,std::map<char,std::string>);	//main function for the class -> query variant(s)

private:
	void downloadWorker();
	void startProcessResult();
	void uploadWorker();
	void startProcessQuery(uint64_t);

	void joinAllThreads();
};