#pragma once

#include "../XPIRc.hpp"

#include "pir/replyGenerator/PIRReplyGeneratorFactory.hpp"
#include "crypto/HomomorphicCryptoFactory_internal.hpp"

class XPIRcParallel: public XPIRc {

private:
	PIRQueryGenerator_internal* m_q_generator;
	GenericPIRReplyGenerator* m_r_generator;
	PIRReplyExtraction_internal* m_r_extractor;

	CryptographicSystem* m_crypto;
	HomomorphicCrypto* m_crypto_c;

	bool m_imported;
	imported_database_t m_imported_db;

public:
	XPIRcParallel(PIRParameters params, int type, DBHandler* db) : XPIRc(params,type,db) {
		if(type==0){
			m_q_generator=nullptr;
			m_r_extractor=nullptr;

			std::vector<std::string> fields;
    		boost::split(fields,m_params.crypto_params,boost::is_any_of(":"));

			m_crypto=HomomorphicCryptoFactory_internal::getCrypto(fields.at(0));
    		m_crypto->setNewParameters(m_params.crypto_params);

	    	m_r_generator = PIRReplyGeneratorFactory::getPIRReplyGenerator(fields.at(0),m_params,m_db);
    		m_r_generator->setCryptoMethod(m_crypto);
    		m_r_generator->setPirParams(m_params);

    		m_imported=0;
		}else{
			m_r_generator=nullptr;

			m_crypto_c=HomomorphicCryptoFactory_internal::getCryptoMethod(m_params.crypto_params);

			m_q_generator=new PIRQueryGenerator_internal(m_params,*m_crypto_c);
			m_r_extractor=new PIRReplyExtraction_internal(m_params,*m_crypto_c);
		}
	}

	PIRQueryGenerator_internal* getQGenerator();
	GenericPIRReplyGenerator* getRGenerator();
	PIRReplyExtraction_internal* getRExtractor();
	CryptographicSystem* getCrypto();
	HomomorphicCrypto* getCryptoClient();
	imported_database_t getImportedDB();
	void setImportedDB(imported_database_t db);
	bool isImported();
};