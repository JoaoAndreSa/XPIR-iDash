#include "PIRClientSequential.hpp"

//***PRIVATE METHODS***//
void PIRClientSequential::sendVector_s(std::vector<char*> vector_c){
    uint64_t pos=0;
    senduInt64(m_xpir->getD());

    for(uint64_t j=1 ; j<=m_xpir->getD(); j++){
        uint32_t length=m_xpir->getQsize(j);
        senduInt32(length);

        senduInt(m_xpir->getN()[j-1]);
        for (uint64_t i=0; i<m_xpir->getN()[j-1]; i++){
            sendXBytes(length,(void*)vector_c[pos]);
            pos++;
        }
    }
}

std::vector<char*> PIRClientSequential::queryGeneration(uint64_t chosen_element){
    std::vector<char*> query;
    query=m_xpir->queryGeneration(chosen_element);

    return query;
}

std::vector<char*> PIRClientSequential::readVector_s(){
    std::vector<char*> vector_s;

    uint64_t size=readuInt64();
    uint32_t message_length=readuInt32();

    double start = omp_get_wtime();
    for(uint64_t i=0; i<size;i++){
        char* buffer = new char[message_length];
        readXBytes(message_length,(void*)buffer);
        vector_s.push_back(buffer);
    }
    double end = omp_get_wtime();
    cout << "SimplePIR: Send query took " << end-start << " seconds" << endl;
    return vector_s;
}

XPIRcSequential::REPLY PIRClientSequential::readReply(){
	XPIRcSequential::REPLY reply;
	reply.nbRepliesGenerated=readuInt64();
	reply.aggregated_maxFileSize=readuInt64();
	reply.reply=readVector_s();
	return reply;
}

char* PIRClientSequential::replyExtraction(XPIRcSequential::REPLY reply){
	char* response;
    response=m_xpir->replyExtraction(reply);

    return response;
}

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
void PIRClientSequential::initXPIR(uint64_t num_entries){
    m_xpir= new XPIRcSequential(readParamsPIR(num_entries),1,nullptr);
}

std::string PIRClientSequential::searchQuery(uint64_t num_entries,std::map<char,std::string> entry){
    string query_str=entry['c']+" "+entry['p']+" # "+entry['r']+" "+entry['a'];
    uint64_t pos=m_SHA_256->hash(query_str);
    uint64_t pack_pos=considerPacking(m_SHA_256->hash(query_str),m_xpir->getAlpha());

    std::vector<char*> query=queryGeneration(pack_pos);
    sendVector_s(query);
    std::cout << "SimplePIR: Query sent" << "\n";

    XPIRcSequential::REPLY reply = readReply();
    char* response;
    response=replyExtraction(reply);
    std::string response_s;

    //if ciphertext
    response_s=extractCiphertext(response,reply.aggregated_maxFileSize,pos);
    //------ ### ------

    //if plaintext
    //response_s=extractPlaintext(response,reply.aggregated_maxFileSize,pos);
    //------ ### ------

    if(response_s!="") response_s = m_SHA_256->search(response_s,query_str);

    m_xpir->cleanQueryBuffer();
    cleanupVector(query);
    delete[] response;

    return response_s;
}

void PIRClientSequential::cleanupVector(vector<char*> v){
    for(uint64_t i=0;i<v.size();i++){
        delete[] v[i];
    }
}

void PIRClientSequential::cleanup(){
    delete m_SHA_256;
    delete m_xpir;
}