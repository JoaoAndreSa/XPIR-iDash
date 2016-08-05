#pragma once

#include <boost/thread.hpp>

class Error{
public:
	//ERROR HANDLING
	static void error(int cond, std::string err){
		if(cond==1){std::cerr << err << "\n" << "THREAD [" << boost::this_thread::get_id() << "] EXITED WITH ERROR" << "\n"; pthread_exit(nullptr);} //For errors on server threads
	}
};