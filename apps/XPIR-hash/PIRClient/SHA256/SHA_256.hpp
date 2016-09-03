/**
    XPIR-hash
    SHA_256.hpp
    Purpose: Handles hashing (HMAC-256) and encoding.

    @author Joao Sa
    @version 1.0 07/09/16
*/

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <iterator>
#include <algorithm>
#include <bitset>
#include <limits>
#include <cmath>
#include <tgmath.h>
#include "openssl/sha.h"
#include <openssl/hmac.h>
#include <openssl/rand.h>

#include "../../Tools/Tools.hpp"
#include "../../Constants/constants.hpp"

class SHA_256{

private:
	//in hex pairs
	int HASH_SIZE;
	int DATA_SIZE;
	unsigned char m_key[32];

public:
	/**
    	Constructor for SHA_256 object.

    	@param s number of bits to be extracted from the hash and mapped to a specific position

    	@return
	*/
	SHA_256(int s){
		HASH_SIZE=s;
		DATA_SIZE=Constants::data_hash_size;

	    if(std::ifstream("data/HMAC_key.bin")){
	    	Tools::readFromBinFile("data/HMAC_key.bin",reinterpret_cast<char*>(m_key),sizeof m_key);
	    }else{
	    	if(!RAND_bytes(m_key,sizeof m_key)){ std::cout << "Random Generator Error" << "\n"; exit(1);}
	    	Tools::writeToBinFile("data/HMAC_key.bin",reinterpret_cast<char*>(m_key),sizeof m_key);
	    }
	};

private:
	void printElem(unsigned char*,int);							//Print char array in hexadecimal format

	std::string data_to_binary(std::string,int,std::string);	//Encodes metadata depending on the type of variant
	std::string op_to_binary(std::string,std::string);			//Encodes variant type
	std::string chr_to_binary(std::string);						//Encodes chromosome (5bits)
	std::string pos_to_binary(std::string);						//Encodes position (28bits)
	std::string base_to_binary(std::string);					//Encodes nucleotids
	std::string decimal_to_binary(unsigned);					//Converts decimal value to binary string
	std::string hex_to_binary(std::string);						//Converts hex string to binary string

	//void sha256(std::string, unsigned char*);
	void mac256(std::string,unsigned char*);					//Perform HMAC-SHA256

public:
	std::string uchar_to_binary(unsigned char*,int,int);		//ASCII to binary string
	unsigned char* binary_to_uchar(std::string);				//Binary string to ASCII
	std::string encoding(std::string);							//Encodes a variant
	string hash(std::string str);								//Performs HMAC-SHA256 and extracts DATA_SIZE number of bits
	bool search(std::string,std::string);						//Looks for a variant in a list of variants
	int getHashSize();
	void printVector(std::vector<std::string>);
};
