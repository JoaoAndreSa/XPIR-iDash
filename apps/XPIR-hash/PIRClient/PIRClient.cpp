/**
    XPIR-hash
    PIRClient.cpp
    Purpose: Parent class (abstract) that binds to each client. Can have to modes of operation: Sequential or Pipeline (child classes).

    @author Joao Sa
    @version 1.0 01/07/16
*/

/**

                  PIRClient(*)
                      |
           ----------- -----------
           |                     |
  PIRClientSequential    PIRClientPipeline

*/

#include "PIRClient.hpp"

//***PRIVATE METHODS***//
/**
    Compares two SNPs/variants and returns 1 if they are equal or 0 otherwise.

    @param t_curr a string that contains variant information (e.g. 1    160929435   rs7520618   G   A   .   .   SVTYPE=SNP;END=160929436)
    @param entry a map/dictionary the stores que variant(s) beeing queried in a key-value way

    @return int 1 if equal; 0 otherwise
*/
int PIRClient::compareSNPs(std::string t_curr, std::map<char,std::string> entry){
    istringstream curr(t_curr);
    std::vector<std::string> tokens_curr{istream_iterator<std::string>{curr},istream_iterator<std::string>{}};

    if( (atoi(tokens_curr[0].c_str()) == atoi(entry['c'].c_str())) && (atoi(tokens_curr[1].c_str()) == atoi(entry['p'].c_str())) && (tokens_curr[3]==entry['r'])  && (tokens_curr[4]==entry['a']) ){
        return 1;
    }else{
        return 0;
    }
}

/**
    Symmetric encrypt plaintext (AES_CBS256) and return the result.

    @param ciphertext variable that stores the ciphertext
    @param line variant(s) to be encrypted

    @return ciphertexlen length of the ciphertext (needed for write())
*/
int PIRClient::symmetricEncrypt(unsigned char* ciphertext, std::string line){
    unsigned char ciphertext_noIV[1024]; //ciphertext with noIV (for memcpy purposes - C stuff)

    unsigned char *plaintext = new unsigned char[line.length()+1];
    memcpy((char*)plaintext,line.c_str(),line.length()+1);

    int ciphertexlen=m_cbc.encrypt(plaintext,strlen((char *)plaintext),ciphertext,ciphertext_noIV);

    delete[] plaintext;
    return ciphertexlen;
}

/**
    Symmetric decrypt ciphertext (AES_CBS256) and return the result.

    @param decryptedtext variable that stores the decrypted plaintext
    @param line ciphertext to be decrypted

    @return decryptedtextlen length of the decrypted text (not needed for anything really, but just in case...)
*/
int PIRClient::symmetricDecrypt(unsigned char* decryptedtext, char* line){
    unsigned char ciphertext[1024];
    memcpy((char *)ciphertext,line,1024);

    int decryptedtextlen = m_cbc.decrypt(ciphertext,decryptedtext);
    decryptedtext[decryptedtextlen] = '\0';

    return decryptedtextlen;
}

/**
    Send ciphertext through socket (length+data).

    @param ciphertexlen length of the ciphertext
    @param ciphertext variable that stores the ciphertext

    @return
*/
void PIRClient::sendCiphertext(int ciphertexlen,unsigned char* ciphertext){
    m_socket.sendInt(ciphertexlen);
    m_socket.senduChar_s(ciphertext,ciphertexlen);
}

/**
    Send plaintext through socket (length+data). In this case there is no symmetric encryption

    @param plaintexlen length of the plaintext
    @param str variant(s) to be sent (as a string)

    @return
*/
void PIRClient::sendPlaintext(int plaintexlen,string str){
    unsigned char *plaintext = new unsigned char[plaintexlen];
    memcpy((char*)plaintext,str.c_str(),plaintexlen);


    m_socket.sendInt(plaintexlen);
    m_socket.senduChar_s(plaintext,plaintexlen);
    delete[] plaintext;
}

/**
    Encrypt (we can trigger this off) and send every variant in vcf file, or better still the catalog, to server.
    WARNING: Remember that for a given index we can potentially have more than one variant or even none.
    If there is nothing to send simply send byte 0.

    @param catalog where all the data (SNPs) is stored

    @return
*/
uint64_t PIRClient::sendData(std::vector<std::string> catalog){
    uint64_t num_entries=0;

    uint64_t bytes_sent=0;
    for(uint64_t i=0; i<catalog.size();i++){
        if(catalog[i]!=""){
            if(!Constants::encrypted){   //if PLAINTEXT
                double start = omp_get_wtime();
                sendPlaintext(catalog[i].length()+1,catalog[i]);
                bytes_sent+=catalog[i].length()+1;
                 double end = omp_get_wtime();
                std::cout << bytes_sent <<"PIRClient: Send file took " << end-start << " (s)\n";
            }else{                        //if CIPHERTEXT
                unsigned char ciphertext[1024];
                int ciphertexlen = symmetricEncrypt(ciphertext,catalog[i]);
                sendCiphertext(ciphertexlen,ciphertext);
                bytes_sent+=ciphertexlen;
            }
        }else{
            std::string blank("0");
            sendPlaintext(blank.length(),blank);
            bytes_sent+=blank.length();
        }

        num_entries++;
    }
    m_socket.sendInt(0);    //signal EOF (no more things to write)


    return num_entries;
}

/**
    Returns the position relative to the aggregation/packing value.

    @param pos the variant position
    @param alpha aggregation/packing value

    @return its relative position
*/
uint64_t PIRClient::considerPacking(uint64_t pos, uint64_t alpha){
    if(alpha>1){    //if there is aggregation/packing
        return floor(static_cast<double>(pos)/alpha);
    }else{          //otherwise
        return pos;
    }
}

//***PUBLIC METHODS***//
/**
    Prepares and uploads the DB data to send to the server. 'Prepare' means initializing 'catalog' with size
    2^num_bits to store data, compute variant's position using HMAC (SHA256) and place in the correct place
    in the 'catalog' (if need be append with existent content using '->' as delimiter)

    @param filename name of the vcf file from which we read all genomic data

    @return num_entries number of entries written (number of variants in 'catalog')
*/
uint64_t PIRClient::uploadData(std::string filename){
	std::string line;
    uint64_t num_entries=0;

    try{

        ifstream f(filename);

        Socket::errorExit(f==NULL || f.is_open()==0,"Error reading file");

        //send the size of the vector to be stored
        std::vector<std::string>catalog(m_SHA_256->getSizeBits());

        double start = omp_get_wtime();
        if (f.is_open()){
            while(getline(f,line)){
                if(line[0]!='#'){
                    int pos=m_SHA_256->hash(line); //hash variant and get its position in the 'catalog'
                    if(catalog[pos]!=""){
                        catalog[pos]+="->";        //delimiter (for appending)
                    }
                    catalog[pos]+=line;
                }
            }
        }
        f.close();
        double end = omp_get_wtime();
        std::cout << "PIRClient: Prepare file took " << end-start << " (s)\n";

        num_entries=sendData(catalog);

    }catch (std::ios_base::failure &fail){
        Socket::errorExit(1,"Error writing output file");
    }

	return num_entries;
}

void PIRClient::initSHA256(){
    m_SHA_256= new SHA_256(Tools::readParamsSHA());
}

void PIRClient::setRTTStart(){
    m_RTT_start=omp_get_wtime();
}
double PIRClient::getRTTStart(){
    return m_RTT_start;
}

void PIRClient::setRTTStop(){
    m_RTT_stop=omp_get_wtime();
}
double PIRClient::getRTTStop(){
    return m_RTT_stop;
}