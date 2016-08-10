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
    double start_t,end_t,total=0;

    vector<char*> vector_s;
    uint64_t size=m_socket.readuInt64();

    uint64_t total_bytes=0;
    for(uint64_t j=1; j<=size; j++){
        uint32_t message_length=m_socket.readuInt32();

        unsigned int n_size=m_socket.readuInt();

        start_t = omp_get_wtime();
        for(uint64_t i=0; i<n_size;i++){
            char* buffer = new char[message_length];
            m_socket.readXBytes(message_length,(void*)buffer);

            vector_s.push_back(buffer);
        }
        end_t = omp_get_wtime();

        total+=end_t-start_t;
        total_bytes+=message_length*n_size;
    }
    if(Constants::bandwith_limit!=0) m_socket.sleepForBytes(sizeof(uint64_t)+sizeof(uint64_t)+sizeof(uint64_t)+((sizeof(uint32_t)+sizeof(int))*size)+total_bytes,total);
    return vector_s;
}

/**
    Send a char* vector through socket (in other words send reply data).

    @param vector_c reply

    @return
*/
void PIRServerSequential::sendVector_s(vector<char*> vector_c){
    double start_t,end_t,total=0;

    m_socket.senduInt64(static_cast<uint64_t>(vector_c.size()));
    uint32_t length=m_xpir->getRsize(m_xpir->getD());
    m_socket.senduInt32(length);

    start_t = omp_get_wtime();
    for(uint64_t i=0; i<vector_c.size(); i++){
        m_socket.sendXBytes(length,(void*)vector_c[i]);
    }
    end_t = omp_get_wtime();
    total+=end_t-start_t;
    if(Constants::bandwith_limit!=0) m_socket.sleepForBytes(sizeof(uint64_t)+sizeof(uint64_t)+sizeof(uint64_t)+sizeof(uint32_t)+length*vector_c.size(),total);
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
    if(m_socket.readInt()==1){
	   downloadData();
       if(Constants::pre_import){
            if(Constants::pre_import){
                try{
                    for (auto const& x : (*m_imported_dbs)){
                        delete x.second;
                    }
                    (*m_imported_dbs).clear();
                    std::vector<string> files = Tools::listFilesFolder("db/");
                    for(int i=0;i<files.size();i++){
                        m_imported_dbs->operator[](files[i]) = XPIRcSequential::import_database(files[i]);
                    }
                    m_socket.sendInt(1);
                }catch(int e){
                    cout << "Error while importing files" << e << '\n';
                    m_socket.sendInt(0);
                }
           }
       }
    }else{
        char* list = m_socket.readChar(m_socket.readInt());
        vector<string> list_clients =  Tools::tokenize(string(list),",");

        DBDirectoryProcessor db(Constants::num_entries,list_clients[0]);

        if(Constants::pre_import && m_imported_dbs->operator[](list_clients[0])!=nullptr){
            m_xpir = new XPIRcSequential(Tools::readParamsPIR(Constants::num_entries),0,&db,m_imported_dbs->operator[](list_clients[0]),list_clients[0]);
        }
        else{
            m_xpir = new XPIRcSequential(Tools::readParamsPIR(Constants::num_entries),0,&db,nullptr,list_clients[0]);
        }

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
    }
    m_socket.closeSocket();
    std::cout << "THREAD [" << m_id << "] EXITED" << "\n";
}