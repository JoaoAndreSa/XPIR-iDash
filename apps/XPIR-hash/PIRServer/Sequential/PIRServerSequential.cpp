/**
    XPIR-hash
    PIRServerSequential.cpp
    Purpose: Child class that binds to each server thread and executes sequential PIR.
             NOTE: In sequential PIR, server has to wait and get all query elements before starting the reply generation.

    @author Joao Sa
    @version 1.0 01/07/16
*/

/**

                  PIRServer
                      |
           ----------- -----------
           |                     |
  PIRServerSequential(*)  PIRServerPipeline

*/

#include "PIRServerSequential.hpp"

//***PRIVATE METHODS***//
/**
    Read a char* vector from socket (in other words read query array).

    @param

    @return vector_s query
*/
vector<char*> PIRServerSequential::readVector_s(){
    vector<char*> vector_s;

    uint64_t size=m_socket.readuInt64();

    for(uint64_t j=1; j<=size; j++){
        uint32_t message_length=m_socket.readuInt32();

        unsigned int n_size=m_socket.readuInt();
        for(uint64_t i=0; i<n_size;i++){
            char* buffer = new char[message_length];
            m_socket.readXBytes(message_length,(void*)buffer);
            vector_s.push_back(buffer);
        }
    }
    return vector_s;
}

/**
    Send a char* vector through socket (in other words send reply data).

    @param vector_c reply

    @return
*/
void PIRServerSequential::sendVector_s(vector<char*> vector_c){
    m_socket.senduInt64(static_cast<uint64_t>(vector_c.size()));
    uint32_t length=m_xpir->getRsize(m_xpir->getD());
    m_socket.senduInt32(length);

    for (uint64_t i=0; i<vector_c.size(); i++) {
        m_socket.sendXBytes(length,(void*)vector_c[i]);
    }
}

/**
    Sends all reply parameters: data, nbRepliesGenerated, maxFileSize).

    @param reply container struct with all reply data

    @return
*/
void PIRServerSequential::sendReply(XPIRcSequential::REPLY reply){
    double start = omp_get_wtime();

    m_socket.senduInt64(reply.nbRepliesGenerated);
    m_socket.senduInt64(reply.maxFileSize);
    sendVector_s(reply.reply);

    double end = omp_get_wtime();
    cout << "PIRServer: Send reply took " << end-start << " seconds" << endl;
    std::cout << "PIRServer: Reply sent" << "\n";
}

/**
    What the thread executes (binding function).

    @param
    @return
*/
void PIRServerSequential::job (){
    m_id = boost::this_thread::get_id();
	std::cout << "THREAD [" << m_id << "]" << "\n";

    //#-------SETUP PHASE--------#
    //read file from client
	downloadData();
    DBDirectoryProcessor db;
    m_xpir = new XPIRcSequential(Tools::readParamsPIR(m_num_entries),0,&db);

    //#-------QUERY PHASE--------#
    vector<char*> query=readVector_s();

    //#-------REPLY PHASE--------#
    XPIRcSequential::REPLY reply=m_xpir->replyGeneration(query);
    sendReply(reply);

    //#-------CLEANUP PHASE--------#
    m_xpir->cleanReplyBuffer();
    Tools::cleanupVector(reply.reply);
    m_xpir->cleanup();
    delete m_xpir;
    m_socket.closeSocket();

    std::cout << "THREAD [" << m_id << "] EXITED" << "\n";
}