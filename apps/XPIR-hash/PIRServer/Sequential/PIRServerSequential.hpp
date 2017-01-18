/**
    XPIR-hash
    PIRServerSequential.cpp
    Purpose: Child class that binds to each server thread and executes sequential PIR.
             NOTE: In sequential PIR, server has to wait and get all query elements before starting the reply generation.

    @author Joao Sa
    @version 1.0 18/01/17
*/

/**

                  PIRServer
                      |
           ----------- -----------
           |                     |
  PIRServerSequential(*)  PIRServerPipeline

*/

#pragma once

#include "../PIRServer.hpp"
#include "../../XPIR/Sequential/XPIRcSequential.hpp"

class PIRServerSequential: public PIRServer {
  std::map<string,imported_database*>* m_imported_dbs;

public:
	/**
    	Constructor for PIRServerSequential object.

    	@param socket (check parent class)

    	@return
	*/
	PIRServerSequential(Socket socket, std::map<string,imported_database*>* imported_dbs) : PIRServer(socket) {
    m_imported_dbs=imported_dbs;
  }

	void job();								                         //what the thread executes

private:
	vector<char*> readVector_s();				               //read a char* vector from socket (in other words read query array)
	void sendVector_s(vector<char*>,uint32_t);	       //send a char* vector through socket (in other words send reply data)
	void sendReply(XPIRcSequential::REPLY,uint32_t);	 //sends all reply parameters: data, nbRepliesGenerated, maxFileSize)
};