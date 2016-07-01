/**
    XPIR-hash
    PIRServer.hpp
    Purpose: Parent class (abstract) that binds to each thread. Can have to modes of operation: Sequential or Pipeline (child classes)

    @author Joao Sa
    @version 1.0 01/07/16
*/

/**

				  PIRServer (*)
				      |
		   ----------- -----------
		   |                     |
	PIRServerSequential   PIRServerPipeline

*/

#pragma once

#include <netdb.h>
#include <boost/asio/io_service.hpp>

#include "libpir.hpp"
#include "../Constants/constants.hpp"
#include "../Socket/Socket.hpp"

class PIRServer{

protected:
	int m_connFd;
	Socket m_socket;
	boost::thread::id m_id;
	uint64_t m_num_entries;

public:
	/**
    	Constructor (super class) for PIRServer object.

    	@param connFd socket descriptor.

    	@return
	*/
	PIRServer(Socket socket){
		m_socket=socket;
		m_connFd=socket.m_connFd;
	}

	static void error(boost::thread::id tid,int cond, std::string err){
		if(cond==1){std::cerr << err << "\n" << "THREAD [" << tid << "] EXITED" << "\n"; pthread_exit(nullptr);} //For errors on server threads
	}

protected:
	char* readCiphertext(int);								//reads ciphertext from socket (does not require to add '\0' to then end)
	char* readPlaintext(int);								//reads plaintext from socket (adds a '\0' to end of the stream)
	PIRParameters readParamsPIR();							//reads PIR params needed to create an XPIRc object
	int verifyParams(uint64_t, uint64_t, unsigned int*);	//validate PIR params
	void removeDB();										//cleans DB folder to allow for another batch of files sent by the client
	void downloadData();									//reads data to be stored by the server

	void cleanupVector(vector<char*>);  					//free allocated memory
};