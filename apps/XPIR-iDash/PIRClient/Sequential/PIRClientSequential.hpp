#pragma once

#include "../PIRClient.hpp"

class PIRClientSequential: public PIRClient {

public:
	PIRClientSequential(char* sname, int portNo) : PIRClient(sname,portNo){}
};