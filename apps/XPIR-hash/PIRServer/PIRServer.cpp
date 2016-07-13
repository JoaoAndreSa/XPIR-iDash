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

    /* Receive data in chunks of 256 bytes */
    int buflen=1;
    uint64_t entry=0;

    while(buflen!=0){ 
        buflen=m_socket.readInt(); if(buflen==0){break;}
        char* recvBuff;

        if(!Constants::encrypted){   //if PLAINTEXT
            recvBuff=m_socket.readChar_s(buflen);
        }else{                       //if CIPHERTEXT
            recvBuff=m_socket.readChar(buflen);
        }

        //Create file where entry will be stored 
        ostringstream oss;
        oss << entry;

        try{

            ofstream f("db/"+oss.str(), ios::out|ios::binary);

            Tools::error(f==nullptr || f.is_open()==0,"Error writing DB file");
            if(f.is_open()){
                f.write(recvBuff,buflen);
            }
            f.close();

        }catch (std::ios_base::failure &fail){
            Tools::error(1,"Error writing DB file");
        }

        delete[] recvBuff;
        entry++;
    }

    m_num_entries=entry;
}