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
#include <cmath>
#include <tgmath.h> 
#include "openssl/sha.h"
#include <openssl/hmac.h>
#include <openssl/rand.h>

class SHA_256{

private:
	//in hex pairs
	int HASH_SIZE;
	unsigned char m_key[32];

public:
	SHA_256(int s){
		HASH_SIZE=s;
		if(!RAND_bytes(m_key,sizeof m_key)){ std::cout << "Random Generator Error" << "\n"; exit(1);}
	};

private:
	void printElem(unsigned char*,int);

	std::string decimal_to_binary(unsigned);
	std::string type_to_binary(int,std::string,std::string);
	std::string chr_to_binary(std::string);
	std::string pos_to_binary(std::string);
	std::string base_to_binary(std::string);
	std::string hex_to_binary(std::string);
	std::string reduceSHA(unsigned char*,int);

	//void sha256(std::string, unsigned char*);
	void mac256(std::string,unsigned char*);
	std::string encoding(std::string);


public:
	uint64_t hash(std::string str);
	std::string search(std::string,std::string);
	uint64_t getSizeBits();
	void printVector(std::vector<std::string>);
	int compareSNPs(std::string,std::string);
	std::vector<std::string> tokenize(std::string,std::string);

};
