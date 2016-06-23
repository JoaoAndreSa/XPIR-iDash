#include "PIRServer.hpp"

//***PRIVATE METHODS***//
// This assumes buffer is at least x bytes long,
// and that the socket is blocking.
void PIRServer::readXBytes(uint64_t x, void* buffer){
    int bytesRead = 0;
    while (bytesRead < x){
        unsigned int result = read(m_connFd, ((uint8_t*)buffer)+bytesRead, x - bytesRead); errorReadSocket(result<0);
        bytesRead += result;
    }
}

int PIRServer::readInt(){
    int v=0;
    readXBytes(sizeof(int), (void*)(&v));

    return static_cast<int>(ntohl(v));
}

unsigned int PIRServer::readuInt(){
    unsigned int v=0;
    readXBytes(sizeof(unsigned int), (void*)(&v));

    return static_cast<unsigned int>(ntohl(v));
}

uint32_t PIRServer::readuInt32(){
    uint32_t v=0;
    readXBytes(sizeof(uint32_t), (void*)(&v));

    return static_cast<uint32_t>(ntohl(v));
}

uint64_t PIRServer::readuInt64(){
    uint64_t v=0;
    readXBytes(sizeof(uint64_t), (void*)(&v));

    return static_cast<uint64_t>(ntohl(v));
}

char* PIRServer::readCiphertext(int buflen){
    char* recvBuff = new char[buflen];
    readXBytes(buflen,(void*)recvBuff);
    return recvBuff;
}

char* PIRServer::readPlaintext(int buflen){
    char* recvBuff = new char[buflen+1];
    readXBytes(buflen,(void*)recvBuff);
    recvBuff[buflen]='\0';
    return recvBuff;

}

/* Check for errors on paramsPIR.txt file (e.g. dimension>4 || alpha<1...) */
PIRParameters PIRServer::readParamsPIR(){
    std::string line;
    PIRParameters params;

    ifstream f("../Constants/paramsPIR.txt");

    PIRServer::error(m_id,f==NULL || f.is_open()==0,"Error reading file");
    if (f.is_open()){
        getline(f,line);
        params.d=atoi(line.c_str());

        getline(f,line);
        params.alpha=atoi(line.c_str());

        for(int i=0;i<4;i++){
            getline(f,line);
            params.n[i]=atoi(line.c_str());
        }
        PIRServer::error(m_id,params.d<1 || params.d>4 || params.alpha<1 || params.alpha>m_num_entries || verifyParams(params.d,params.alpha,params.n),"Wrong PIR parameters");

        getline(f,line);
        params.crypto_params=line;
    }
    f.close();

    return params;
}

int PIRServer::verifyParams(uint64_t d, uint64_t alpha, unsigned int* n){
    int total=alpha;

    for(int i=0;i<d;i++){
        total*=n[i];
    }

    if(total<m_num_entries) return 1;
    else return 0;
}

void PIRServer::removeDB(){
    std::system("exec rm -rf db/*");
}

void PIRServer::downloadData(){
    /* Erase data in db folder */
    removeDB();

    /* Receive data in chunks of 256 bytes */
    int buflen=1;
    uint64_t entry=0;

    double start = omp_get_wtime();
    while(buflen!=0){  
        buflen=readInt(); if(buflen==0){break;}
        char* recvBuff;

        //if ciphertext
        recvBuff=readCiphertext(buflen);
        //------ ### ------    

        //if plaintext
        //recvBuff=readPlaintext(buflen,m_connFd);
        //------ ### ------    

        //Create file where entry will be stored 
        ostringstream oss;
        oss << entry;

        ofstream f("db/"+oss.str(), ios::out|ios::binary);

        PIRServer::error(m_id,f==nullptr || f.is_open()==0,"Error reading file");
        if(f.is_open()){
            f.write(recvBuff,buflen);
        }

        f.close();
        delete[] recvBuff;
        entry++;
    }
    double end = omp_get_wtime();
    std::cout << "Received file from client. It took " << end-start << " (s)\n";

    m_num_entries=entry;
}

void PIRServer::sleepForBytes(unsigned int bytes){
    uint64_t seconds=(bytes*8)/Constants::bandwith_limit;
    uint64_t nanoseconds=((((double)bytes*8.)/(double)Constants::bandwith_limit)-(double)seconds)*1000000000UL;

    struct timespec req={0},rem={0};
    req.tv_sec=seconds;
    req.tv_nsec=nanoseconds;

    nanosleep(&req,&rem);
}

// This assumes buffer is at least x bytes long,
// and that the socket is blocking.
void PIRServer::sendXBytes(uint64_t x, void* buffer){
    int bytesWrite = 0;
    while (bytesWrite < x){
        int result = write(m_connFd, ((uint8_t*)buffer)+bytesWrite, x - bytesWrite); errorWriteSocket(result<0);
        bytesWrite += result;

        if(Constants::bandwith_limit!=0) sleepForBytes(result);
    }
}

void PIRServer::senduInt64(uint64_t integer){
    uint64_t v = htonl(integer);
    sendXBytes(sizeof(uint64_t),(void*)(&v));
}

void PIRServer::senduInt32(uint32_t integer){
    uint32_t v = htonl(integer);
    sendXBytes(sizeof(uint32_t),(void*)(&v));
}

void PIRServer::cleanupVector(vector<char*> v){
    for(uint64_t i=0;i<v.size();i++){
        delete[] v[i];
    }
}