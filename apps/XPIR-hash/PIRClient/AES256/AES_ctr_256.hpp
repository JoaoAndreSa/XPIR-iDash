/**
    XPIR-hash
    AES_ctr_256.hpp
    Purpose: Handles symmetric encryption/decryption (AES-CTR256)

    @author Joao Sa
    @version 1.0 18/01/17
*/

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
	/**
    	Constructor for AES_ctr_256 object.

    	@param
    	@return
	*/
	AES_ctr_256(){
		if(std::ifstream("data/AES_key.bin")){
	    	Tools::readFromBinFile("data/AES_key.bin",reinterpret_cast<char*>(m_key),sizeof m_key);
	    }else{
	    	if(!RAND_bytes(m_key,sizeof m_key)){ std::cout << "Random Generator Error" << "\n"; exit(1);}
	    	Tools::writeToBinFile("data/AES_key.bin",reinterpret_cast<char*>(m_key),sizeof m_key);
	    }

		/* Initialise the library */
  		ERR_load_crypto_strings();
  		OpenSSL_add_all_algorithms();
  		OPENSSL_config(NULL);
	}
	/**
    	Destructor for AES_ctr_256 object.

    	@param
    	@return
	*/
	~AES_ctr_256(){
		/* Clean up */
  		EVP_cleanup();
  		ERR_free_strings();
	}

	int encrypt(unsigned char*, int, unsigned char*, uint64_t);		//encryption function
	int decrypt(unsigned char*, int, unsigned char*, uint64_t);		//decryption function
	void setIV(string);												//copy nonce to IV
	void test(unsigned char*, int, unsigned char*, unsigned char*);	//test encryption/decryption
};