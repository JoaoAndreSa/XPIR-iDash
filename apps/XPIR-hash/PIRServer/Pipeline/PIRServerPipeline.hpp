/**
    XPIR-hash
    PIRServerPipeline.hpp
    Purpose: Child class that binds to each server thread and executes pipeline PIR.
    		     NOTE: In pipeline PIR, server does not have to wait for all query elements to start the reply generation.

    @author Joao Sa
    @version 1.0 01/07/16
*/

/**

                  PIRServer
                      |
           ----------- -----------
           |                     |
  PIRServerSequential     PIRServerPipeline(*)

*/

#pragma once

#include "../PIRServer.hpp"
#include "../../XPIR/Pipeline/XPIRcPipeline.hpp"

class PIRServerPipeline: public PIRServer {

private:
  std::map<string,imported_database_t>* m_imported_dbs;

public:
	/**
    	Constructor for PIRServerPipeline object.

    	@param socket (check parent class)

    	@return
	*/
	PIRServerPipeline(Socket socket, std::map<string,imported_database_t>* imported_dbs) : PIRServer(socket) {
    m_imported_dbs=imported_dbs;
  }

	void job();             //what the thread executes

private:
	void uploadWorker(XPIRcPipeline*);   //send PIR's result, asynchronously
	void downloadWorker(XPIRcPipeline*); //receive queries n messages with n = nbr of files.
};