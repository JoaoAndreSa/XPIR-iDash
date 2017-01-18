/**
    XPIR-hash
    Error.hpp
    Purpose: Error handler

    @author Joao Sa
    @version 1.0 18/01/17
*/

#pragma once

#include <boost/thread.hpp>

class Error{
public:
	//ERROR HANDLING
	static void error(int cond, std::string err){
		if(cond==1){std::cerr << err << "\n" << "THREAD [" << boost::this_thread::get_id() << "] EXITED WITH ERROR" << "\n"; pthread_exit(nullptr);} //For errors on server threads
	}
};