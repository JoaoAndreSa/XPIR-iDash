/**
    XPIR-hash
    XPIRcPipeline.hpp
    Purpose: Child class that encloses the XPIR library function calls for the parallel execution

    @authors: Joao Sa
    @version 1.0 01/07/16
*/

/**

			    XPIRc
			      |
	   ----------- -----------
	   |                     |
  XPIRcPipeline (*)    XPIRcSequential 

*/

#pragma once

#include "../XPIRc.hpp"

#include "pir/replyGenerator/PIRReplyGeneratorFactory.hpp"
#include "crypto/HomomorphicCryptoFactory_internal.hpp"
#include "pir/replyExtraction/PIRReplyWriter.hpp"

#include <boost/signals2.hpp>

//signal handlers needed for PIRReplyWriter
typedef boost::signals2::signal<void (MessageEvent&)>   messageListener;
typedef boost::signals2::signal<void (WriteEvent&)>     writeListener;

class XPIRcPipeline: public XPIRc {

private:
	PIRQueryGenerator_internal* m_q_generator;
	GenericPIRReplyGenerator* m_r_generator;
	PIRReplyExtraction_internal* m_r_extractor;

	bool m_imported;								//whether the data has been imported before or not (if m_imported_db has been instanciated)
	imported_database_t m_imported_db;				//it seems the same object as in XPIRcSequential but it's not
	uint64_t m_maxFileSize;

	PIRReplyWriter* m_replyWriter;					//needed for the reply extraction
	messageListener m_messageListeners; 
    writeListener	m_writeListeners;

public:
	/**
    	Constructor for XPIRcSequential object.

    	@param params 	(check parent class)
    	@param type 	(check parent class)
    	@param db 		(check parent class)

    	@return
	*/
	XPIRcPipeline(PIRParameters params, int type, DBHandler* db) : XPIRc(params,type,db) {
		if(type==0){ //if SERVER
			m_q_generator=nullptr;
			m_r_extractor=nullptr;

			std::vector<std::string> fields;
    		boost::split(fields,m_params.crypto_params,boost::is_any_of(":"));

	    	m_r_generator = PIRReplyGeneratorFactory::getPIRReplyGenerator(fields.at(0),m_params,m_db);
    		m_r_generator->setCryptoMethod(m_crypto);
    		m_r_generator->setPirParams(m_params);

    		m_imported=0; //database has not been imported yet
		}else{		//if CLIENT
			m_r_generator=nullptr;

			m_q_generator=new PIRQueryGenerator_internal(m_params,*m_crypto);
			m_r_extractor=new PIRReplyExtraction_internal(m_params,*m_crypto);
			m_replyWriter=new PIRReplyWriter(m_params,m_writeListeners,m_messageListeners);
		}
	}

	PIRReplyWriter* getReplyWriter();				//m_replyWriter getter (needed to proceed with the reply extraction)
	PIRQueryGenerator_internal* getQGenerator();	/*m_q_generator getter (opposite to what happens in the sequential execution
													the PIR methods are called within threads)*/
	GenericPIRReplyGenerator* getRGenerator();		//m_r_generator getter
	PIRReplyExtraction_internal* getRExtractor();	//m_r_extractor getter
	imported_database_t getImportedDB();			//m_imported_db getter
	void setImportedDB(imported_database_t db);		//m_imported_db setter
	uint64_t getMaxFileSize();						//m_maxFileSize getter (only makes sense for client since he does not have access to db object);
	void setMaxFileSize(uint64_t);					//m_maxFileSize setter
	bool isImported();								//m_imported getter
	void setImported(bool);							//m_importedsetter
	void cleanup();									//clean 'tools'
};