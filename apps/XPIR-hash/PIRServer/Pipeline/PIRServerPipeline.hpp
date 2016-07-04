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
	boost::thread m_upThread;		  //thread for uploading reply
	boost::thread m_downThread;		//thread for downloading query
	XPIRcPipeline* m_xpir;

public:
	/**
    	Constructor for PIRServerPipeline object.

    	@param socket (check parent class)

    	@return
	*/
	PIRServerPipeline(Socket socket) : PIRServer(socket) {}
	
	void job();             //what the thread executes

private:
	void uploadWorker();   //send PIR's result, asynchronously
	void downloadWorker(); //receive queries n messages with n = nbr of files.
};