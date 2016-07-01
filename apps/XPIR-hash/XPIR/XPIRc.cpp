/**
    XPIR-hash
    XPIRc.cpp
    Purpose: Parent class (abstract) that encloses the XPIR library function calls

    @author Joao Sa
    @version 1.0 01/07/16
*/

/**

			    XPIRc (*)
			      |
	   ----------- -----------
	   |                     |
  XPIRcPipeline        XPIRcSequential

*/

#include "XPIRc.hpp"

//***PRIVATE METHODS***//
/**
    Deletes allocated 'tools'..

    @param
    @return
*/
void XPIRc::upperCleanup(){
    if(m_crypto!=nullptr){delete m_crypto;}
}


//***PUBLIC METHODS***//
uint64_t XPIRc::getAlpha(){
	return m_params.alpha;
}

uint64_t XPIRc::getD(){
	return m_params.d;
}

unsigned int* XPIRc::getN(){
	return m_params.n;
}

DBHandler* XPIRc::getDB(){
	return m_db;
}

HomomorphicCrypto* XPIRc::getCrypto(){
	return m_crypto;
}

/**
    GETTER for the query element size given a specific dimension (remember that each dimension can contain a different number of elements).

    @param d what dimension

    @return query_element_size (bits)
*/
uint32_t XPIRc::getQsize(uint64_t d){
	return (uint32_t) m_crypto->getPublicParameters().getQuerySizeFromRecLvl(d)/GlobalConstant::kBitsPerByte;
}

/**
    GETTER for the reply element size.

    @param

    @return reply_element_size (bits)
*/
uint32_t XPIRc::getRsize(){
	return (uint32_t) m_crypto->getPublicParameters().getCiphBitsizeFromRecLvl(m_params.d)/GlobalConstant::kBitsPerByte;
}