#include "XPIRc.hpp"

//***PRIVATE METHODS***//
void XPIRc::import_database(){
	// Import database
	// This could have been done on the "Database setup" phase if:
	//  - the contents are static
	//  - AND the imported database fits in RAM
	//  - AND the server knows in advance the PIR and crypto parameters (e.g. chosen by him)
	cout << "SimplePIR: Importing database ..." << endl;
	// Warning aggregation is dealt with internally the bytes_per_db_element parameter here
	// is to be given WITHOUT multiplying it by params.alpha
	m_imported_db = m_r_generator->importData(/* uint64_t offset*/ 0, /*uint64_tbytes_per_db_element */ m_db->getmaxFileBytesize());
	cout << "SimplePIR: Database imported" << endl;
	
	m_maxFileSize = m_db->getmaxFileBytesize();
}

//***PUBLIC METHODS***//
vector<char*> XPIRc::queryGeneration(uint64_t chosen_element){
	/*******************************************************************************
	* Query generation phase (client-side)                                       
	******************************************************************************/

	// Create the query generator object
	cout << "SimplePIR: Generating query ..." << endl;
	// Generate a query to get the FOURTH element in the database (indexes begin at 0)
	// Warning : if we had set params.alpha=2 elements would be aggregated 2 by 2 and 
	// generatequery would only accept as input 0 (the two first elements) or 1 (the other two)
	m_q_generator->generateQuery(chosen_element);

	// In a real application the client would pop the queries from q with popQuery and 
	// send them through the network and the server would receive and push them into s 
	// using pushQuery
	vector<char*> query;
	char* query_element;
	while (m_q_generator->popQuery(&query_element)){
		query.push_back(query_element);
	}
	cout << "SimplePIR: Query generated" << endl;

	return query;
}

XPIRc::REPLY XPIRc::replyGeneration(vector<char*> query){
	/******************************************************************************
	* Reply generation phase (server-side)
	******************************************************************************/

	// In a real application the client would pop the queries from q with popQuery and 
	// send them through the network and the server would receive and push them into s 
	// using pushQuery
	for(int i=0;i<query.size();i++){
		m_r_generator->pushQuery(query[i]);
	}
			
	// Once the query is known and the database imported launch the reply generation
	cout << "SimplePIR: Generating reply ..." << endl;
	double start = omp_get_wtime();
	m_r_generator->generateReply(m_imported_db);
	double end = omp_get_wtime();
	cout << "SimplePIR: Reply generated in " << end-start << " seconds" << endl;

	// In a real application the server would pop the replies from s with popReply and 
	// send them through the network together with nbRepliesGenerated and aggregated_maxFileSize 
	// and the client would receive the replies and push them into r using pushEncryptedReply
	XPIRc::REPLY reply;

	char* reply_element;
	while (m_r_generator->popReply(&reply_element)){
		reply.reply.push_back(reply_element);
	}

	reply.nbRepliesGenerated=m_r_generator->getnbRepliesGenerated();  
	cout << "SimplePIR: "<< reply.nbRepliesGenerated << " Replies generated " << endl;	

	reply.aggregated_maxFileSize=m_maxFileSize;
	return reply;
}

char* XPIRc::replyExtraction(XPIRc::REPLY reply){
	delete m_r_generator;
	m_r_generator = new PIRReplyGenerator(m_params,*m_crypto,m_db);
  	m_r_generator->setPirParams(m_params);
	/******************************************************************************
	* Reply extraction phase (client-side)
	******************************************************************************/
	
	for(int i=0;i<reply.reply.size();i++){
		m_r_extractor->pushEncryptedReply(reply.reply[i]);
	}

	cout << "SimplePIR: Extracting reply ..." << endl;
	m_r_extractor->extractReply(reply.aggregated_maxFileSize);
	cout << "SimplePIR: Reply extracted" << endl;

	// In a real application instead of writing to a buffer we could write to an output file
	char *outptr, *result, *tmp;

	outptr = result = (char*)calloc(m_r_extractor->getnbPlaintextReplies(reply.aggregated_maxFileSize)*m_r_extractor->getPlaintextReplyBytesize(), sizeof(char));
	while (m_r_extractor->popPlaintextResult(&tmp)) {
		memcpy(outptr, tmp, m_r_extractor->getPlaintextReplyBytesize());
		outptr+=m_r_extractor->getPlaintextReplyBytesize();
		free(tmp);
	}
	// Result is in ... result
	return result;
}

uint32_t XPIRc::getQsize(uint64_t d){
	return (uint32_t) m_crypto->getPublicParameters().getQuerySizeFromRecLvl(d)/GlobalConstant::kBitsPerByte;
}

uint32_t XPIRc::getRsize(){
	return (uint32_t) m_crypto->getPublicParameters().getCiphBitsizeFromRecLvl(m_params.d)/GlobalConstant::kBitsPerByte;
}

uint64_t XPIRc::getAlpha(){
	return m_params.alpha;
}

uint64_t XPIRc::getD(){
	return m_params.d;
}

unsigned int* XPIRc::getN(){
	return m_params.n;
}

void XPIRc::cleanQueryBuffer(){
	m_q_generator->cleanQueryBuffer();
}

void XPIRc::freeQueries(){
	char* reply_element_tmp;
	m_r_generator->freeQueries();
}

void XPIRc::cleanup(){
	if(m_q_generator!=nullptr) delete m_q_generator;
	if(m_r_generator!=nullptr) delete m_r_generator;
	if(m_r_extractor!=nullptr) delete m_r_extractor;
	if(m_imported_db!=nullptr) delete m_imported_db;
}