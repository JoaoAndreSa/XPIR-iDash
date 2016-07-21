/**
    XPIR-hash
    Tools.hpp
    Purpose: Class to manage a set of functions used by both client and server (e.g. read parameters, verify parameters, etc.).

    @author Joao Sa
    @version 1.0 01/07/16
*/

#pragma once

#include <stdint.h>
#include <fstream>

#include <boost/thread.hpp>
#include "pir/PIRParameters.hpp"
#include "../PIRServer/PIRServer.hpp"

class Tools{
public:
	static void readFromBinFile(string, char*);
	static void writeToBinFile(string, char*, int);
	static int readParamsSHA();
	static PIRParameters readParamsPIR(uint64_t);			//reads PIR params needed to create an XPIRc object
	static void cleanupVector(vector<char*>);  					//free allocated memory
	static void error(int cond, std::string err){
		if(cond==1){std::cerr << err << "\n" << "THREAD [" << boost::this_thread::get_id() << "] EXITED" << "\n"; pthread_exit(nullptr);} //For errors on server threads
	}
private:
	static int verifyParams(uint64_t, uint64_t, unsigned int*, uint64_t);		//validate PIR params

};