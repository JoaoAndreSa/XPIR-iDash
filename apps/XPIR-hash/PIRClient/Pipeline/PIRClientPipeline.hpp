/**
    XPIR-hash
    PIRClientPipeline.hpp
    Purpose: Child class that executes client using pipelien PIR.
       NOTE: In pipeline PIR, client sends query to the server while he generates it.
             Furthermore, he does not need to wait and get all reply elements before starting the reply extraction.

    @author  Joao Sa
    @version 1.0 18/01/17
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
public:
	/**
    	Constructor for PIRClientPipeline object.

    	@param socket (check parent class)

    	@return
	*/
	PIRClientPipeline(Socket socket) : PIRClient(socket){}

	bool searchQuery(std::map<char,std::string>);	       //main function for the class -> query variant(s)

private:
	void downloadWorker(int,XPIRcPipeline*);                            //donwload thread handler (reads reply)
	void startProcessResult(int,XPIRcPipeline*);                        //initiate reply extraction (reading and exctraction are do)
	void uploadWorker(XPIRcPipeline*,vector<char*>);                    //upload query thread handler
  void startProcessQuery(uint64_t,XPIRcPipeline*,vector<char*>);      //initiate query generation
	void joinAllThreads(vector<XPIRcPipeline*>);                        //terminate threads
};