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
    Reads ciphertext from socket (does not require to add '\0' to then end).

    @param buflen size of the element to be read.

    @return recvBuff buffer where we store the bytes.
*/
char* PIRServer::readCiphertext(int buflen){
    char* recvBuff = new char[buflen];
    m_socket.readXBytes(buflen,(void*)recvBuff);
    return recvBuff;
}

/**
    Reads plaintext from socket (adds a '\0' to end of the stream).

    @param buflen size of the element to be read.

    @return recvBuff buffer where we store the bytes.
*/
char* PIRServer::readPlaintext(int buflen){
    char* recvBuff = new char[buflen+1];
    m_socket.readXBytes(buflen,(void*)recvBuff);
    recvBuff[buflen]='\0';
    return recvBuff;

}

/**
    Check for errors on paramsPIR.txt file (e.g. dimension>4 || alpha<1...). 
    List of errors checked:
        - 4 < dimension < 1;
        - num_elements in db < alpha < 1;
        - alpha * N[i] (where i=0..d) < num_elements in db (the aggregation and dimension parameters have to fit the amount of 
            data in DB - example: 1000 entries> alpha=10, d=2, N[0]=10, N[1]=10 - 10*10*10>=1000 CORRECT)

    @param

    @return params PIR parameters.
*/
PIRParameters PIRServer::readParamsPIR(){
    std::string line;
    PIRParameters params;

    try{

        ifstream f("../Constants/paramsPIR.txt");

        PIRServer::error(m_id,f==NULL || f.is_open()==0,"Error reading paramsPIR.txt file");
        if (f.is_open()){
            getline(f,line);
            params.d=atoi(line.c_str());

            getline(f,line);
            params.alpha=atoi(line.c_str());

            for(int i=0;i<4;i++){
                getline(f,line);
                params.n[i]=atoi(line.c_str());
            }
            PIRServer::error(m_id,params.d<1 || params.d>4 || params.alpha<1 || params.alpha>m_num_entries || verifyParams(params.d,params.alpha,params.n)==0,"Wrong PIR parameters");

            getline(f,line);
            params.crypto_params=line;
        }
        f.close();

    }catch (std::ios_base::failure &fail){
        PIRServer::error(m_id,1,"Error reading paramsPIR.txt file");
    }

    return params;
}

/**
    Check 3rd error on the previous list of errors - see function readParamsPIR().

    @param

    @return 0/1 0-INCORRECT, 1-CORRECT.
*/
int PIRServer::verifyParams(uint64_t d, uint64_t alpha, unsigned int* n){
    int total=alpha;

    for(int i=0;i<d;i++){
        total*=n[i];
    }

    if(total<m_num_entries) return 0;
    else return 1;
}

/**
    Cleans DB folder to allow for another batch of files sent by the client.

    @param
    @return
*/
void PIRServer::removeDB(){
    std::system("exec rm -rf db/*");
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

    double start = omp_get_wtime();
    while(buflen!=0){ 
        buflen=m_socket.readInt(); if(buflen==0){break;}
        char* recvBuff;

        //if(Constants::encrypted){   //if CIPHERTEXT
        //    recvBuff=m_socket.readChar_s(buflen);
        //}else{                      //if PLAINTEXT
            //recvBuff=m_socket.readChar_s(buflen+1);
            //recvBuff[buflen]='\0';
        //}

        cout << recvBuff << endl;

        //if ciphertext
        //recvBuff=readCiphertext(buflen);
        //------ ### ------    

        //if plaintext
        recvBuff=readPlaintext(buflen);
        //------ ### ------    

        //Create file where entry will be stored 
        ostringstream oss;
        oss << entry;

        try{

            ofstream f("db/"+oss.str(), ios::out|ios::binary);

            PIRServer::error(m_id,f==nullptr || f.is_open()==0,"Error writing DB file");
            if(f.is_open()){
                f.write(recvBuff,buflen);
            }
            f.close();

        }catch (std::ios_base::failure &fail){
            PIRServer::error(m_id,1,"Error writing DB file");
        }

        delete[] recvBuff;
        entry++;
    }
    double end = omp_get_wtime();
    std::cout << "Received file from client. It took " << end-start << " (s)\n";

    m_num_entries=entry;
}

/**
    Free allocated memory.

    @param v vector of char* to be freed

    @return
*/
void PIRServer::cleanupVector(vector<char*> v){
    for(uint64_t i=0;i<v.size();i++){
        delete[] v[i];
    }
}