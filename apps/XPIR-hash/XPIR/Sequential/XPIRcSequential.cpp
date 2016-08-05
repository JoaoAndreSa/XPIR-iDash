/**
    XPIR-hash
    XPIRcSequential.cpp
    Purpose: Child class that encloses the XPIR library function calls for the sequential execution

    @authors: Marc-Olivier Killijian, Carlos Aguillar & Joao Sa
    @version 1.0 01/07/16
*/

/**

			    XPIRc
			      |
	   ----------- -----------
	   |                     |
  XPIRcPipeline        XPIRcSequential (*)

*/

#include "XPIRcSequential.hpp"

//***PUBLIC METHODS***//
void import_all_databases(){

}

/**
	Imports the database/filesystem (stored on the db/ folder) to be used by the remaining PIR operations.

	@param
	@return
*/
imported_database* XPIRcSequential::import_database(string filename){
	DBDirectoryProcessor db(Constants::num_entries,filename);
	PIRParameters params = Tools::readParamsPIR(Constants::num_entries);
	HomomorphicCrypto* crypto=HomomorphicCryptoFactory::getCryptoMethod(params.crypto_params);
	crypto->setandgetAbsBitPerCiphertext(params.n[0]);

	/**
		Import database
		This can be done on the "Database setup" phase because:
			- the contents are static
			- the imported database fits in RAM
			- the server knows in advance the PIR and crypto parameters (e.g. read from file)
	*/
	cout << "PIRServer: Importing database ..." << endl;
	/**
		Warning aggregation is dealt with internally, the bytes_per_db_element parameter here is to be given WITHOUT multiplying it by params.alpha
	*/

	PIRReplyGenerator* r_generator = new PIRReplyGenerator(params,*crypto,&db);

	imported_database*  imported_db = r_generator->importData(/* uint64_t offset*/ 0, /*uint64_t bytes_per_db_element */ (&db)->getmaxFileBytesize());
	cout << "PIRServer: Database imported" << endl;

	delete r_generator;
	delete crypto;
	return imported_db;
}

/**
	Query generation phase (client-side). Stores encrypted query elements in an array to be sent to the server.

	@param chosen_element position of element queried

	@return query array that cointains the corresponding encrypted query (all its elements)
*/
vector<char*> XPIRcSequential::queryGeneration(uint64_t chosen_element){
	cout << "PIRClient: Generating query ..." << endl;
	/**
		Generate a query to get an element in the database (indexes begin at 0).
		Warning: if we set params.alpha=2, elements would be aggregated 2 by 2 and generateQuery
		would only accept as input 0 (the two first elements) or 1 (the other two)
	*/
	m_q_generator->generateQuery(chosen_element);

	/**
		The client pops the queries from m_q_generator with popQuery and 
		send them through the network and the server would receive and 
		push them into m_r_generator using pushQuery.
	*/
	vector<char*> query;
	char* query_element;
	while (m_q_generator->popQuery(&query_element)){
		query.push_back(query_element);
	}
	cout << "PIRClient: Query generated" << endl;

	return query;
}

/**
	Reply generation phase (server-side). Stores encrypted elements in array to be sent to the client.

	@param query array of encrypted query elements

	@return reply encrypted response message that contains data, number of reply elements and 
	              the size of the biggest aggegated file
*/
XPIRcSequential::REPLY XPIRcSequential::replyGeneration(vector<char*> query){

	for(int i=0;i<query.size();i++){
		m_r_generator->pushQuery(query[i]);
	}

	// Once the query is known and the database imported launch the reply generation
	cout << "PIRServer: Generating reply ..." << endl;
	double start = omp_get_wtime();
	m_r_generator->generateReply(m_imported_db);
	double end = omp_get_wtime();
	cout << "PIRServer: Reply generated in " << end-start << " seconds" << endl;

	/**
		The server would pop the replies from m_r_generator with popReply and
		sends them through the network together with nbRepliesGenerated and maxFileSize
		and the client would receive the replies and push them into m_r_extractor using pushEncryptedReply
	*/
	XPIRcSequential::REPLY reply;

	char* reply_element;
	while (m_r_generator->popReply(&reply_element)){
		reply.reply.push_back(reply_element);
	}

	reply.nbRepliesGenerated=m_r_generator->getnbRepliesGenerated();  
	cout << "PIRServer: "<< reply.nbRepliesGenerated << " Replies generated " << endl;	

	reply.maxFileSize=m_db->getmaxFileBytesize();
	return reply;
}

/**
	Reply extraction phase (client-side). Decrypts reply (homomorphic decryption) and returns result.

	@param reply encrypted response message sent by the server

	@return result decrypted reply
*/
char* XPIRcSequential::replyExtraction(XPIRcSequential::REPLY reply){
	for(int i=0;i<reply.reply.size();i++){
		m_r_extractor->pushEncryptedReply(reply.reply[i]);
	}

	cout << "PIRClient: Extracting reply ..." << endl;
	m_r_extractor->extractReply(reply.maxFileSize);
	cout << "PIRClient: Reply extracted" << endl;

	// In a real application instead of writing to a buffer we could write to an output file
	char *outptr, *result, *tmp;

	outptr = result = (char*)calloc(m_r_extractor->getnbPlaintextReplies(reply.maxFileSize)*m_r_extractor->getPlaintextReplyBytesize(), sizeof(char));
	while (m_r_extractor->popPlaintextResult(&tmp)) {
		memcpy(outptr, tmp, m_r_extractor->getPlaintextReplyBytesize());
		outptr+=m_r_extractor->getPlaintextReplyBytesize();
		free(tmp);
	}
	return result;
}

/**
	Frees the allocated memory taken by queries generated by m_q_generator

	@param
	@return
*/
void XPIRcSequential::cleanQueryBuffer(){
	m_q_generator->cleanQueryBuffer();
}

/**
	Frees the allocated memory taken by the queries pushed into m_r_generator

	@param
	@return
*/
void XPIRcSequential::cleanReplyBuffer(){
	m_r_generator->freeQueries();
}

/**
	Deletes allocated 'tools'..

	@param
	@return
*/
void XPIRcSequential::cleanup(){
	if(m_q_generator!=nullptr){delete m_q_generator;}
	if(m_r_generator!=nullptr){delete m_r_generator;}
	if(m_r_extractor!=nullptr){delete m_r_extractor;}
	//if(m_imported_db!=nullptr){delete m_imported_db;}
	upperCleanup();
}