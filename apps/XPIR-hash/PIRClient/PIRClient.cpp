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
void PIRClient::removeData(){
    int ret_val=std::system("exec rm -rf data/*");
    if (ret_val==1) cout << "Error performing system call" << endl;

    ret_val = std::system("exec mkdir data/nonces");

    delete m_SHA_256;
    delete m_AES_256;

    initAES256();
    initSHA256();
}

/**
    Extract the exact ciphertext (with aggregation the reply contains more than one element).

    @param response reply data (ciphertext)
    @param alpha aggregation value
    @param aggregated_entrySize reply element size
    @param pos the relative position inside the 'pack' we want to extract

    @return decoded_pack returns the symmetrically decrypted reply
*/
std::string PIRClient::extractCiphertext(char* response, uint64_t alpha, uint64_t aggregated_entrySize, uint64_t pos){
    string decoded_pack="";

    int data_hash_bytes = ceil(Constants::data_hash_size/8);
    for(int i=0;i<Constants::padding_size;i++){
        unsigned char* ciphertext = new unsigned char[data_hash_bytes];
        memcpy((char *)ciphertext,response+aggregated_entrySize*(pos%alpha)+data_hash_bytes*i,data_hash_bytes);

        unsigned char* plaintext = new unsigned char[data_hash_bytes];
        int plaintexlen = symmetricDecrypt(plaintext,ciphertext,pos*Constants::padding_size+i,data_hash_bytes);

        string decoded_elem = m_SHA_256->uchar_to_binary(plaintext,data_hash_bytes,data_hash_bytes*8);

        decoded_pack+=decoded_elem;

        delete[] ciphertext;
        delete[] plaintext;
    }

    return decoded_pack;
}

/**
    Extract the exact plaintext (with aggregation the reply contains more than one element).

    @param response reply data (plaintext)
    @param alpha aggregation value
    @param aggregated_entrySize reply element size
    @param pos the relative position inside the 'pack' we want to extract

    @return response_s the specific element we are looking for or if it does not exist return ""
*/
std::string PIRClient::extractPlaintext(char* response, uint64_t alpha, uint64_t aggregated_entrySize, uint64_t pos){
    return m_SHA_256->uchar_to_binary(reinterpret_cast<unsigned char*>(response+aggregated_entrySize*(pos%alpha)),aggregated_entrySize,aggregated_entrySize*8);
}

bool PIRClient::checkContent(char* response, uint64_t alpha, int max_bytesize, std::pair<uint64_t,std::vector<std::string>> elements){
    bool check=true;

    string response_s;
    if(!Constants::encrypted){   //if PLAINTEXT
        response_s = extractPlaintext(response,alpha,max_bytesize,elements.first);
    }else{                       //if CIPHERTEXT
        response_s = extractCiphertext(response,alpha,max_bytesize,elements.first);
    }

    //TIME MEASURE (only for iDash challenge): retrieved variants/per query 
    int variants_retrieved=0;
    string zeros(Constants::data_hash_size,'0');
    for(int i=0;i<response_s.length();i+=Constants::data_hash_size){
        if(zeros.compare(response_s.substr(i,Constants::data_hash_size))!=0){
            variants_retrieved++;
        }
    }
    cout << "PIRClient: " << variants_retrieved << " variants retrieved" << endl;
    //END

    for(int j=0;j<elements.second.size();j++){
        if(m_SHA_256->search(elements.second[j],response_s)==false){
            check=false;
        }
    }

    //#-------CLEANUP PHASE--------#
    delete[] response;
    return check;
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
        string query_str=chr[i]+pos[i]+ref[i]+alt[i];
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
    int ciphertexlen=m_AES_256->encrypt(plaintext,size,ciphertext,pos);
    return ciphertexlen;
}

/**
    Symmetric decrypt ciphertext (AES_CBS256) and return the result.

    @param decryptedtext variable that stores the decrypted plaintext
    @param line ciphertext to be decrypted

    @return decryptedtextlen length of the decrypted text (not needed for anything really, but just in case...)
*/
int PIRClient::symmetricDecrypt(unsigned char* plaintext, unsigned char* ciphertext, uint64_t pos, int size){
    int plaintexlen = m_AES_256->decrypt(ciphertext,size,plaintext,pos);
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
void PIRClient::sendData(std::vector<std::string> catalog, string filename){
    double start = omp_get_wtime(),total = 0;
    m_AES_256->setIV(filename);

    int max_bytesize=Constants::padding_size*Constants::data_hash_size/8;
    m_socket.sendInt(max_bytesize);
    m_socket.sendInt(catalog.size());

    int data_hash_bytes = ceil(Constants::data_hash_size/8);
    uint64_t pos=0;

    for(uint64_t i=0; i<catalog.size();i++){
        unsigned char* entry=m_SHA_256->binary_to_uchar(padData(catalog[i],max_bytesize*8));

        double start_t,end_t;
        if(!Constants::encrypted){  //if PLAINTEXT
            start_t = omp_get_wtime();
            m_socket.senduChar_s(entry,max_bytesize);
            end_t = omp_get_wtime();
        }else{                      //if CIPHERTEXT
            for(int j=0;j<Constants::padding_size; j++,pos++){
                unsigned char* ciphertext = new unsigned char[data_hash_bytes];
                int ciphertexlen = symmetricEncrypt(ciphertext,entry+data_hash_bytes*j,pos,data_hash_bytes);

                start_t = omp_get_wtime();
                m_socket.senduChar_s(ciphertext,ciphertexlen);
                end_t = omp_get_wtime();

                delete[] ciphertext;

                total+=end_t-start_t;
            }
        }
        delete[] entry;
    }
    if(Constants::bandwith_limit!=0) m_socket.sleepForBytes(sizeof(uint64_t)+sizeof(int)+(filename.length()+1)*sizeof(char)+sizeof(int)+sizeof(int)+sizeof(int)+max_bytesize*catalog.size(),total);
    double end = omp_get_wtime();
    std::cout << "PIRClient: Sending encrypted file took " << end-start << " (s)\n";
}

//***PUBLIC METHODS***//
/**
    Prepares and uploads the DB data to send to the server. 'Prepare' means initializing 'catalog' with size
    2^num_bits to store data, compute variant's position using HMAC (SHA256) and place in the correct place
    in the 'catalog'.

    @param filename name of the vcf file from which we read all genomic data

    @return num_entries number of entries written (number of variants in 'catalog')
*/
void PIRClient::uploadData(string foldername){
    removeData();
    std::vector<string> listFiles = Tools::listFilesFolder(foldername);

    int num_attempts=0;

    m_socket.senduInt64(listFiles.size());
    for(uint64_t i=0;i<listFiles.size();i++){
        try{
            double start = omp_get_wtime();

            m_socket.sendInt(listFiles[i].length()+1);
            m_socket.sendChar_s((char*)listFiles[i].c_str(),listFiles[i].length()+1);

            std::string line;
            ifstream f(foldername+listFiles[i]);
            Error::error(f==NULL || f.is_open()==0,"Error opening vcf file");

            std::vector<std::string>catalog(pow(2,m_SHA_256->getHashSize()),"");

            int redo=0;
            int max_collisions=0;
            if (f.is_open()){
                while(getline(f,line)){
                    if(line[0]!='#'){
                        std::vector<std::string> tokens = Tools::tokenize(line,"\t");
                        string data_hash=m_SHA_256->hash(tokens[0]+tokens[1]+tokens[3]+tokens[4]);
                        uint64_t pos=stol(data_hash.substr(0,m_SHA_256->getHashSize()),nullptr,2);  //hash variant and get its position in the 'catalog'
                        catalog[pos]+=data_hash;

                        if(catalog[pos].length()>(Constants::padding_size*Constants::data_hash_size)){
                            redo=1;
                            break;
                        }
                        //TIME MEASURE (only for iDash challenge): max retrieved variants/per query 
                        if(catalog[pos].length()>max_collisions) max_collisions=catalog[pos].length();
                        //END
                    }
                }
            }
            max_collisions = max_collisions/Constants::data_hash_size;
            cout << "\nPIRClient: The maximum number of collisions is " << max_collisions;

            if(redo==1){
                num_attempts++;
                if(num_attempts==5){
                    m_socket.sendInt(1);
                    cout << "\n[IMPORTANT] You really need to increase the padding_size!" << endl;
                    return;
                }
                cout << "\n[IMPORTANT] Re-uploading... You may need to increase the padding_size" << endl;
                m_socket.sendInt(2);
                i=-1;
                removeData();
                continue;
            }

            f.close();
            double end = omp_get_wtime();
            std::cout << "\nPIRClient: Preparing file " << listFiles[i] << " took " << end-start << " (s)\n";

            cout << "PIRClient: Sending file " << listFiles[i] << " to the server..." << endl;
            m_socket.sendInt(0);
            sendData(catalog,listFiles[i]);
        }catch (std::ios_base::failure &fail){
            m_socket.sendInt(1);
            Error::error(1,"Error reading vcf file");
        }
    }

    cout << "PIRClient: Waiting for server to load files..." << endl;
    if(m_socket.readInt()==1) cout << "PIRClient: All files loaded" << endl << endl;
    else cout << "PIRClient: Error importing files at the server side" << endl;
}

void PIRClient::initSHA256(){
    m_SHA_256= new SHA_256(Tools::readParamsSHA());
}

void PIRClient::initAES256(){
    m_AES_256= new AES_ctr_256();                      //=0 CBC mode, =1 CTR mode
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