#include "PIRServerSequential.hpp"

//***PRIVATE METHODS***//
vector<char*> PIRServerSequential::readVector_s(){
    vector<char*> vector_s;

    uint64_t size=readuInt64();

    double start = omp_get_wtime();
    for(uint64_t j=1; j<=size; j++){
        uint32_t message_length=readuInt32();

        unsigned int n_size=readuInt();
        for(uint64_t i=0; i<n_size;i++){
            char* buffer = new char[message_length];
            readXBytes(message_length,(void*)buffer);
            vector_s.push_back(buffer);
        }
    }
    double end = omp_get_wtime();
    cout << "SimplePIR: Send query took " << end-start << " seconds" << endl;
    return vector_s;
}

void PIRServerSequential::sendVector_s(vector<char*> vector_c, uint32_t length){
    senduInt64(static_cast<uint64_t>(vector_c.size()));
    senduInt32(length);
    
    for (uint64_t i=0; i<vector_c.size(); i++) {
        sendXBytes(length,(void*)vector_c[i]);
    }
}

void PIRServerSequential::sendReply(XPIRcSequential::REPLY reply,uint32_t size){
    senduInt64(reply.nbRepliesGenerated);
    senduInt64(reply.aggregated_maxFileSize);
    sendVector_s(reply.reply,size);
}

void PIRServerSequential::job (){
	std::cout << "THREAD [" << m_id << "]" << "\n";

    //#-------SETUP PHASE--------#
    //read file from client
	downloadData();
    DBDirectoryProcessor db;
    m_xpir = new XPIRcSequential(readParamsPIR(),0,&db);

    //#-------QUERY PHASE--------#
    vector<char*> query=readVector_s();

    XPIRcSequential::REPLY reply=m_xpir->replyGeneration(query);
    sendReply(reply,m_xpir->getQsize(m_xpir->getD()));
    std::cout << "SimplePIR: Reply sent" << "\n";

    m_xpir->freeQueries();
    cleanupVector(reply.reply);
    m_xpir->cleanup();
    delete m_xpir;
    close(m_connFd);
    std::cout << "THREAD [" << m_id << "] EXITED" << "\n";
}