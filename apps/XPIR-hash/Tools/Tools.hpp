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

#include "pir/PIRParameters.hpp"
#include "../PIRServer/PIRServer.hpp"
#include "../Error/Error.hpp"

class Tools{
public:
	static string readFromTextFile(string);
	static void writeToTextFile(string, string);
	static void readFromBinFile(string, char*, int);
	static void writeToBinFile(string, char*, int);

	static int readParamsSHA();
	static PIRParameters readParamsPIR(uint64_t);			//reads PIR params needed to create an XPIRc object

	static void cleanupVector(vector<char*>);  					//free allocated memory
private:
	static int verifyParams(uint64_t, uint64_t, unsigned int*, uint64_t);		//validate PIR params

};