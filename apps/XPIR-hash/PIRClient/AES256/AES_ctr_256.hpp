#pragma once

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <string.h>
#include <iostream>

class AES_ctr_256{
private:
	unsigned char m_key[32];
	unsigned char m_iv[16];

private:
	void pack64(uint64_t,unsigned char *);
	void pack32(uint32_t,unsigned char *);
	uint32_t unpack32(unsigned char *);
	void handleErrors(void);

public:
	AES_ctr_256(){
		if(!RAND_bytes(m_key,sizeof m_key)){ std::cout << "Random Generator Error" << "\n"; exit(1);}

		unsigned char iv[8];
		if(!RAND_bytes(iv,8)){ std::cout << "Random Generator Error" << "\n"; exit(1);}

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
	int encrypt(unsigned char*, int, unsigned char*, unsigned char*, uint64_t);
	int decrypt(unsigned char*, unsigned char*, uint64_t);
	void test(unsigned char*, int, unsigned char*, unsigned char*, unsigned char*);
};