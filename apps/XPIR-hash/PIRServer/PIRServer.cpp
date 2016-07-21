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
    removeDB();

    try{
        /* Receive data in chunks of 256 bytes */
        m_max_bytesize=m_socket.readInt();
        m_num_entries=m_socket.readuInt64();

        for(uint64_t i=0;i<m_num_entries;i++){
            char* recvBuff=m_socket.readChar(m_max_bytesize);

            //Create file where entries will be stored 
            Tools::writeToBinFile("db/db.bin",recvBuff,m_max_bytesize),
            delete[] recvBuff;
        }
    }catch (std::ios_base::failure &fail){
        Tools::error(1,"Error writing DB file");
    }
}