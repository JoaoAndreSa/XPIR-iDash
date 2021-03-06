/**
    XPIR-hash
    PIRClientSequential.cpp
    Purpose: Child class that executes client using sequential PIR.
             NOTE: In sequential PIR, client generates the entire query and only then does he send it to the server.
             Furthermore, he has to wait and get all reply elements before starting the reply extraction.

    @author Joao Sa
    @version 1.0 18/01/17
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
void PIRClientSequential::sendQuery(std::vector<char*> query, XPIRcSequential* xpir, vector<char*> request){
    double start = omp_get_wtime(), start_t, end_t, total=0;

    uint64_t request_size = xpir->getCrypto()->getCiphertextBytesize();
    m_socket.sendInt(request.size());

    for(int i=0;i<request.size();i++){
        m_socket.sendXBytes(request_size,(void*)request[i]);
        delete[] request[i];
    }

    uint64_t pos=0;
    m_socket.senduInt64(xpir->getD());                //number of dimensions

    uint64_t total_bytes=0;
    for(uint64_t j=1 ; j<=xpir->getD(); j++){
        uint32_t length=xpir->getQsize(j);            //query size (depends on the dimension)
        m_socket.senduInt32(length);
        m_socket.senduInt(xpir->getN()[j-1]);         //number of elements in that dimension

        start_t = omp_get_wtime();
        for (uint64_t i=0; i<xpir->getN()[j-1]; i++){ //for each element in that dimension
            m_socket.sendXBytes(length,(void*)query[pos]);
            pos++;

            total_bytes+=length;
        }
        end_t = omp_get_wtime();

        total+=end_t-start_t;
    }
    if(Constants::bandwith_limit!=0) m_socket.sleepForBytes(request_size*request.size() + sizeof(int) + sizeof(uint64_t) + xpir->getD()*(sizeof(uint32_t)+sizeof(int)) + total_bytes,total);

    double end = omp_get_wtime();
    cout << "PIRClient: Send query (" << total_bytes << " bytes) took " << end-start << " seconds" << endl;
}

/**
    Generate and return query.

    @param chosen_element the position/index of the variant we are querying
    @param xpir to perform the PIR operations

    @return query
*/
std::vector<char*> PIRClientSequential::queryGeneration(uint64_t chosen_element,XPIRcSequential* xpir){
    std::vector<char*> query;
    query=xpir->queryGeneration(chosen_element);

    return query;
}

//READ REPLY
std::vector<char*> PIRClientSequential::readReplyData(){
    double start_t,end_t,total=0;

    std::vector<char*> replyData;

    uint64_t size=m_socket.readuInt64();            //number of reply elements
    uint32_t message_length=m_socket.readuInt32();  //size (bytes) of each element

    start_t = omp_get_wtime();
    for(uint64_t i=0; i<size;i++){
        char* buffer = new char[message_length];
        m_socket.readXBytes(message_length,(void*)buffer);
        replyData.push_back(buffer);
    }
    end_t = omp_get_wtime();
    total+=end_t-start_t;
    if(Constants::bandwith_limit!=0) m_socket.sleepForBytes(sizeof(uint64_t)+sizeof(uint64_t)+sizeof(uint64_t)+sizeof(uint32_t)+size*message_length,total);

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
char* PIRClientSequential::replyExtraction(XPIRcSequential::REPLY reply,XPIRcSequential* xpir){
	char* response;
    response=xpir->replyExtraction(reply);

    return response;
}

//***PUBLIC METHODS***//
/**
    Main function of PIRClientSequential class. Queries server!

    @param entry a map/dictionary the stores que variant(s) beeing queried in a key-value way

    @return true or false whether all variants were found or at least one was missing
*/
bool PIRClientSequential::searchQuery(std::map<char,std::string> entry){
    bool check=true;

    //#-------INITIALIZATION PHASE--------#
    m_socket.sendInt(entry['f'].length()+1);
    m_socket.sendChar_s(const_cast<char*>(entry['f'].c_str()),entry['f'].length()+1);

    std::vector<std::pair<uint64_t,std::string>> pos = listQueryPos(entry);
    m_socket.sendInt(pos.size());

    int max_bytesize = ceil(Constants::padding_size*Constants::data_hash_size/8);
    int data_hash_bytes = ceil(Constants::data_hash_size/8);

    std::vector<XPIRcSequential*> container;
    std::vector<string> files = Tools::tokenize(entry['f'],",");
    for(int k=0;k<files.size();k++){
        m_AES_256->setIV(files[k]);
        for(int i=0;i<pos.size();i++){
            XPIRcSequential* xpir= new XPIRcSequential(Tools::readParamsPIR(Constants::num_entries),1,nullptr,nullptr);
            container.push_back(xpir);

            //#-------INITIALIZATION PHASE--------#
            uint64_t pack_pos=considerPacking(pos[i].first,xpir->getAlpha());

            //#-------QUERY PHASE--------#
            unsigned char* request = generateRequest(pos[i].first,pos[i].second,data_hash_bytes);
            vector<char*> request_encrypted = encryptRequest(request,xpir->getCrypto(),xpir->getAlpha(),max_bytesize);

            std::vector<char*> query=queryGeneration(pack_pos,xpir);
            sendQuery(query,xpir,request_encrypted);

            //#-------CLEANUP PHASE--------#
            container[i]->cleanQueryBuffer();
            Tools::cleanupVector(query);
        }
    }
    std::cout << "PIRClient: Query sent" << "\n\n";

    //#-------CHECK RESPONSE PHASE--------#
    for(int k=0,l=0;k<files.size();k++){
        m_AES_256->setIV(files[k]);
        for(int i=0;i<pos.size();i++,l++){
            XPIRcSequential::REPLY reply = readReply();
            char* response=replyExtraction(reply,container[l]);

            //Tools::printCharArray(response+max_bytesize*((pos[i].first)%(container[l]->getAlpha())),736*8);
            if(!checkContent(response,data_hash_bytes)) check=false;
            //if(!checkContent(response+max_bytesize*((pos[i].first)%(container[l]->getAlpha())),data_hash_bytes)) check=false;

            delete[] response;
            container[l]->cleanup();
            delete container[l];
        }
    }

    //#-------CLEANUP PHASE--------#
    delete m_AES_256;
    delete m_SHA_256;

    return check;
}
