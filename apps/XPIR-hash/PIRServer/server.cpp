#include "Parallel/PIRServerParallel.hpp"
#include "Sequential/PIRServerSequential.hpp"

int createSocket(){
	int listenFd = socket(AF_INET, SOCK_STREAM, 0);
	PIRServer::errorExit(listenFd<0,"Cannot open socket");
	return listenFd;
}

struct sockaddr_in generateServerAddress(struct sockaddr_in svrAdd){
	bzero((char*) &svrAdd, sizeof(svrAdd));
	svrAdd.sin_family = AF_INET;
	svrAdd.sin_addr.s_addr = INADDR_ANY;
	svrAdd.sin_port = htons(Constants::port);
	return svrAdd;
}

socklen_t bindServer(int listenFd, struct sockaddr_in svrAdd, struct sockaddr_in clntAdd){
	int n;
	n=::bind(listenFd, (struct sockaddr *)&svrAdd, sizeof(svrAdd));
	PIRServer::errorExit(n<0,"Cannot bind");

	n=listen(listenFd,Constants::max_connects);
	PIRServer::errorExit(n<0,"Error listening");

	socklen_t len = sizeof(clntAdd);
	return len;
}

void run(int listenFd, struct sockaddr_in svrAdd, struct sockaddr_in clntAdd, socklen_t len){
	vector<boost::thread> pool;
    uint64_t threads=0;

    while (1){
        std::cout << "Listening" << "\n";

        //this is where client connects. svr will hang in this mode until client connects
        int connFd = accept(listenFd, (struct sockaddr *)&clntAdd, &len);
        PIRServer::errorExit(connFd<0,"Cannot accept connection");
        std::cout << "Connection successful" << "\n";

        if(Constants::parallel){
        	PIRServerParallel s(connFd,threads);
        	pool.push_back(boost::thread(boost::bind(&PIRServerParallel::job,&s)));
		}
		else{
		    PIRServerSequential s(connFd,threads);
		    pool.push_back(boost::thread(boost::bind(&PIRServerSequential::job,&s)));
		}
        threads++;
    }

    for(int i=0;i< pool.size();i++){
    	if(pool[i].joinable())  pool[i].join();
    }
};

int main(int argc, char* argv[]){
	PIRServer::errorExit((Constants::port > 65535) || (Constants::port < 2000),"Please choose a port number between 2000 - 65535");

	int listenFd = createSocket();
	struct sockaddr_in svrAdd,clntAdd;
	svrAdd = generateServerAddress(svrAdd);
	socklen_t len = bindServer(listenFd,svrAdd,clntAdd);

	run(listenFd,svrAdd,clntAdd,len);
    return 0;
}