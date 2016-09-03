/**
    XPIR-hash
    AES_ctr_256.cpp
    Purpose: Handles symmetric encryption/decryption (AES-CTR256)

    @author Joao Sa
    @version 1.0 07/09/16
*/

#include "AES_ctr_256.hpp"

//***PRIVATE METHODS***//
/**
    Error handling.

    @param
    @return
*/
void AES_ctr_256::handleErrors(void){
  	ERR_print_errors_fp(stderr);
  	abort();
}

/**
    Packs a uint64 into an array of unsigned chars.
    64bits -> 8 || 8 || 8 || 8 || 8 || 8 || 8 || 8 bits

    @param val value to be stored
    @param dest destination
    @return
*/
void AES_ctr_256::pack64(uint64_t val,unsigned char *dest){
	dest[0] = (val & 0xff00000000000000) >> 56;
	dest[1] = (val & 0x00ff000000000000) >> 48;
    dest[2] = (val & 0x0000ff0000000000) >> 40;
    dest[3] = (val & 0x000000ff00000000) >> 32;
    dest[4] = (val & 0x00000000ff000000) >> 24;
    dest[5] = (val & 0x0000000000ff0000) >> 16;
    dest[6] = (val & 0x000000000000ff00) >>  8;
    dest[7] = (val & 0x00000000000000ff)      ;
}

//***PUBLIC METHODS***//
/**
    Performs AES-CTR mode encryption (using a 256bit key).

    @param plaintext
    @param plaintexlen length of the plaintext
    @param ciphertext destination container
    @param pos index of the element in the database (used to complete the IV: nonce || index)
    @return ciphertexlen length of the ciphertext
*/
int AES_ctr_256::encrypt(unsigned char *plaintext, int plaintexlen, unsigned char *ciphertext, uint64_t pos){
	EVP_CIPHER_CTX *ctx;

  	int len;

  	int ciphertexlen;

  	/* Create and initialise the context */
  	if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

	/* Initialise the encryption operation. IMPORTANT - ensure you use a key
	* and IV size appropriate for your cipher
	* In this example we are using 256 bit AES (i.e. a 256 bit key). The
	* IV size for *most* modes is the same as the block size. For AES this
	* is 128 bits */
	pack64(pos,m_iv+8);
	if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, m_key, m_iv)) handleErrors();

	/* Provide the message to be encrypted, and obtain the encrypted output.
	* EVP_EncryptUpdate can be called multiple times if necessary
	*/
	if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintexlen)) handleErrors();
	ciphertexlen = len;

	/* Finalise the encryption. Further ciphertext bytes may be written at
	* this stage.
	*/
	if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
	ciphertexlen += len;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return ciphertexlen;
}

/**
    Performs AES-CTR mode decryption (using a 256bit key).

    @param ciphertext
    @param ciphertexlen length of the ciphertext
    @param plaintext destination container
    @param pos index of the element in the database (used to complete the IV: nonce || index)
    @return plaintexlen length of the ciphertext
*/
int AES_ctr_256::decrypt(unsigned char *ciphertext, int ciphertexlen, unsigned char *plaintext, uint64_t pos){
	EVP_CIPHER_CTX *ctx;

	int len;

	int plaintexlen;

	/* Create and initialise the context */
	if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

	/* Initialise the decryption operation. IMPORTANT - ensure you use a key
	* and IV size appropriate for your cipher
	* In this example we are using 256 bit AES (i.e. a 256 bit key). The
	* IV size for *most* modes is the same as the block size. For AES this
	* is 128 bits */
	pack64(pos,m_iv+8);
	if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, m_key, m_iv)) handleErrors();

	/* Provide the message to be decrypted, and obtain the plaintext output.
	* EVP_DecryptUpdate can be called multiple times if necessary
	*/
	if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertexlen)) handleErrors();
	plaintexlen = len;

	/* Finalise the decryption. Further plaintext bytes may be written at
	* this stage.
	*/
	if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
	plaintexlen += len;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return plaintexlen;
}

/**
    Copy nonce to the IV. There is one different nonce for each vcf file. They are stored inside the data/nonces/ folder.

    @param filename name of the target vcf file
    @return
*/
void AES_ctr_256::setIV(string filename){
	unsigned char iv[8];

	if(std::ifstream("data/nonces/"+filename+".bin")){
    	Tools::readFromBinFile("data/nonces/"+filename+".bin",reinterpret_cast<char*>(iv),sizeof iv);
    }else{
    	if(!RAND_bytes(iv,8)){ std::cout << "Random Generator Error" << "\n"; exit(1);}
    	Tools::writeToBinFile("data/nonces/"+filename+".bin",reinterpret_cast<char*>(iv),sizeof iv);
    }

	//Initialise counter in 'iv' to 0
	memset(m_iv+8,0,8);

	//Copy IV into 'iv'
	memcpy(m_iv,iv,8);
}

/**
    Test encryption and decryption.

    @param plaintext
    @param plaintexlen length of the plaintext
    @param ciphertext
    @param decryptedtext
    @return
*/
void AES_ctr_256::test(unsigned char * plaintext, int plaintexlen, unsigned char *ciphertext, unsigned char *decryptedtext){
	/* Encrypt the plaintext */
	int ciphertexlen = encrypt(plaintext,strlen((char *)plaintext),ciphertext,1);

	/* Decrypt the ciphertext */
	int decryptedtexlen = decrypt(ciphertext,strlen((char *)plaintext),decryptedtext,1);

	/* Add a NULL terminator. We are expecting printable text */
	decryptedtext[decryptedtexlen] = '\0';

	/* Show the decrypted text */
	std::cout << ciphertexlen << std::endl;
	std::cout << decryptedtext << std::endl;
}