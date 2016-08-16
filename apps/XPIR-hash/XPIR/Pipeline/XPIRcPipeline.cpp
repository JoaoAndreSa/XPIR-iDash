/**
    XPIR-hash
    XPIRcPipeline.cpp
    Purpose: Child class that encloses the XPIR library function calls for the parallel execution

    @authors: Marc-Olivier Killijian, Carlos Aguillar & Joao Sa
    @version 1.0 01/07/16
*/

/**

			    XPIRc
			      |
	   ----------- -----------
	   |                     |
  XPIRcPipeline (*)    XPIRcSequential 

*/

#include "XPIRcPipeline.hpp"

//***PUBLIC METHODS***//
imported_database_t XPIRcPipeline::import_database(string filename){
	DBDirectoryProcessor db(Constants::num_entries,filename);
	PIRParameters params = Tools::readParamsPIR(Constants::num_entries);
	HomomorphicCrypto* crypto=HomomorphicCryptoFactory::getCryptoMethod(params.crypto_params);
	crypto->setandgetAbsBitPerCiphertext(params.n[0]);

	std::vector<std::string> fields;
    boost::split(fields,params.crypto_params,boost::is_any_of(":"));

	GenericPIRReplyGenerator* r_generator = PIRReplyGeneratorFactory::getPIRReplyGenerator(fields.at(0),params,&db);
    r_generator->setCryptoMethod(crypto);
    r_generator->setPirParams(params);

    imported_database_t imported_db = r_generator->generateReplyGeneric(true,true);

    delete r_generator;
    delete crypto;

    return imported_db;
}


PIRReplyWriter* XPIRcPipeline::getReplyWriter(){
	return m_replyWriter;
}

PIRQueryGenerator_internal* XPIRcPipeline::getQGenerator(){
	return m_q_generator;
}

GenericPIRReplyGenerator* XPIRcPipeline::getRGenerator(){
	return m_r_generator;
}

PIRReplyExtraction_internal* XPIRcPipeline::getRExtractor(){
	return m_r_extractor;
}

imported_database_t XPIRcPipeline::getImportedDB(){
	return m_imported_db;
}

void XPIRcPipeline::setImportedDB(imported_database_t db){
	m_imported_db=db;
}

/**
	Deletes allocated 'tools'..

	@param
	@return
*/
void XPIRcPipeline::cleanup(){
	if(m_q_generator!=nullptr){delete m_q_generator;}
	if(m_r_generator!=nullptr){delete m_r_generator;}
	if(m_r_extractor!=nullptr){delete m_r_extractor;}
	if(m_replyWriter!=nullptr){delete m_replyWriter;}
	upperCleanup();
}
