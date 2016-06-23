#include "XPIRcParallel.hpp"

//***PUBLIC METHODS***//
PIRQueryGenerator_internal* XPIRcParallel::getQGenerator(){
	return m_q_generator;
}

GenericPIRReplyGenerator* XPIRcParallel::getRGenerator(){
	return m_r_generator;
}

PIRReplyExtraction_internal* XPIRcParallel::getRExtractor(){
	return m_r_extractor;
}

CryptographicSystem* XPIRcParallel::getCrypto(){
	return m_crypto;
}

HomomorphicCrypto* XPIRcParallel::getCryptoClient(){
	return m_crypto_c;
}

imported_database_t XPIRcParallel::getImportedDB(){
	return m_imported_db;
}

void XPIRcParallel::setImportedDB(imported_database_t db){
	m_imported_db=db;
}

bool XPIRcParallel::isImported(){
	return m_imported;
}
