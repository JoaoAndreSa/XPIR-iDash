#pragma once

#include "../XPIRc.hpp"

class XPIRcSequential: public XPIRc {

private:
	PIRQueryGenerator* m_q_generator;
	PIRReplyGenerator* m_r_generator;
	PIRReplyExtraction* m_r_extractor;

	HomomorphicCrypto* m_crypto;
	imported_database* m_imported_db;

public:
	typedef struct reply{
		vector<char*> reply;
		uint64_t nbRepliesGenerated;
		uint64_t aggregated_maxFileSize;
	} REPLY;

private:
	void import_database();

public:
	XPIRcSequential(PIRParameters params, int type, DBHandler* db) : XPIRc(params,type,db) {
		m_crypto=HomomorphicCryptoFactory::getCryptoMethod(m_params.crypto_params);
		m_crypto->setandgetAbsBitPerCiphertext(m_params.n[0]);

		// Absorption capacity of an LWE encryption scheme depends on the number of sums that are going
		// to be done in the PIR protocol, it must therefore be initialized
		// Warning here we suppose the biggest dimension is in d[0] 
		// otherwise absorbtion needs to be computed accordingly
		if(m_type==0){
			m_q_generator=nullptr;
			m_r_extractor=nullptr;

			m_r_generator = new PIRReplyGenerator(m_params,*m_crypto,m_db);
  			m_r_generator->setPirParams(m_params);

  			import_database();
		}else{
			m_r_generator=nullptr;
			m_imported_db=nullptr;

			m_q_generator = new PIRQueryGenerator(m_params,*m_crypto);
			m_r_extractor = new PIRReplyExtraction(m_params,*m_crypto);
		}

	}

	vector<char*> queryGeneration(uint64_t chosen_element);
	REPLY replyGeneration(vector<char*>);
	char* replyExtraction(REPLY);
	void cleanQueryBuffer();
	void freeQueries();
	uint32_t getQsize(uint64_t);
	uint32_t getRsize();
	void cleanup();
};