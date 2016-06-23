#include "PIRServer.hpp"

static vector<std::thread> pool;
static int num_t=0;

void removeDB(){
    std::system("exec rm -rf db/*");
}

void sleepForBytes(unsigned int bytes) {
    uint64_t seconds=(bytes*8)/Constants::bandwith_limit;
    uint64_t nanoseconds=((((double)bytes*8.)/(double)Constants::bandwith_limit)-(double)seconds)*1000000000UL;

    struct timespec req={0},rem={0};
    req.tv_sec=seconds;
    req.tv_nsec=nanoseconds;

    nanosleep(&req,&rem);
}

// This assumes buffer is at least x bytes long,
// and that the socket is blocking.
void readXBytes(int socket, uint64_t x, void* buffer){
    int bytesRead = 0;
    while (bytesRead < x){
        unsigned int result = read(socket, ((uint8_t*)buffer)+bytesRead, x - bytesRead); PIRServer::errorReadSocket(result<0);
        bytesRead += result;
    }
}

int readInt(int connFd){
    int v=0;
    readXBytes(connFd, sizeof(int), (void*)(&v));

    return static_cast<int>(ntohl(v));
}

long readLong(int connFd){
    long v=0;
    readXBytes(connFd, sizeof(long), (void*)(&v));

    return static_cast<long>(ntohl(v));
}

unsigned int readuInt(int connFd){
    unsigned int v=0;
    readXBytes(connFd, sizeof(unsigned int), (void*)(&v));

    return static_cast<unsigned int>(ntohl(v));
}

uint32_t readuInt32(int connFd){
    uint32_t v=0;
    readXBytes(connFd, sizeof(uint32_t), (void*)(&v));

    return static_cast<uint32_t>(ntohl(v));
}

uint64_t readuInt64(int connFd){
    uint64_t v=0;
    readXBytes(connFd,sizeof(uint64_t), (void*)(&v));

    return static_cast<uint64_t>(ntohl(v));
}

vector<char*> readVector_s(int connFd){
    vector<char*> vector_s;

    uint64_t size=readuInt64(connFd);

    double start = omp_get_wtime();
    for(uint64_t j=1; j<=size; j++){
        uint32_t message_length=readuInt32(connFd);

        unsigned int n_size=readuInt(connFd);
        for(uint64_t i=0; i<n_size;i++){
            char* buffer = new char[message_length];
            readXBytes(connFd,message_length,(void*)buffer);
            vector_s.push_back(buffer);
        }
    }
    double end = omp_get_wtime();
    cout << "SimplePIR: Send query took " << end-start << " seconds" << endl;
    return vector_s;
}

char* readCiphertext(int buflen, int connFd){
    char* recvBuff = new char[buflen];
    readXBytes(connFd,buflen,(void*)recvBuff);
    return recvBuff;
}

char* readPlaintext(int buflen, int connFd){
    char* recvBuff = new char[buflen+1];
    readXBytes(connFd,buflen,(void*)recvBuff);    
    recvBuff[buflen]='\0';
    return recvBuff;

}

uint64_t downloadData(int connFd, int tid){
    /* Erase data in db folder */
    removeDB();

    /* Receive data in chunks of 256 bytes */
    int buflen=1;
    uint64_t entry=0;

    double start = omp_get_wtime();
    while(buflen!=0){  
        buflen=readInt(connFd); if(buflen==0){break;}
        char* recvBuff;

        //if ciphertext
        recvBuff=readCiphertext(buflen,connFd);
        //------ ### ------    

        //if plaintext
        //recvBuff=readPlaintext(buflen,connFd);
        //------ ### ------    

        /* Create file where entry will be stored */
        ostringstream oss;
        oss << entry;

        ofstream f("db/"+oss.str(), ios::out|ios::binary);

        PIRServer::error(tid,f==nullptr || f.is_open()==0,"Error reading file");
        if(f.is_open()){
            f.write(recvBuff,buflen);
        }

        f.close();
        delete[] recvBuff;
        entry++;
    }
    double end = omp_get_wtime();
    std::cout << "Received file from client. It took " << end-start << " (s)\n";

    return entry;
}

void cleanupVector(vector<char*> v){
    for(uint64_t i=0;i<v.size();i++){
        delete[] v[i];
    }
}

void cleanup(XPIRc* xpir){
    xpir->cleanup();
    delete xpir;
}

// This assumes buffer is at least x bytes long,
// and that the socket is blocking.
void sendXBytes(int socket, uint64_t x, void* buffer){
    int bytesWrite = 0;
    while (bytesWrite < x){
        int result = write(socket, ((uint8_t*)buffer)+bytesWrite, x - bytesWrite); PIRServer::errorWriteSocket(result<0);
        bytesWrite += result;

        if(Constants::bandwith_limit!=0) sleepForBytes(result);
    }
}

void senduInt64(uint64_t integer,int connFd){
    uint64_t v = htonl(integer);
    sendXBytes(connFd,sizeof(uint64_t),(void*)(&v));
}

void senduInt32(uint32_t integer,int connFd){
    uint32_t v = htonl(integer);
    sendXBytes(connFd,sizeof(uint32_t),(void*)(&v));
}

void sendVector_s(vector<char*> vector_c, int connFd, uint32_t length){
    senduInt64(static_cast<uint64_t>(vector_c.size()),connFd);
    senduInt32(length,connFd);
    
    for (uint64_t i=0; i<vector_c.size(); i++) {
        sendXBytes(connFd,length,(void*)vector_c[i]);
    }
    //cout << "SimplePIR: Send reply took " << end-start << " seconds" << endl;
}

void sendReply(XPIRc::REPLY reply,int connFd,uint32_t size){
    senduInt64(reply.nbRepliesGenerated,connFd);
    senduInt64(reply.aggregated_maxFileSize,connFd);
    sendVector_s(reply.reply,connFd,size);
}

int verifyParams(uint64_t num_entries, uint64_t d, uint64_t alpha, unsigned int* n){
    int total=alpha;

    for(int i=0;i<d;i++){
        total*=n[i];
    }

    if(total<num_entries) return 1;
    else return 0;
}

/* Check for errors on paramsPIR.txt file (e.g. dimension>4 || alpha<1...) */
PIRParameters readParamsPIR(uint64_t num_entries, int tid){
    std::string line;
    PIRParameters params;

    ifstream f("../Constants/paramsPIR.txt");

    PIRServer::error(tid,f==NULL || f.is_open()==0,"Error reading file");
    if (f.is_open()){
        getline(f,line);
        params.d=atoi(line.c_str());

        getline(f,line);
        params.alpha=atoi(line.c_str());

        for(int i=0;i<4;i++){
            getline(f,line);
            params.n[i]=atoi(line.c_str());
        }
        PIRServer::error(tid,params.d<1 || params.d>4 || params.alpha<1 || params.alpha>num_entries || verifyParams(num_entries,params.d,params.alpha,params.n),"Wrong PIR parameters");

        getline(f,line);
        params.crypto_params=line;
    }
    f.close();

    return params;
}

XPIRc* initXPIR(uint64_t num_entries, int tid){
    uint64_t database_size, nb_files, maxFileBytesize;
    PIRParameters params; 

    DBDirectoryProcessor db;
    nb_files=db.getNbStream();
    database_size = db.getDBSizeBits();
    maxFileBytesize = database_size/nb_files;

    return new XPIRc(readParamsPIR(num_entries,tid),0,&db);
}

void job (int tid, int connFd){
	std::cout << "THREAD [" << tid << "]" << "\n";

    //read file from client
	uint64_t num_entries=downloadData(connFd,tid);

    XPIRc* xpir=initXPIR(num_entries,tid);

    vector<char*> query=readVector_s(connFd);

    XPIRc::REPLY reply=xpir->replyGeneration(query);
    sendReply(reply,connFd,xpir->getQsize(xpir->getD()));
    std::cout << "SimplePIR: Reply sent" << "\n";

    xpir->freeQueries();
    cleanupVector(reply.reply);
    cleanup(xpir);
    std::cout << "THREAD [" << tid << "] EXITED" << "\n";
}

void starthreads(PIRServer s){
   
    while (1){
        std::cout << "Listening" << "\n";

        //this is where client connects. svr will hang in this mode until client connects
        struct sockaddr_in clntAdd = s.getClntAdd();
        socklen_t len = s.getLen();

        int connFd = accept(s.getListenFd(), (struct sockaddr *)&clntAdd, &len);
        PIRServer::errorExit(connFd<0,"Cannot accept connection");
        std::cout << "Connection successful" << "\n";
        
        pool.push_back(std::thread(job,num_t,connFd));
        num_t++;
    }
    
    for(int i=0;i<pool.size();i++){
    	pool[i].join();
    }
}


int main(int argc, char* argv[]){    
    PIRServer s(Constants::port,Constants::n_threads,Constants::max_connects);
   	starthreads(s);

    return 0;
}