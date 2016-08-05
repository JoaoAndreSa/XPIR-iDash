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
	void printElem(unsigned char*,int);

	std::string data_to_binary(std::string,int,std::string);
	std::string op_to_binary(std::string,std::string);
	std::string chr_to_binary(std::string);
	std::string pos_to_binary(std::string);
	std::string base_to_binary(std::string);
	std::string decimal_to_binary(unsigned);
	std::string hex_to_binary(std::string);

	//void sha256(std::string, unsigned char*);
	void mac256(std::string,unsigned char*);

public:
	std::string uchar_to_binary(unsigned char*,int,int);
	unsigned char* binary_to_uchar(std::string);
	std::string encoding(std::string);
	string hash(std::string str);
	bool search(std::string,std::string);
	int getHashSize();
	void printVector(std::vector<std::string>);
};
