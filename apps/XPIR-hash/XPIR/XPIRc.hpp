#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "libpir.hpp"
#include "pir/dbhandlers/DBDirectoryProcessor.hpp"
#include "pir/GlobalConstant.hpp"

class XPIRc{
protected:
	int m_type;

	PIRParameters m_params;
	DBHandler* m_db;
	uint64_t m_maxFileSize;

	HomomorphicCrypto* m_crypto;

public:
	// if type==0 (server) else type==1 (client)
	XPIRc(PIRParameters params, int type, DBHandler* db) {
		m_params=params;
		m_type=type;
		m_db=db;

		if(type==0) m_maxFileSize = m_db->getmaxFileBytesize();
	}
	
	uint64_t getD();
	uint64_t getAlpha();
	unsigned int* getN();
	uint64_t getMaxSize();
	HomomorphicCrypto* getCrypto();
	uint32_t getQsize(uint64_t);
	uint32_t getRsize();
};