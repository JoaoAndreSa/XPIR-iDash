/**
    XPIR-hash
    Socket.hpp
    Purpose: Class to manage sockets (creation and information flow).

    @author Joao Sa
    @version 1.0 01/07/16
*/

#include "Socket.hpp"

//***PRIVATE METHODS***//
/**
    CLIENT/SERVER -> Initializes socket descriptor.

    @param
    @return
*/
void Socket::createSocket(){
	m_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	errorExit(m_socketFd<0,"Cannot open socket");
}

/**
    CLIENT -> Connects client to server and stores socket's file descriptor.

    @param
    @return
*/
void Socket::connectToServer(){
	m_connFd = connect(m_socketFd,(struct sockaddr *) &m_svrAdd, sizeof(m_svrAdd));
    errorExit(m_connFd<0,"Cannot connect!");
}

/**
    CLIENT -> Get server address (identified by name and port).

    @param
    @return
*/
void Socket::getServerAddress(){
	m_server = gethostbyname(m_sname);
	errorExit(m_server==NULL,"Host does not exist");

	bzero((char *)&m_svrAdd, sizeof(m_svrAdd));
    m_svrAdd.sin_family = AF_INET;

    bcopy((char *)m_server->h_addr, (char *)&m_svrAdd.sin_addr.s_addr, m_server->h_length);
    m_svrAdd.sin_port = htons(m_portNo);
}

/**
    SERVER -> Generate server address (for server socket creation).

    @param
    @return
*/
void Socket::generateServerAddress(){
	bzero((char*) &m_svrAdd, sizeof(m_svrAdd));
	m_svrAdd.sin_family = AF_INET;
	m_svrAdd.sin_addr.s_addr = INADDR_ANY;
	m_svrAdd.sin_port = htons(Constants::port);
}

/**
    SERVER -> Bind server to a specific server socket.

    @param
    @return
*/
void Socket::bindServer(){
	int n;
	n=::bind(m_socketFd, (struct sockaddr *)&m_svrAdd, sizeof(m_svrAdd));
	errorExit(n<0,"Cannot bind");

	n=listen(m_socketFd,Constants::max_connects);
	errorExit(n<0,"Error listening");

	m_len = sizeof(m_clntAdd);
}

/**
    SERVER -> Accepts connection betwen server and client and stores socket's file descriptor.

    @param
    @return
*/
void Socket::acceptConnection(){
	//This is where client connects. svr will hang in this mode until client connects
    m_connFd = accept(m_socketFd, (struct sockaddr *)&m_clntAdd, &m_len);
   	errorExit(m_connFd<0,"Cannot accept connection");
    std::cout << "Connection successful" << "\n";
}

/**
    Sleep a number o nanoseconds necessary to emulate a given bandwith value.

    @author Marc-Olivier Killijian & Carlos Aguillar

    @param
    @return
*/
void Socket::sleepForBytes(unsigned int bytes){
    uint64_t seconds=(bytes*8)/Constants::bandwith_limit;
    uint64_t nanoseconds=((((double)bytes*8.)/(double)Constants::bandwith_limit)-(double)seconds)*1000000000UL;

    struct timespec req={0},rem={0};
    req.tv_sec=seconds;
    req.tv_nsec=nanoseconds;

    nanosleep(&req,&rem);
}


//***PUBLIC METHODS***//
/**
    Read a X amount of bytes from the socket (we can then cast it to whatever type we need).

    @param x amount of bytes to read.
    @param buffer where we store the bytes.

    @return
*/
void Socket::readXBytes(uint64_t x, void* buffer){
    // This assumes buffer is at least x bytes long, and that the socket is blocking.
    int bytesRead = 0;
    while (bytesRead < x){
        unsigned int result = read(m_connFd, ((uint8_t*)buffer)+bytesRead, x - bytesRead); errorReadSocket(result<0);
        bytesRead += result;
    }
}

/**
    Reads an unsigned char array from the socket and returns it.

    @param buflen size of the element to be read.

    @return recvBuff buffer where we store the bytes.
*/
char* Socket::readChar_s(int buflen){
    char* recvBuff = new char[buflen];
    readXBytes(buflen,(void*)recvBuff);
    return recvBuff;
}

/**
    Reads an integer from the socket and returns it.

    @param

    @return v(int) value read.
*/
int Socket::readInt(){
    int v=0;
    readXBytes(sizeof(int), (void*)(&v));

    return static_cast<int>(ntohl(v));
}

/**
    Reads an unsigned integer from the socket and returns it.

    @param

    @return v(unsigned int) value read.
*/
unsigned int Socket::readuInt(){
    unsigned int v=0;
    readXBytes(sizeof(unsigned int), (void*)(&v));

    return static_cast<unsigned int>(ntohl(v));
}

/**
    Reads an unsigned integer (32bits) from the socket and returns it.

    @param

    @return v(uint32_t) value read.
*/
uint32_t Socket::readuInt32(){
    uint32_t v=0;
    readXBytes(sizeof(uint32_t), (void*)(&v));

    return static_cast<uint32_t>(ntohl(v));
}

/**
    Reads an unsigned integer (64bits) from the socket and returns it.

    @param

    @return v(uint64_t) value read.
*/
uint64_t Socket::readuInt64(){
    uint64_t v=0;
    readXBytes(sizeof(uint64_t), (void*)(&v));

    return static_cast<uint64_t>(ntohl(v));
}

/**
    Send X amount of bytes through the socket (we convert any type to a bunch of bytes).

    @param x amount of bytes to send.
    @param buffer source of bytes.

    @return
*/
void Socket::sendXBytes(uint64_t x, void* buffer){
    //This assumes buffer is at least x bytes long, and that the socket is blocking.
    int bytesWrite = 0;
    while (bytesWrite < x){
        int result = write(m_connFd, ((uint8_t*)buffer)+bytesWrite, x - bytesWrite); errorWriteSocket(result<0);
        bytesWrite += result;

        if(Constants::bandwith_limit!=0) sleepForBytes(result); //enforce bandwith
    }
}

/**
    Send an unsigned char* through the socket.

    @param c_str value to be sent.
    @param len length of string/array.

    @return
*/
void Socket::senduChar_s(unsigned char* c_str,int len){
    sendXBytes(len,(void*)c_str);
}

/**
    Send an integer through the socket.

    @param integer(uint64_t) value to be sent.

    @return
*/
void Socket::sendInt(int integer){
	int v = htonl(integer);
    sendXBytes(sizeof(int),(void*)(&v));
}

/**
    Send an unsigned integer through the socket.

    @param (unsigned)integer value to be sent.

    @return
*/
void Socket::senduInt(unsigned int integer){
    unsigned int v = htonl(integer);
    sendXBytes(sizeof(unsigned int),(void*)(&v));
}

/**
    Send an unsigned integer (32bits) through the socket.

    @param integer(uint32_t) value to be sent.

    @return
*/
void Socket::senduInt32(uint32_t integer){
    uint32_t v = htonl(integer);
    sendXBytes(sizeof(uint32_t),(void*)(&v));
}

/**
    Send an unsigned integer (64bits) through the socket.

    @param integer(uint64_t) value to be sent.

    @return
*/
void Socket::senduInt64(uint64_t integer){
    uint64_t v = htonl(integer);
    sendXBytes(sizeof(uint64_t),(void*)(&v));
}