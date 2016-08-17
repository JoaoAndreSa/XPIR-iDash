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
void PIRServerPipeline::downloadWorker(XPIRcPipeline* xpir){
    double total=0;

    //Allocate an array with d dimensions with pointers to arrays of n[i] lwe_query elements 
    xpir->getRGenerator()->initQueriesBuffer();

    uint64_t num_queries=0;
    uint64_t total_bytes=0;

    for(unsigned int j=0; j<xpir->getD(); j++){
        //Compute and allocate the size in bytes of a query element of dimension j
        unsigned int message_length=xpir->getQsize(j+1);

        double start_t = omp_get_wtime();
        for (unsigned int i=0; i<xpir->getN()[j]; i++){
            if (i==0 && j == 0){
                cout << "PIRServer: Starting query element reception"  << endl;
            }

            char* recvBuf = new char[message_length];

            // Get a query element
            m_socket.readXBytes(message_length,(void*)recvBuf);
            xpir->getRGenerator()->pushQuery(recvBuf,message_length,j,i);
            num_queries++;
        }
        double end_t = omp_get_wtime();
        total+=end_t-start_t;
        total_bytes+=xpir->getN()[j]*message_length;
    }
    if(Constants::bandwith_limit!=0) m_socket.sleepForBytes(total_bytes,total);

    // All the query elements received, unlock reply generation
    xpir->getRGenerator()->mutex.unlock();
    cout << "PIRServer: Finish query element reception" << endl;
}

/**
    Send PIR's result, asynchronously.

    @param
    @return
*/
void PIRServerPipeline::uploadWorker(XPIRcPipeline* xpir){
    double total=0;

    GenericPIRReplyGenerator* generator=xpir->getRGenerator();

    // Ciphertext byte size
    unsigned int length=xpir->getRsize(xpir->getD());

    // Number of ciphertexts in the reply
    unsigned long reply_nbr=generator->computeReplySizeInChunks(xpir->getDB()->getmaxFileBytesize());

    // For each ciphertext in the reply_nbr
    for(unsigned i=0; i<reply_nbr; i++){
        while(generator->repliesArray == NULL || generator->repliesArray[i] == NULL){
            boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        }
        // Pointer for the ciphertexts to be sent
        char* ptr = generator->repliesArray[i];

        // Send it
        double start_t = omp_get_wtime();
        m_socket.sendXBytes(length,(void*)ptr);
        double end_t = omp_get_wtime();

        total+=end_t-start_t;
        // Free its memory
        free(ptr);
        generator->repliesArray[i]=NULL;
    }
    double start_t = omp_get_wtime();
    if(Constants::bandwith_limit!=0) m_socket.sleepForBytes(length*reply_nbr,total);
    double end_t = omp_get_wtime();
    total+=end_t-start_t;

    cout << "PIRServer: " << reply_nbr << " reply elements (" << length*reply_nbr << " bytes) sent in " << total << " seconds" << endl;
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
                (*m_imported_dbs).clear();
                std::vector<string> files = Tools::listFilesFolder("db/");
                for(int i=0;i<files.size();i++){
                    m_imported_dbs->operator[](files[i]) = XPIRcPipeline::import_database(files[i]);
                }
                m_socket.sendInt(1);
            }catch(int e){
                cout << "Error while importing files" << e << '\n';
                m_socket.sendInt(0);
            }
       }else{
            m_socket.sendInt(1);
       }
    }else{
        char* list = m_socket.readChar(m_socket.readInt());
        vector<string> list_clients =  Tools::tokenize(string(list),",");

        int num_variants = m_socket.readInt();

        for(int k=0;k<list_clients.size();k++){
            DBDirectoryProcessor db(Constants::num_entries,list_clients[k]);
            for(int i=0;i<num_variants;i++){
                XPIRcPipeline* xpir;
                if(Constants::pre_import){
                    xpir = new XPIRcPipeline(Tools::readParamsPIR(Constants::num_entries),0,&db,m_imported_dbs->operator[](list_clients[k]));
                }else{
                    imported_database_t garbage;
                    xpir = new XPIRcPipeline(Tools::readParamsPIR(Constants::num_entries),0,&db,garbage);
                }

                //#-------QUERY PHASE--------#
                // This is just a download thread. Reply generation is unlocked (by a mutex) when this thread finishes.
                boost::thread downThread = boost::thread(&PIRServerPipeline::downloadWorker, this, xpir);      //thread for uploading reply

                //#-------REPLY PHASE--------#
                /**
                    Start reply generation when mutex unlocked.
                    Start a thread which uploads the reply as it is generated.
                */
                boost::thread upThread = boost::thread(&PIRServerPipeline::uploadWorker, this, xpir);         //thread for downloading query

                /**
                    Generate reply once unlocked by the query downloader thread.
                    If we got a preimported database generate reply directly from it.
                */
                if(Constants::pre_import){
                    xpir->getRGenerator()->generateReplyGenericFromData(m_imported_dbs->operator[](list_clients[k]));
                }else{
                    xpir->setImportedDB(xpir->getRGenerator()->generateReplyGeneric(false));
                }

                if(downThread.joinable()) downThread.join();
                if(upThread.joinable()) upThread.join();

                //#-------CLEANUP PHASE--------#
                xpir->cleanup();
                delete xpir;
            }
        }

        delete[] list;
    }

    m_socket.closeSocket();
    std::cout << "THREAD [" << m_id << "] EXITED" << "\n";
}