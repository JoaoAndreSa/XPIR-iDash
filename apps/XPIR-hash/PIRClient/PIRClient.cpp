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
void PIRClient::removeInfoVCF(){
    int ret_val=std::system("exec rm -rf data/catalog.txt");

    if (ret_val==1){
        cout << "Error performing system call" << endl;
    }
}

int PIRClient::getInfoVCF(string filename){
    string f = Tools::readFromTextFile("data/catalog.txt");

    std::vector<string> vcf = Tools::tokenize(f,"\n");
    for(uint64_t i=0;i<vcf.size();i++){
        std::vector<string> line = Tools::tokenize(vcf[i]," ");
        if(line[0]==filename){
            return atoi(line[1].c_str());
        }
    }
}
/**
    Extract the exact ciphertext (with aggregation the reply contains more than one element).

    @param response reply data (ciphertext)
    @param aggregated_entrySize reply element size
    @param pos the relative position inside the 'pack' we want to extract

    @return response_s the specific element we are looking for or if it does not exist return ""
*/
std::string PIRClient::extractCiphertext(char* response, uint64_t alpha, uint64_t aggregated_entrySize, uint64_t pos, std::vector<string> query){
    unsigned char* ciphertext = new unsigned char[aggregated_entrySize];
    memcpy((char *)ciphertext,response+aggregated_entrySize*(pos%alpha),aggregated_entrySize);

    unsigned char* plaintext = new unsigned char[aggregated_entrySize];
    int plaintexlen = symmetricDecrypt(plaintext,ciphertext,pos,aggregated_entrySize);

    string decoded_pack = m_SHA_256->uchar_to_binary(plaintext,aggregated_entrySize,aggregated_entrySize*8);

    delete[] ciphertext;
    delete[] plaintext;

    return decoded_pack;
}

/**
    Extract the exact plaintext (with aggregation the reply contains more than one element).

    @param response reply data (plaintext)
    @param aggregated_entrySize reply element size
    @param pos the relative position inside the 'pack' we want to extract

    @return response_s the specific element we are looking for or if it does not exist return ""
*/
std::string PIRClient::extractPlaintext(char* response, uint64_t alpha, uint64_t aggregated_entrySize, uint64_t pos,std::vector<string> query){
    return m_SHA_256->uchar_to_binary(reinterpret_cast<unsigned char*>(response+aggregated_entrySize*(pos%alpha)),aggregated_entrySize,aggregated_entrySize*8);
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

std::vector<std::pair<uint64_t,std::vector<std::string>>> PIRClient::listQueryPos(std::map<char,std::string> entry){
    std::vector<std::string> chr = Tools::tokenize(entry['c'],",");
    std::vector<std::string> pos = Tools::tokenize(entry['p'],",");
    std::vector<std::string> ref = Tools::tokenize(entry['r'],",");
    std::vector<std::string> alt = Tools::tokenize(entry['a'],",");

    if(chr.size()!=pos.size() || chr.size()!=ref.size() || chr.size()!=alt.size()){
        Error::error(1,"Input Error\nUploading: ./client [-f folderPath]\nQuerying: ./client [-c chromosome1,2,...] [-p startPosition1,2,...] [-r refAllele1,2,...] [-a altAllele1,2,...] [-f vcfFile1,2,...]\n");
    }

    std::vector<std::pair<uint64_t,std::vector<std::string>>> all_pos;

    for(int i=0;i<chr.size();i++){
        //string query_str=chr[i]+"\t"+pos[i]+"\t.\t"+ref[i]+"\t"+alt[i]; //the . is to represent the missing id field
        string query_str=chr[i]+pos[i]+ref[i]+alt[i]; //the . is to represent the missing id field
        string data_hash=m_SHA_256->hash(query_str);
        uint64_t pos=stol(data_hash.substr(0,m_SHA_256->getHashSize()),nullptr,2);

        bool exists=false;
        int j;
        for(j=0;j<all_pos.size();j++){
            if(all_pos[j].first==pos){
                exists=true;
                break;
            }
        }

        if(exists){
            all_pos[j].second.push_back(data_hash);
        }else{
            std::vector<string> container;
            container.push_back(data_hash);
            all_pos.push_back(std::make_pair(pos,container));
        }
    }
    return all_pos;
}

/**
    Symmetric encrypt plaintext (AES_CBS256) and return the result.

    @param ciphertext variable that stores the ciphertext
    @param line variant(s) to be encrypted

    @return ciphertexlen length of the ciphertext (needed for write())
*/
int PIRClient::symmetricEncrypt(unsigned char* ciphertext, unsigned char* plaintext, uint64_t pos, int size){
    int ciphertexlen=m_aes_256->encrypt(plaintext,size,ciphertext,pos);
    return ciphertexlen;
}

/**
    Symmetric decrypt ciphertext (AES_CBS256) and return the result.

    @param decryptedtext variable that stores the decrypted plaintext
    @param line ciphertext to be decrypted

    @return decryptedtextlen length of the decrypted text (not needed for anything really, but just in case...)
*/
int PIRClient::symmetricDecrypt(unsigned char* plaintext, unsigned char* ciphertext, uint64_t pos, int size){
    int plaintexlen = m_aes_256->decrypt(ciphertext,size,plaintext,pos);
    return plaintexlen;
}

std::string PIRClient::padData(string input, int max_bits){
    int len=input.length();

    std::vector<char> v(max_bits-len,'0');
    std::string str(v.begin(),v.end());

    return str+input;
}

/**
    Encrypt (we can trigger this off) and send every variant in vcf file, or better still the catalog, to server.
    WARNING: Remember that for a given index we can potentially have more than one variant or even none.
    If there is nothing to send simply send byte 0.

    @param catalog where all the data (SNPs) is stored

    @return
*/
void PIRClient::sendData(std::vector<std::string> catalog, string filename, int max_bytesize){
    double start = omp_get_wtime(),total = 0;

    m_socket.sendInt(max_bytesize);
    for(uint64_t i=0; i<catalog.size();i++){
        unsigned char* entry;
        entry=m_SHA_256->binary_to_uchar(padData(catalog[i],max_bytesize*8));

        double start_t,end_t;
        if(!Constants::encrypted){    //if PLAINTEXT
            start_t = omp_get_wtime();
            m_socket.senduChar_s(entry,max_bytesize);
            end_t = omp_get_wtime();
        }else{                        //if CIPHERTEXT
            unsigned char* ciphertext = new unsigned char[max_bytesize];
            int ciphertexlen = symmetricEncrypt(ciphertext,entry,i,max_bytesize);

            start_t = omp_get_wtime();
            m_socket.senduChar_s(ciphertext,ciphertexlen);
            end_t = omp_get_wtime();

            delete[] ciphertext;
        }
        delete[] entry;

        total+=end_t-start_t;
    }
    if(Constants::bandwith_limit!=0) m_socket.sleepForBytes(sizeof(int)+(filename.length()+1)*sizeof(char)+sizeof(int)+sizeof(int)+max_bytesize*catalog.size(),total);

    Tools::writeToTextFile("data/catalog.txt",filename+" "+to_string(max_bytesize));
    double end = omp_get_wtime();
    std::cout << "PIRClient: Sending encrypted file took " << end-start << " (s)\n\n";
}

//***PUBLIC METHODS***//
/**
    Prepares and uploads the DB data to send to the server. 'Prepare' means initializing 'catalog' with size
    2^num_bits to store data, compute variant's position using HMAC (SHA256) and place in the correct place
    in the 'catalog' (if need be append with existent content using '->' as delimiter)

    @param filename name of the vcf file from which we read all genomic data

    @return num_entries number of entries written (number of variants in 'catalog')
*/
void PIRClient::uploadData(string foldername){
    removeInfoVCF();
    std::vector<string> listFiles = Tools::listFilesFolder(foldername);

    m_socket.senduInt64(listFiles.size());
    for(uint64_t i=0;i<listFiles.size();i++){
        try{
            double start = omp_get_wtime();

            m_socket.sendInt(listFiles[i].length()+1);
            m_socket.sendChar_s((char*)listFiles[i].c_str(),listFiles[i].length()+1);

            std::string line;
            ifstream f(foldername+listFiles[i]);
            Error::error(f==NULL || f.is_open()==0,"Error opening vcf file");

            std::vector<std::string>catalog(Constants::num_entries,"");

            int max_bitsize=0;
            if (f.is_open()){
                while(getline(f,line)){
                    if(line[0]!='#'){
                        std::vector<std::string> tokens = Tools::tokenize(line,"\t");
                        string data_hash=m_SHA_256->hash(tokens[0]+tokens[1]+tokens[3]+tokens[4]);
                        uint64_t pos=stol(data_hash.substr(0,m_SHA_256->getHashSize()),nullptr,2);  //hash variant and get its position in the 'catalog'
                        catalog[pos]+=data_hash;

                        if(catalog[pos].length()>max_bitsize) max_bitsize=catalog[pos].length();
                    }
                }
            }
            f.close();
            double end = omp_get_wtime();
            std::cout << "PIRClient: Preparing file " << listFiles[i] << " took " << end-start << " (s)\n";

            cout << "PIRClient: Sending file " << listFiles[i] << " to the server..." << endl;
            sendData(catalog,listFiles[i],max_bitsize/8);
        }catch (std::ios_base::failure &fail){
            Error::error(1,"Error reading vcf file");
        }
    }

    cout << "PIRClient: Waiting for server to load files..." << endl;
    if(m_socket.readInt()==1) cout << "PIRClient: All files loaded" << endl;
    else cout << "PIRClient: Error importing files at the server side" << endl;
}

void PIRClient::initSHA256(){
    m_SHA_256= new SHA_256(Tools::readParamsSHA());
}

void PIRClient::initAES256(){
    m_aes_256= new AES_ctr_256();                      //=0 CBC mode, =1 CTR mode
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