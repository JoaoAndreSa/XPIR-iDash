/**
    XPIR-hash
    XPIRcSequential.hpp
    Purpose: Child class that encloses the XPIR library function calls for the sequential execution

    @author Joao Sa
    @version 1.0 01/07/16
*/

/**

			    XPIRc
			      |
	   ----------- -----------
	   |                     |
  XPIRcPipeline        XPIRcSequential (*)

*/

#pragma once

#include "../XPIRc.hpp"

class XPIRcSequential: public XPIRc {

private:
	PIRQueryGenerator*  m_q_generator;
	PIRReplyGenerator*  m_r_generator;
	PIRReplyExtraction* m_r_extractor;

	imported_database*  m_imported_db;

public:
	//defines reply message structure which includes the following fields
	typedef struct reply{
		vector<char*> reply; 			//the actual data
		uint64_t nbRepliesGenerated;	//number of replies generated (size of previous array)
		uint64_t maxFileSize;    		//the size of the biggest aggegated file
	} REPLY;

	/**
    	Constructor for XPIRcSequential object.

    	@param params 	(check parent class)
    	@param type 	(check parent class)
    	@param db 		(check parent class)

    	@return
	*/
	XPIRcSequential(PIRParameters params, int type, DBHandler* db, imported_database*  imported_db=nullptr) : XPIRc(params,type,db) {

		if(m_type==0){ 	//if SERVER
			m_q_generator=nullptr;
			m_r_extractor=nullptr;

			m_r_generator = new PIRReplyGenerator(m_params,*m_crypto,m_db);

			m_imported_db = imported_db;
		}else{ 			//if CLIENT
			m_r_generator=nullptr;
			m_imported_db=nullptr;

			m_q_generator = new PIRQueryGenerator(m_params,*m_crypto);
			m_r_extractor = new PIRReplyExtraction(m_params,*m_crypto);
		}

	}
	static imported_database* import_database(string);//import database and initialize imported_database object
	vector<char*> queryGeneration(uint64_t chosen_element);  //generate query (client)
	REPLY replyGeneration(vector<char*>);					 //generate reply (server)
	char* replyExtraction(REPLY);							 //extract reply  (client)
	void cleanQueryBuffer();								 //clean queries  (client)
	void cleanReplyBuffer();							     //clean queries in reply object (server)
	void cleanup();											 //clean 'tools'
};