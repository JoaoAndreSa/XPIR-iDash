#pragma once

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <string.h>
#include <iostream>

class AES_cbc_256{
private:
	unsigned char m_key[32];
	unsigned char m_iv[16];

private:
	void pack32(uint32_t,unsigned char *);
	uint32_t unpack32(unsigned char *);
	void handleErrors(void);

public:
	AES_cbc_256(){
		if(!RAND_bytes(m_key,sizeof m_key)){ std::cout << "Random Generator Error" << "\n"; exit(1);}

		/* Initialise the library */
  		ERR_load_crypto_strings();
  		OpenSSL_add_all_algorithms();
  		OPENSSL_config(NULL);
	}
	~AES_cbc_256(){
		/* Clean up */
  		EVP_cleanup();
  		ERR_free_strings();
	}
	int encrypt(unsigned char*, int, unsigned char*, unsigned char*);
	int decrypt(unsigned char*, unsigned char*);

	void testCBC(unsigned char*, int, unsigned char*, unsigned char*, unsigned char*);
};


// #include "AES_cbc_256.hpp"

// #include <iostream>
// #include <fstream>

// using namespace std;

// int main(int argc, char* argv[]){
// 	string line;
// 	string filename="RCV000015246_100000.vcf";
// 	int pad_size=compute_padding_size(filename);

// 	ifstream f(filename);

// 	AES_cbc_256 cbc;


	
// 	if (f.is_open()){
// 		while(getline(f,line)){
// 			unsigned char ciphertext[256];
// 			unsigned char decryptedtext[256];

// 			unsigned char *plaintext = new unsigned char[line.length()+1];
// 	  		memcpy((char*)plaintext,line.c_str(),line.length()+1);

// 			cbc.testCBC(plaintext,strlen((char *)plaintext),ciphertext,decryptedtext);

// 			cout << plaintext << endl << decryptedtext << endl;

// 			delete[] plaintext;
// 		}
// 		f.close();
// 	}
// }