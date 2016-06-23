#pragma once

#include "../PIRClient.hpp"

class PIRClientParallel: public PIRClient {

public:
	PIRClientParallel(char* sname, int portNo) : PIRClient(sname,portNo){}
};