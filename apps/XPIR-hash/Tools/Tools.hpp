/**
    XPIR-hash
    Tools.hpp
    Purpose: Class to manage a set of functions used by both client and server (e.g. read parameters, verify parameters, etc.).

    @author Joao Sa
    @version 1.0 18/01/17
*/

#pragma once

#include <stdint.h>
#include <fstream>

#include "pir/PIRParameters.hpp"
#include "../PIRServer/PIRServer.hpp"
#include "../Error/Error.hpp"

class Tools{
public:
	static vector<string> listFilesFolder(string);								//list the VCF files inside a folder
	static string readFromTextFile(string);										//read from text file
	static void writeToTextFile(string, string);								//write to text file
	static void readFromBinFile(string, char*, int);							//read from binary file
	static void writeToBinFile(string, char*, int);								//write to binary file

	static int readParamsSHA();													//reads SHA params needed for hashing (number of bits for mapping)
	static PIRParameters readParamsPIR(uint64_t);								//reads PIR params needed to create an XPIRc object

	static std::vector<std::string> tokenize(std::string,std::string);			//just like java split

	static void cleanupVector(vector<char*>);  									//free allocated memory
private:
	static int verifyParams(uint64_t, uint64_t, unsigned int*, uint64_t);		//validate PIR params

};