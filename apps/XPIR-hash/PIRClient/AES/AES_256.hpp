#pragma once

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <string.h>
#include <iostream>

class AES_256{
private:
	unsigned char m_key[32];
	unsigned char m_iv[16];

	bool m_type;					//=0 CBC mode, =1 CTR mode

private:
	void pack32(uint32_t,unsigned char *);
	uint32_t unpack32(unsigned char *);
	void handleErrors(void);

public:
	AES_256(int type){
		type=m_type;
		if(!RAND_bytes(m_key,sizeof m_key)){ std::cout << "Random Generator Error" << "\n"; exit(1);}

		/* Initialise the library */
  		ERR_load_crypto_strings();
  		OpenSSL_add_all_algorithms();
  		OPENSSL_config(NULL);
	}
	~AES_256(){
		/* Clean up */
  		EVP_cleanup();
  		ERR_free_strings();
	}
	int encrypt(unsigned char*, int, unsigned char*, unsigned char*);
	int decrypt(unsigned char*, unsigned char*);
	void test(unsigned char*, int, unsigned char*, unsigned char*, unsigned char*);
};