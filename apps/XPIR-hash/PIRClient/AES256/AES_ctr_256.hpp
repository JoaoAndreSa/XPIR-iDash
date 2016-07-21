#pragma once

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <string.h>
#include <iostream>

#include "../../Tools/Tools.hpp"

class AES_ctr_256{
private:
	unsigned char m_key[32];
	unsigned char m_iv[16];

private:
	void pack64(uint64_t,unsigned char *);
	void handleErrors(void);

public:
	AES_ctr_256(){
		if(!RAND_bytes(m_key,sizeof m_key)){ std::cout << "Random Generator Error" << "\n"; exit(1);}

		if(std::ifstream("AES_key.bin")){
	    	Tools::readFromBinFile("AES_key.bin",reinterpret_cast<char*>(m_key));
	    }else{
	    	Tools::writeToBinFile("AES_key.bin",reinterpret_cast<char*>(m_key),sizeof m_key);
	    }

		unsigned char iv[8];
		if(!RAND_bytes(iv,8)){ std::cout << "Random Generator Error" << "\n"; exit(1);}

		if(std::ifstream("AES_nonce.bin")){
	    	Tools::readFromBinFile("AES_nonce.bin",reinterpret_cast<char*>(iv));
	    }else{
	    	Tools::writeToBinFile("AES_nonce.bin",reinterpret_cast<char*>(iv),sizeof iv);
	    }

		//Initialise counter in 'iv' to 0
		memset(m_iv+8,0,8);

		//Copy IV into 'iv'
		memcpy(m_iv,iv,8);

		/* Initialise the library */
  		ERR_load_crypto_strings();
  		OpenSSL_add_all_algorithms();
  		OPENSSL_config(NULL);
	}
	~AES_ctr_256(){
		/* Clean up */
  		EVP_cleanup();
  		ERR_free_strings();
	}
	int encrypt(unsigned char*, int, unsigned char*, uint64_t);
	int decrypt(unsigned char*, int, unsigned char*, uint64_t);
	void test(unsigned char*, int, unsigned char*, unsigned char*);
};