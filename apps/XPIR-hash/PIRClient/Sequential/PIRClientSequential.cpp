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
    double start = omp_get_wtime(), start_t, end_t, total=0;

    uint64_t pos=0;
    m_socket.senduInt64(m_xpir->getD());                //number of dimensions

    uint64_t total_bytes=0;
    for(uint64_t j=1 ; j<=m_xpir->getD(); j++){
        uint32_t length=m_xpir->getQsize(j);            //query size (depends on the dimension)
        m_socket.senduInt32(length);
        m_socket.senduInt(m_xpir->getN()[j-1]);         //number of elements in that dimension

        start_t = omp_get_wtime();
        for (uint64_t i=0; i<m_xpir->getN()[j-1]; i++){ //for each element in that dimension
            m_socket.sendXBytes(length,(void*)query[pos]);
            pos++;

            total_bytes+=length;
        }
        end_t = omp_get_wtime();

        total+=end_t-start_t;
    }
     if(Constants::bandwith_limit!=0) m_socket.sleepForBytes(sizeof(uint64_t) + m_xpir->getD()*(sizeof(uint32_t)+sizeof(int)) + total_bytes,total);

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
char* PIRClientSequential::replyExtraction(XPIRcSequential::REPLY reply){
	char* response;
    response=m_xpir->replyExtraction(reply);

    return response;
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
    int max_bytesize = getInfoVCF(Tools::tokenize(entry['f'],",")[0]);
    uint64_t pack_pos=considerPacking(pos[0].first,m_xpir->getAlpha());

    //#-------QUERY PHASE--------#
    m_socket.sendInt(entry['f'].length()+1);
    m_socket.sendChar_s(const_cast<char*>(entry['f'].c_str()),entry['f'].length()+1);

    std::vector<char*> query=queryGeneration(pack_pos);
    sendQuery(query);
    std::cout << "PIRClient: Query sent" << "\n";

    //#-------REPLY PHASE--------#
    XPIRcSequential::REPLY reply = readReply();
    char* response;
    response=replyExtraction(reply);

    string response_s;
    if(!Constants::encrypted){   //if PLAINTEXT
        response_s = extractPlaintext(response,m_xpir->getAlpha(),reply.maxFileSize,pos[0].first,pos[0].second);
    }else{                       //if CIPHERTEXT
        response_s = extractCiphertext(response,m_xpir->getAlpha(),reply.maxFileSize,pos[0].first,pos[0].second);
    }

    bool check=true;
    for(int i=0;i<pos[0].second.size();i++){
        if(m_SHA_256->search(pos[0].second[i],response_s)==false){
            check=false;
        }
    }

    //#-------CLEANUP PHASE--------#
    m_xpir->cleanQueryBuffer();
    Tools::cleanupVector(query);
    delete[] response;
    delete m_SHA_256;
    delete m_xpir;

    return check;
}
