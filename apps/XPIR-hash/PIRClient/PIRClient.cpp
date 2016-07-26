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
std::vector<string> PIRClient::listFilesFolder(string foldername){
    std::vector<string> listFiles;

    DIR* dir;
    struct dirent* file;
    dir = opendir(foldername.c_str());

    if(dir==NULL) Error::error(1,"Error reading file");

    while ((file = readdir(dir)) != NULL){
        string filename(file->d_name);
        if (filename.find(".vcf") != std::string::npos){
            listFiles.push_back(filename);
        }
    }
    (void)closedir(dir);

    return listFiles;
}

/**
    Symmetric encrypt plaintext (AES_CBS256) and return the result.

    @param ciphertext variable that stores the ciphertext
    @param line variant(s) to be encrypted

    @return ciphertexlen length of the ciphertext (needed for write())
*/
int PIRClient::symmetricEncrypt(unsigned char* ciphertext, unsigned char* plaintext, uint64_t pos){
    int ciphertexlen=m_aes_256->encrypt(plaintext,m_max_bytesize,ciphertext,pos);
    return ciphertexlen;
}

/**
    Symmetric decrypt ciphertext (AES_CBS256) and return the result.

    @param decryptedtext variable that stores the decrypted plaintext
    @param line ciphertext to be decrypted

    @return decryptedtextlen length of the decrypted text (not needed for anything really, but just in case...)
*/
int PIRClient::symmetricDecrypt(unsigned char* plaintext, unsigned char* ciphertext, uint64_t pos){
    int plaintexlen = m_aes_256->decrypt(ciphertext,9,plaintext,pos);
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
void PIRClient::sendData(std::vector<std::vector<std::string>> catalog,string filename){
    double start = omp_get_wtime(),total = 0;

    m_socket.sendInt(filename.length()+1);
    m_socket.sendChar_s((char*)filename.c_str(),filename.length()+1);

    if(m_socket.readInt()==1){ //if FILE EXISTS
        std::cout << "PIRClient: File " << filename << " already uploaded\n";
        return;
    }

    Tools::writeToTextFile("data/catalog.txt",filename+" "+to_string(m_snp_bitsize)+" "+to_string(m_bits_pad)+" "+to_string(m_max_bytesize));

    m_socket.sendInt(m_max_bytesize);
    for(uint64_t i=0; i<catalog.size();i++){
        unsigned char* entry;

        if(catalog[i].size()>0){
            entry=m_SHA_256->binary_to_uchar(padData(catalog[i][0],m_max_bytesize*8));
        }else{
            entry=m_SHA_256->binary_to_uchar(padData("",m_max_bytesize*8));
        }

        double start_t,end_t;
        if(!Constants::encrypted){    //if PLAINTEXT
            start_t = omp_get_wtime();
            m_socket.senduChar_s(entry,m_max_bytesize);
            end_t = omp_get_wtime();
        }else{                        //if CIPHERTEXT
            unsigned char* ciphertext = new unsigned char[m_max_bytesize];
            int ciphertexlen = symmetricEncrypt(ciphertext,entry,i);

            start_t = omp_get_wtime();
            m_socket.senduChar_s(ciphertext,ciphertexlen);
            end_t = omp_get_wtime();

            delete[] ciphertext;
        }
        delete[] entry;

        total+=end_t-start_t;
    }
    if(Constants::bandwith_limit!=0) m_socket.sleepForBytes(sizeof(uint64_t)+sizeof(int)+(filename.length()+1)*sizeof(char)+sizeof(int)+sizeof(int)+m_max_bytesize*catalog.size(),total);

    double end = omp_get_wtime();
    std::cout << "PIRClient: Sending encrypted file took " << end-start << " (s)\n";
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
    std::vector<std::string> chr = m_SHA_256->tokenize(entry['c'],",");
    std::vector<std::string> pos = m_SHA_256->tokenize(entry['p'],",");
    std::vector<std::string> ref = m_SHA_256->tokenize(entry['r'],",");
    std::vector<std::string> alt = m_SHA_256->tokenize(entry['a'],",");

    if(chr.size()!=pos.size() || chr.size()!=ref.size() || chr.size()!=alt.size()){
        Error::error(1,"Input Error\nUploading: ./client [-f folderPath]\nQuerying: ./client [-c chromosome1,2,...] [-p startPosition1,2,...] [-r refAllele1,2,...] [-a altAllele1,2,...] [-f vcfFile1,2,...]\n");
    }

    std::vector<std::pair<uint64_t,std::vector<std::string>>> all_pos;

    for(int i=0;i<chr.size();i++){
        string query_str=chr[i]+"\t"+pos[i]+"\t.\t"+ref[i]+"\t"+alt[i]; //the . is to represent the missing id field

        string encoded=m_SHA_256->encoding(query_str);
        uint64_t pos=m_SHA_256->hash(encoded);

        bool exists=false;
        int j;
        for(j=0;j<all_pos.size();j++){
            if(all_pos[j].first==pos){
                exists=true;
                break;
            }
        }

        if(exists){
            all_pos[j].second.push_back(encoded);
        }else{
            std::vector<string> container;
            container.push_back(encoded);
            all_pos.push_back(std::make_pair(pos,container));
        }
    }
    return all_pos;
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
	std::vector<string> listFiles=listFilesFolder(foldername);
    m_socket.senduInt64(listFiles.size());

    for(uint64_t i=0;i<listFiles.size();i++){

        try{
            double start = omp_get_wtime();

            std::string line;
            ifstream f(foldername+listFiles[i]);
            Error::error(f==NULL || f.is_open()==0,"Error opening vcf file");

            std::vector<std::vector<std::string>>catalog(Constants::num_entries);

            int snp_bitsize=0;
            if (f.is_open()){
                while(getline(f,line)){
                    if(line[0]!='#'){
                        std::string encoded=m_SHA_256->encoding(line);

                        uint64_t pos=m_SHA_256->hash(encoded); //hash variant and get its position in the 'catalog'
                        catalog[pos].push_back(encoded);

                        if(encoded.length()>snp_bitsize) snp_bitsize=encoded.length();
                    }
                }
            }
            m_snp_bitsize=snp_bitsize;
            f.close();

            int max_bitsize=0;
            for(uint64_t i=0;i<catalog.size();i++){
                std:string aux("");
                for(uint64_t j=0;j<catalog[i].size();j++){
                    aux+=padData(catalog[i][j],snp_bitsize);
                }

                if(aux!=""){
                    if(aux.length()>max_bitsize) max_bitsize=aux.length();

                    catalog[i][0]=aux;
                }
            }

            m_bits_pad=(8-max_bitsize%8);
            m_max_bytesize=(max_bitsize+m_bits_pad)/8;

            double end = omp_get_wtime();
            std::cout << "PIRClient: Preparing file " << listFiles[i] << " took " << end-start << " (s)\n";

            sendData(catalog,listFiles[i]);
        }catch (std::ios_base::failure &fail){
            Error::error(1,"Error reading vcf file");
        }
    }
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