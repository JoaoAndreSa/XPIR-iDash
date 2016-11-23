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
#include "../Tools/Tools.hpp"

class PIRServer{

protected:
	Socket m_socket;
	boost::thread::id m_id;

	int m_max_bytesize;

public:
	/**
    	Constructor (super class) for PIRServer object.

    	@param socket

    	@return
	*/
	PIRServer(Socket socket){
		m_socket=socket;
	}

protected:
	vector<char*> readRequest(uint64_t);
	void removeDB();										//cleans DB folder to allow for another batch of files sent by the client
	void downloadData();									//reads data to be stored by the server
};