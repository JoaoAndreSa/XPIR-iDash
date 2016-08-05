/**
    XPIR-hash
    PIRServerPipeline.cpp
    Purpose: Child class that binds to each server thread and executes pipeline PIR.
             NOTE: In pipeline PIR, server does not have to wait for all query elements to start the reply generation.

    @author Marc-Olivier Killijian, Carlos Aguillar & Joao Sa
    @version 1.0 01/07/16
*/

/**

                  PIRServer
                      |
           ----------- -----------
           |                     |
  PIRServerSequential     PIRServerPipeline(*)

*/

#include "PIRServerPipeline.hpp"

//***PRIVATE METHODS***//
/**
    Receive queries n messages with n = nbr of files.

    @param
    @return
*/
void PIRServerPipeline::downloadWorker(){
    double start_t,end_t,total=0;

    //Allocate an array with d dimensions with pointers to arrays of n[i] lwe_query elements 
    m_xpir->getRGenerator()->initQueriesBuffer();

    uint64_t num_queries=0;
    uint64_t total_bytes=0;
    for(unsigned int j=0; j<m_xpir->getD(); j++){
        //Compute and allocate the size in bytes of a query element of dimension j
        unsigned int message_length=m_xpir->getQsize(j+1);

        start_t = omp_get_wtime();
        for (unsigned int i=0; i<m_xpir->getN()[j]; i++){
            if (i==0 && j == 0){
                cout << "PIRServer: Starting query element reception"  << endl;
            }

            char* recvBuf = new char[message_length];

            // Get a query element
            m_socket.readXBytes(message_length,(void*)recvBuf);
            m_xpir->getRGenerator()->pushQuery(recvBuf,message_length,j,i);
            num_queries++;
        }
        end_t = omp_get_wtime();
        total+=end_t-start_t;
        total_bytes+=m_xpir->getN()[j]*message_length;
    }
    if(Constants::bandwith_limit!=0) m_socket.sleepForBytes(total_bytes,total);

    // All the query elements received, unlock reply generation
    m_xpir->getRGenerator()->mutex.unlock();
    cout << "PIRServer: Finish query element reception" << endl;
}

/**
    Send PIR's result, asynchronously.

    @param
    @return
*/
void PIRServerPipeline::uploadWorker(){
    double start = omp_get_wtime(),start_t,end_t,total=0;


    GenericPIRReplyGenerator* generator=m_xpir->getRGenerator();

    // Ciphertext byte size
    unsigned int length=m_xpir->getRsize(m_xpir->getD());
    uint64_t bytes_sent=0;

    // Number of ciphertexts in the reply
    unsigned long reply_nbr=generator->computeReplySizeInChunks(m_xpir->getDB()->getmaxFileBytesize()),i=0;

    // Pointer for the ciphertexts to be sent
    char *ptr;

    // For each ciphertext in the reply_nbr
    for(unsigned i=0; i<reply_nbr; i++){
        while(generator->repliesArray == NULL || generator->repliesArray[i] == NULL){
            boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        }
        ptr=generator->repliesArray[i];

        // Send it
        start_t = omp_get_wtime();
        m_socket.sendXBytes(length,(void*)ptr);
        bytes_sent+=length;
        end_t = omp_get_wtime();

        total+=end_t-start_t;
        // Free its memory
        free(ptr);
        generator->repliesArray[i]=NULL;
    }
    if(Constants::bandwith_limit!=0) m_socket.sleepForBytes(length*reply_nbr,total);

    double end = omp_get_wtime();
    cout << "PIRServer: " << reply_nbr << " reply elements sent in " << end-start << " seconds" << endl;
    std::cout << "PIRServer: Reply sent" << "\n";
}

/**
    What the thread executes (binding function).

    @param
    @return
*/
void PIRServerPipeline::job (){
    m_id = boost::this_thread::get_id();
    std::cout << "THREAD [" << m_id << "]" << "\n";

    //#-------SETUP PHASE--------#
    //read file from client
    if(m_socket.readInt()==1){
       downloadData();
       if(Constants::pre_import){
            try{
                std::vector<string> files = Tools::listFilesFolder("db/");
                for(int i=0;i<files.size();i++){
                    if((*m_imported_dbs).find(files[i]) == (*m_imported_dbs).end()){
                        m_imported_dbs->operator[](files[i]) = XPIRcPipeline::import_database(files[i]);
                    }
                }
                m_socket.sendInt(1);
            }catch(int e){
                cout << "Error while importing files" << e << '\n';
                m_socket.sendInt(0);
            }
       }
    }else{
        char* list = m_socket.readChar(m_socket.readInt());
        vector<string> list_clients =  Tools::tokenize(string(list),",");

        boost::thread upThread;       //thread for uploading reply
        boost::thread downThread;     //thread for downloading query

        DBDirectoryProcessor db(Constants::num_entries,list_clients[0]);
        if(Constants::pre_import){
            m_xpir = new XPIRcPipeline(Tools::readParamsPIR(Constants::num_entries),0,&db,m_imported_dbs->operator[](list_clients[0]));
        }else{
            imported_database_t garbage;
            m_xpir = new XPIRcPipeline(Tools::readParamsPIR(Constants::num_entries),0,&db,garbage);
        }

        //#-------QUERY PHASE--------#
        // This is just a download thread. Reply generation is unlocked (by a mutex) when this thread finishes.
        downThread = boost::thread(&PIRServerPipeline::downloadWorker, this);

        //#-------REPLY PHASE--------#
        /**
            Start reply generation when mutex unlocked.
            Start a thread which uploads the reply as it is generated.
        */
        upThread = boost::thread(&PIRServerPipeline::uploadWorker, this);

        /**
            Generate reply once unlocked by the query downloader thread.
            If we got a preimported database generate reply directly from it.
        */
        if(Constants::pre_import){
            m_xpir->getRGenerator()->generateReplyGenericFromData(m_imported_dbs->operator[](list_clients[0]));
        }else{
            m_xpir->setImportedDB(m_xpir->getRGenerator()->generateReplyGeneric(true));
        }

        // Wait for child threads
        if (upThread.joinable())  upThread.join();
        if (downThread.joinable()) downThread.join();

        //#-------CLEANUP PHASE--------#
        // When everything is sent, clean 'tools' and close the socket
        m_xpir->cleanup();
        delete list;
        delete m_xpir;
    }

    m_socket.closeSocket();
    std::cout << "THREAD [" << m_id << "] EXITED" << "\n";
}