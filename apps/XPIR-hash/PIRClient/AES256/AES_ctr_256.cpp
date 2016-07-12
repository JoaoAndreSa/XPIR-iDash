#include "AES_ctr_256.hpp"

//***PRIVATE METHODS***//
void AES_ctr_256::handleErrors(void){
  	ERR_print_errors_fp(stderr);
  	abort();
}

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

void AES_ctr_256::pack32(uint32_t val,unsigned char *dest){
    dest[0] = (val & 0xff000000) >> 24;
    dest[1] = (val & 0x00ff0000) >> 16;
    dest[2] = (val & 0x0000ff00) >>  8;
    dest[3] = (val & 0x000000ff)      ;
}


uint32_t AES_ctr_256::unpack32(unsigned char *src){
    uint32_t val;

    val  = src[0] << 24;
    val |= src[1] << 16;
    val |= src[2] <<  8;
    val |= src[3]      ;

    return val;
}

//***PUBLIC METHODS***//
int AES_ctr_256::encrypt(unsigned char *plaintext, int plaintexlen, unsigned char *ciphertext, unsigned char *ciphertext_noIV, uint64_t pos){
	pack64(pos,m_iv+8);

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
	if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, m_key, m_iv)) handleErrors();

	/* Provide the message to be encrypted, and obtain the encrypted output.
	* EVP_EncryptUpdate can be called multiple times if necessary
	*/
	if(1 != EVP_EncryptUpdate(ctx, ciphertext_noIV, &len, plaintext, plaintexlen)) handleErrors();
	ciphertexlen = len;

	/* Finalise the encryption. Further ciphertext bytes may be written at
	* this stage.
	*/
	if(1 != EVP_EncryptFinal_ex(ctx, ciphertext_noIV + len, &len)) handleErrors();
	ciphertexlen += len;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	pack32(ciphertexlen,ciphertext);

	memcpy(ciphertext+4,ciphertext_noIV,ciphertexlen*sizeof(unsigned char));
	return ciphertexlen+4;
}

int AES_ctr_256::decrypt(unsigned char *ciphertext, unsigned char *plaintext, uint64_t pos){
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
	if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext+4, unpack32(ciphertext))) handleErrors();
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

void AES_ctr_256::test(unsigned char * plaintext, int plaintexlen, unsigned char *ciphertext, unsigned char *ciphertext_noIV, unsigned char *decryptedtext){
	/* Encrypt the plaintext */
	int ciphertexlen = encrypt(plaintext, strlen((char *)plaintext),ciphertext, ciphertext_noIV,1);

	/* Decrypt the ciphertext */
	int decryptedtexlen = decrypt(ciphertext, decryptedtext,1);

	/* Add a NULL terminator. We are expecting printable text */
	decryptedtext[decryptedtexlen] = '\0';

	/* Show the decrypted text */
	std::cout << ciphertexlen << std::endl;
	std::cout << decryptedtext << std::endl;
}