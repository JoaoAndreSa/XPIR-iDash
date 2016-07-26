/**
    XPIR-hash
    PIRServer.cpp
    Purpose: Parent class (abstract) that binds to each thread. Can have to modes of operation: Sequential or Pipeline (child classes)

    @author Joao Sa
    @version 1.0 01/07/16
*/

/**

                  PIRServer (*)
                      |
           ----------- -----------
           |                     |
    PIRServerSequential   PIRServerPipeline

*/

#include "PIRServer.hpp"

//***PRIVATE METHODS***//
/**
    Cleans DB folder to allow for another batch of files sent by the client.

    @param
    @return
*/
void PIRServer::removeDB(){
    int ret_val=std::system("exec rm -rf db/*");

    if (ret_val==1){
        cout << "Error performing system call" << endl;
    }
}

/**
    Reads data to be stored by the server

    @param
    @return
*/
void PIRServer::downloadData(){
    /* Erase data in db folder */
    //removeDB();

    uint64_t num_files=m_socket.readuInt64();

    for(uint64_t i=0;i<num_files;i++){
         try{
            double start = omp_get_wtime(),total = 0;

            int len=m_socket.readInt();
            char* filename_c=m_socket.readChar(len);
            filename_c[len]='\0';
            string filename(filename_c);

            if(ifstream("db/"+filename)){
                m_socket.sendInt(1);    //if FILE EXISTS
                continue;
            }else{
                m_socket.sendInt(0);    //if FILE DOES NOT EXISTS
            }

            m_max_bytesize=m_socket.readInt();
            for(uint64_t i=0;i<Constants::num_entries;i++){
                double start_t = omp_get_wtime();
                char* recvBuff=m_socket.readChar(m_max_bytesize);
                double end_t = omp_get_wtime();

                total+=end_t-start_t;

                //Create file where entries will be stored 
                Tools::writeToBinFile("db/"+filename,recvBuff,m_max_bytesize),
                delete[] recvBuff;
            }
            if(Constants::bandwith_limit!=0) m_socket.sleepForBytes(sizeof(uint64_t)+sizeof(int)+(filename.length()+1)*sizeof(char)+sizeof(int)+m_max_bytesize*Constants::num_entries,total);
            double end = omp_get_wtime();
            std::cout << "PIRServer: Reading encrypted file took " << end-start << " (s)\n";
        }catch (std::ios_base::failure &fail){
            Error::error(1,"Error writing DB file");
        }
    }
}