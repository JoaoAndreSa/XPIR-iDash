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

	uint64_t m_maxFileSize;

	HomomorphicCrypto* m_crypto;
	DBHandler* m_db;
	PIRParameters m_params;

public:
	// if type==0 (server) else type==1 (client)
	XPIRc(PIRParameters params, int type, DBHandler* db) {
		m_params=params;
		m_type=type;
		m_db=db;

		if(type==0) m_maxFileSize = m_db->getmaxFileBytesize();

		m_crypto=HomomorphicCryptoFactory::getCryptoMethod(m_params.crypto_params);
		m_crypto->setandgetAbsBitPerCiphertext(m_params.n[0]);
	}

	uint64_t getD();
	uint64_t getAlpha();
	unsigned int* getN();
	DBHandler* getDB();
	HomomorphicCrypto* getCrypto();
	uint32_t getQsize(uint64_t);
	uint32_t getRsize();
};