#include "PIRClientParallel.hpp"

//***PRIVATE METHODS***//
//Download reply from the server and stores it chunks in shared replies queue.
void PIRClientParallel::downloadWorker(){
  unsigned int message_length=m_xpir->getCrypto()->getPublicParameters().getCiphBitsizeFromRecLvl(m_xpir->getD())/GlobalConstant::kBitsPerByte;

  double nbr = ceil(static_cast<double>(m_xpir->getMaxSize()*m_xpir->getAlpha())/double(m_xpir->getCrypto()->getPublicParameters().getAbsorptionBitsize(0)/GlobalConstant::kBitsPerByte)); 

    for (unsigned int i=1; i<m_xpir->getD(); i++){
    	nbr = ceil(nbr * double(m_xpir->getCrypto()->getPublicParameters().getCiphBitsizeFromRecLvl(i)/GlobalConstant::kBitsPerByte) / double(m_xpir->getCrypto()->getPublicParameters().getAbsorptionBitsize(i) / GlobalConstant::kBitsPerByte));
    } 

	char* buf;
	for (unsigned int i=0 ; i<nbr; i++){
		char* recvBuf = new char[message_length];

		// Get a reply element
      	readXBytes(message_length,(void*)recvBuf);
		m_xpir->getRExtractor()->repliesBuffer.push(recvBuf);
	}
}

//Sets reply extractor and launches parallely reply download and reply extraction.
void PIRClientParallel::startProcessResult(uint64_t pack_pos){
	m_xpir->getRExtractor()->startExtractReply(m_xpir->getMaxSize()*m_xpir->getAlpha(),m_xpir->getReplyWriter()->getClearDataQueue());
	downloadWorker();
}

//Upload query to the server and delete its parts from the shared query queue.
void PIRClientParallel::uploadWorker(){
	unsigned int length=0;
	char *tmp;

	for (unsigned int j=1; j<=m_xpir->getD(); j++){
		length=m_xpir->getCrypto()->getPublicParameters().getQuerySizeFromRecLvl(j) / GlobalConstant::kBitsPerByte;

		for (unsigned int i=0; i<m_xpir->getN()[j-1]; i++){
			tmp = m_xpir->getQGenerator()->queryBuffer.pop_front();
			sendXBytes(length,(void*)tmp);
			free(tmp);
        	sleepForBytes(length);
        }
    }
}

void PIRClientParallel::startProcessQuery(uint64_t pack_pos){
	m_xpir->getQGenerator()->setChosenElement(pack_pos);
  m_xpir->getQGenerator()->startGenerateQuery();
  while(1);
  uploadWorker();
}

void PIRClientParallel::joinAllThreads(){
  m_xpir->getRExtractor()->replyThread.join();
  //replyWriter.join();
}

//***PUBLIC METHODS***//
void PIRClientParallel::initXPIR(uint64_t num_entries){
	m_xpir= new XPIRcParallel(readParamsPIR(num_entries),1,nullptr);
}

std::string PIRClientParallel::searchQuery(uint64_t num_entries,std::map<char,std::string> entry){
    string query_str=entry['c']+" "+entry['p']+" # "+entry['r']+" "+entry['a'];
    uint64_t pos=m_SHA_256->hash(query_str);
    uint64_t pack_pos=considerPacking(m_SHA_256->hash(query_str),m_xpir->getAlpha());

    startProcessQuery(pack_pos);
    startProcessResult(pack_pos);
    joinAllThreads();

    return "";
}

void PIRClientParallel::cleanup(){
	delete m_SHA_256;
    delete m_xpir;
}