/**
    XPIR-hash
    PIRClientSequential.cpp
    Purpose: Child class that executes client using sequential PIR.
             NOTE: In sequential PIR, client generates the entire query and only then does he send it to the server.
             Furthermore, he has to wait and get all reply elements before starting the reply extraction.

    @author Joao Sa
    @version 1.0 01/07/16
*/

/**

                  PIRClient
                      |
           ----------- -----------
           |                     |
  PIRClientSequential(*)   PIRClientPipeline

*/

#include "PIRClientSequential.hpp"

//***PRIVATE METHODS***//
//QUERY GENERATION & SEND QUERY
void PIRClientSequential::sendQuery(std::vector<char*> query){
    double start = omp_get_wtime();

    uint64_t pos=0;
    m_socket.senduInt64(m_xpir->getD());                //number of dimensions

    for(uint64_t j=1 ; j<=m_xpir->getD(); j++){
        uint32_t length=m_xpir->getQsize(j);            //query size (depends on the dimension)
        m_socket.senduInt32(length);

        m_socket.senduInt(m_xpir->getN()[j-1]);         //number of elements in that dimension
        for (uint64_t i=0; i<m_xpir->getN()[j-1]; i++){ //for each element in that dimension
            m_socket.sendXBytes(length,(void*)query[pos]);
            pos++;
        }
    }

    double end = omp_get_wtime();
    cout << "PIRClient: Send query took " << end-start << " seconds" << endl;
}

/**
    Generate and return query.

    @param chosen_element the position/index of the variant we are querying

    @return query
*/
std::vector<char*> PIRClientSequential::queryGeneration(uint64_t chosen_element){
    std::vector<char*> query;
    query=m_xpir->queryGeneration(chosen_element);

    return query;
}

//READ REPLY
std::vector<char*> PIRClientSequential::readReplyData(){
    std::vector<char*> replyData;

    uint64_t size=m_socket.readuInt64();            //number of reply elements
    uint32_t message_length=m_socket.readuInt32();  //size (bytes) of each element

    for(uint64_t i=0; i<size;i++){
        char* buffer = new char[message_length];
        m_socket.readXBytes(message_length,(void*)buffer);
        replyData.push_back(buffer);
    }
    return replyData;
}

/**
    Read all elements of reply (not just data but also the number of reply elements generated and the max file size).

    @param

    @return reply
*/
XPIRcSequential::REPLY PIRClientSequential::readReply(){
	XPIRcSequential::REPLY reply;
	reply.nbRepliesGenerated=m_socket.readuInt64();
	reply.maxFileSize=m_socket.readuInt64();
	reply.reply=readReplyData();
	return reply;
}

//REPLY EXTRACTION
char* PIRClientSequential::replyExtraction(XPIRcSequential::REPLY reply){
	char* response;
    response=m_xpir->replyExtraction(reply);

    return response;
}

/**
    Extract the exact ciphertext (with aggregation the reply contains more than one element).

    @param response reply data (ciphertext)
    @param aggregated_entrySize reply element size
    @param pos the relative position inside the 'pack' we want to extract

    @return response_s the specific element we are looking for or if it does not exist return ""
*/
bool PIRClientSequential::extractCiphertext(char* response, uint64_t aggregated_entrySize, uint64_t pos,std::vector<string> query){
    unsigned char* ciphertext = new unsigned char[aggregated_entrySize];
    memcpy((char *)ciphertext,response+aggregated_entrySize*(pos%m_xpir->getAlpha()),aggregated_entrySize);

    unsigned char* plaintext = new unsigned char[aggregated_entrySize];
    int plaintexlen = symmetricDecrypt(plaintext,ciphertext,pos);

    string decoded_pack = m_SHA_256->uchar_to_binary(plaintext,aggregated_entrySize,aggregated_entrySize*8).substr(3);

    std::vector<bool> check(query.size(),false);
    int snp_bitsize=69;
    for(int i=0;i<decoded_pack.length();i+=snp_bitsize){
        for(int j=0;j<query.size();j++){
            if(padData(query[j],snp_bitsize).compare(decoded_pack.substr(i,snp_bitsize))==0){
                check[j]=true;
            }
        }
    }

    delete[] ciphertext;
    delete[] plaintext;

    for(int i=0;i<check.size();i++){
        if(check[i]==false){
            return false;
        }
    }
    return true;
}

/**
    Extract the exact plaintext (with aggregation the reply contains more than one element).

    @param response reply data (plaintext)
    @param aggregated_entrySize reply element size
    @param pos the relative position inside the 'pack' we want to extract

    @return response_s the specific element we are looking for or if it does not exist return ""
*/
std::string PIRClientSequential::extractPlaintext(char* response, uint64_t aggregated_entrySize, uint64_t pos,std::vector<string> query){
    if(response+aggregated_entrySize*(pos%m_xpir->getAlpha())=='\0'){
        return "";
    }else{
        std::string response_s(response+aggregated_entrySize*(pos%m_xpir->getAlpha()));
        cout << "Reply: " << response_s << endl << endl;
        return response_s;
    }
}

//***PUBLIC METHODS***//
/**
    Main function of PIRClientSequential class. Queries server!

    @param num_entries total number of entries (size of database)
    @param entry a map/dictionary the stores que variant(s) beeing queried in a key-value way

    @return response_s stores the variant(s) we are looking for or "" otherwise
*/
bool PIRClientSequential::searchQuery(uint64_t num_entries,std::map<char,std::string> entry){
    m_xpir= new XPIRcSequential(Tools::readParamsPIR(num_entries),1,nullptr);

    //#-------SETUP PHASE--------#
    std::vector<std::pair<uint64_t,std::vector<std::string>>> pos = listQueryPos(entry);
    //TODO: Search multiple variants at the same time
    uint64_t pack_pos=considerPacking(pos[0].first,m_xpir->getAlpha());

    //#-------QUERY PHASE--------#
    std::vector<char*> query=queryGeneration(pack_pos);
    sendQuery(query);
    std::cout << "PIRClient: Query sent" << "\n";

    //#-------REPLY PHASE--------#
    XPIRcSequential::REPLY reply = readReply();
    char* response;
    response=replyExtraction(reply);
    bool response_b;

    if(!Constants::encrypted){   //if PLAINTEXT
        extractPlaintext(response,reply.maxFileSize,pos[0].first,pos[0].second);
    }else{                       //if CIPHERTEXT
        response_b = extractCiphertext(response,reply.maxFileSize,pos[0].first,pos[0].second);
    }

    //#-------CLEANUP PHASE--------#
    m_xpir->cleanQueryBuffer();
    Tools::cleanupVector(query);
    delete[] response;
    delete m_SHA_256;
    delete m_xpir;

    return response_b;
    
    return true;
}
