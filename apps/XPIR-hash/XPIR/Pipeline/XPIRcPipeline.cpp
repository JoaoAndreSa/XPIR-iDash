/**
    XPIR-hash
    XPIRcPipeline.cpp
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

#include "XPIRcPipeline.hpp"

//***PUBLIC METHODS***//
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

bool XPIRcPipeline::isImported(){
	return m_imported;
}

void XPIRcPipeline::setImported(bool imported){
	m_imported=imported;
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
	upperCleanup();
}
