/**
    XPIR-hash
    Socket.hpp
    Purpose: Class to manage sockets (creation and information flow).

    @author Joao Sa
    @version 1.0 18/01/17
*/

#pragma once

#include <iostream>
#include <stdint.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>
#include <time.h>
#include "omp.h"

#include "../Constants/constants.hpp"
#include "../Error/Error.hpp"

class Socket{
private:
	bool m_type;							//if =0 server, if =1 client

	//Connection variables
	int m_socketFd, m_portNo;
	char* m_sname;
	struct sockaddr_in m_svrAdd,m_clntAdd;
	socklen_t m_len;


public:
	int m_connFd;
	/**
    	Constructor for Socket object.

    	@param type to distinguish between client and server.

    	@return
	*/
	Socket(bool type){
		m_type=type;
		createSocket();

		if(m_type==0){
			generateServerAddress();
			bindServer();
		}else{
			getServerAddress();
			connectToServer();
		}
	}

	Socket(){}

	//SERVER
	void acceptConnection();				//accepts connection betwen server and client and stores socket's file descriptor

	//BANDWIDTH
	void sleepForBytes(uint64_t,double);	//sleep a number o nanoseconds necessary to emulate a given bandwith value

	//READING
	void readXBytes(uint64_t, void*);   	//read X amount of bytes from the socket (we can then cast it to whatever type we need)
	unsigned char* readuChar(int);
	char* readChar(int);					//reads an char array (binary) from the socket and returns it
	int readInt();							//reads an integer from the socket and returns it
	unsigned int readuInt();				//reads an unsigned integer from the socket and returns it
	uint32_t readuInt32();					//reads an unsigned integer (32bits) from the socket and returns it
	uint64_t readuInt64();					//reads an unsigned integer (64bits) from the socket and returns it

	//WRITING
	void sendXBytes(uint64_t, void*);		//send X amount of bytes through the socket (we convert any type to a bunch of bytes)
	void senduChar_s(unsigned char*,int);	//sends an unsigned char array through the socket
	void sendChar_s(char*,int);				//sends an unsigned char array through the socket
	void sendInt(int);						//sends an integer through the socket
	void senduInt(unsigned int);			//sends an unsigned integer through the socket
	void senduInt32(uint32_t);				//sends an unsigned integer (32 bits) through the socket
	void senduInt64(uint64_t);				//sends an unsigned integer (64 bits) through the socket

	//ERROR HANDLING
	void errorWriteSocket(int cond){if(cond==1){std::cerr << "ERROR writing to socket" << "\n";}}	//For errors while writing in socket
	void errorReadSocket(int cond){if(cond==1){std::cerr << "ERROR reading socket"<< "\n";}}			//For errors while reading from socket

	//CLOSE
	void closeSocket();						//closes socket

private:
	//CREATION
	void createSocket();					//initializes socket and stores its descriptor

	//CLIENT
	void connectToServer();					//connects client to server and stores socket's file descriptor
	void getServerAddress();				//get server address (identified by name and port)

	//SERVER
	void bindServer();						//bind server to a specific server socket
	void generateServerAddress();			//generate server address

};