#pragma once

#include "../XPIRc.hpp"

#include "pir/replyGenerator/PIRReplyGeneratorFactory.hpp"
#include "crypto/HomomorphicCryptoFactory_internal.hpp"

#include "pir/replyExtraction/PIRReplyWriter.hpp"
#include <boost/signals2.hpp>

typedef boost::signals2::signal<void (MessageEvent&)>   messageListener;
typedef boost::signals2::signal<void (WriteEvent&)>     writeListener;

class XPIRcParallel: public XPIRc {

private:
	PIRQueryGenerator_internal* m_q_generator;
	GenericPIRReplyGenerator* m_r_generator;
	PIRReplyExtraction_internal* m_r_extractor;

	bool m_imported;
	imported_database_t m_imported_db;

	PIRReplyWriter* m_replyWriter;
	messageListener m_messageListeners; 
    writeListener	m_writeListeners;

public:
	XPIRcParallel(PIRParameters params, int type, DBHandler* db) : XPIRc(params,type,db) {
		if(type==0){
			m_q_generator=nullptr;
			m_r_extractor=nullptr;

			std::vector<std::string> fields;
    		boost::split(fields,m_params.crypto_params,boost::is_any_of(":"));

	    	m_r_generator = PIRReplyGeneratorFactory::getPIRReplyGenerator(fields.at(0),m_params,m_db);
    		m_r_generator->setCryptoMethod(m_crypto);
    		m_r_generator->setPirParams(m_params);

    		m_imported=0;
		}else{
			m_r_generator=nullptr;

			m_q_generator=new PIRQueryGenerator_internal(m_params,*m_crypto);
			m_r_extractor=new PIRReplyExtraction_internal(m_params,*m_crypto);

			m_replyWriter=new PIRReplyWriter(m_params,m_writeListeners,m_messageListeners);
		}
	}

	PIRReplyWriter* getReplyWriter();
	PIRQueryGenerator_internal* getQGenerator();
	GenericPIRReplyGenerator* getRGenerator();
	PIRReplyExtraction_internal* getRExtractor();
	imported_database_t getImportedDB();
	void setImportedDB(imported_database_t db);
	bool isImported();
	void setImported(bool);
};