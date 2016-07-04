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

    double start = omp_get_wtime();
    for(uint64_t i=0; i<size;i++){
        char* buffer = new char[message_length];
        m_socket.readXBytes(message_length,(void*)buffer);
        replyData.push_back(buffer);
    }
    double end = omp_get_wtime();
    cout << "SimplePIR: Send reply took " << end-start << " seconds" << endl;
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
std::string PIRClientSequential::extractCiphertext(char* response, uint64_t aggregated_entrySize, uint64_t pos){
    if(response[aggregated_entrySize*(pos%m_xpir->getAlpha())]=='0'){
        cout << "Reply: " << endl << endl;
        return "";
    }

    unsigned char decryptedtext[1024];
    int decryptedtextlen = symmetricDecrypt(decryptedtext,response+aggregated_entrySize*(pos%m_xpir->getAlpha()));

    std::string response_s(reinterpret_cast<char*>(decryptedtext));
    cout << "Reply: " << response_s << endl << endl;

    return response_s;
}

/**
    Extract the exact plaintext (with aggregation the reply contains more than one element).

    @param response reply data (plaintext)
    @param aggregated_entrySize reply element size
    @param pos the relative position inside the 'pack' we want to extract

    @return response_s the specific element we are looking for or if it does not exist return ""
*/
std::string PIRClientSequential::extractPlaintext(char* response, uint64_t aggregated_entrySize, uint64_t pos){
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
std::string PIRClientSequential::searchQuery(uint64_t num_entries,std::map<char,std::string> entry){
    m_xpir= new XPIRcSequential(Tools::readParamsPIR(num_entries),1,nullptr);

    //#-------SETUP PHASE--------#
    //prepare and send file to server
    string query_str=entry['c']+" "+entry['p']+" # "+entry['r']+" "+entry['a'];
    uint64_t pos=m_SHA_256->hash(query_str);
    uint64_t pack_pos=considerPacking(m_SHA_256->hash(query_str),m_xpir->getAlpha());

    //#-------QUERY PHASE--------#
    std::vector<char*> query=queryGeneration(pack_pos);
    sendQuery(query);
    std::cout << "SimplePIR: Query sent" << "\n";

    //#-------REPLY PHASE--------#
    XPIRcSequential::REPLY reply = readReply();
    char* response;
    response=replyExtraction(reply);
    std::string response_s;

    if(!Constants::encrypted){   //if PLAINTEXT
        response_s=extractPlaintext(response,reply.maxFileSize,pos);
    }else{                       //if CIPHERTEXT
        response_s=extractCiphertext(response,reply.maxFileSize,pos);
    }

    if(response_s!="") response_s = m_SHA_256->search(response_s,query_str);

    //#-------CLEANUP PHASE--------#
    m_xpir->cleanQueryBuffer();
    Tools::cleanupVector(query);
    delete[] response;
    delete m_SHA_256;
    delete m_xpir;

    return response_s;
}