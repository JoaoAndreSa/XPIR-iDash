/**
    XPIR-hash
    XPIRc.hpp
    Purpose: Parent class (abstract) that encloses the XPIR library function calls

    @author Joao Sa
    @version 1.0 18/01/17
*/

/**

			    XPIRc
			      |
	   ----------- -----------
	   |                     |
  XPIRcPipeline        XPIRcSequential

*/

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
#include "../Tools/Tools.hpp"

class XPIRc{
protected:
	bool m_type;					//if =0 server, if =1 client

	HomomorphicCrypto* m_crypto;
	DBHandler* m_db;				//if client m_db=nullptr
	PIRParameters m_params;

public:

	/**
    	Constructor (super class) for XPIRc object.

    	@param params PIR params (eg. aggregation/packing, recursion/dimensionality, encryption scheme).
    	@param type to distinguish between client and server.
    	@param db database handler (only makes sense on the server, null otherwise).

    	@return
	*/
	XPIRc(PIRParameters params, bool type, DBHandler* db) {
		m_params=params;
		m_type=type;
		m_db=db;

		/**
			Absorption capacity of an LWE encryption scheme depends on the number of sums that are going to be done in the 
			PIR protocol, it must therefore be initialized.
		 	Warning here we suppose the biggest dimension is in d[0] otherwise absorbtion needs to be computed accordingly
		*/
		m_crypto=HomomorphicCryptoFactory::getCryptoMethod(m_params.crypto_params);
		m_crypto->setandgetAbsBitPerCiphertext(params.n[0]);
	}

	uint64_t getD();   						//m_params.d getter (recursion/dimension value)
	uint64_t getAlpha();					//m_params.alpha getter (aggregation value)
	unsigned int* getN();					//m_params.n getter (recursion array)
	DBHandler* getDB();
	HomomorphicCrypto* getCrypto();
	uint32_t getQsize(uint64_t);			//get query element size in bytes
	uint32_t getRsize(uint64_t);			//get reply element size in bytes
	uint32_t getAbsorptionSize(uint64_t); 	//get absorption size in bytes

protected:
	void upperCleanup();	  				//clean 'tools'
};