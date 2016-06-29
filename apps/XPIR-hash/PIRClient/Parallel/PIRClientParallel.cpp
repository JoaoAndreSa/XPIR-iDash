#include "PIRClientParallel.hpp"

//***PRIVATE METHODS***//
//Download reply from the server and stores it chunks in shared replies queue.
void PIRClientParallel::downloadWorker(){
  unsigned int message_length=m_xpir->getCrypto()->getPublicParameters().getCiphBitsizeFromRecLvl(m_xpir->getD())/GlobalConstant::kBitsPerByte;

  double nbr = ceil(static_cast<double>(m_maxFileBytesize*m_xpir->getAlpha())/double(m_xpir->getCrypto()->getPublicParameters().getAbsorptionBitsize(0)/GlobalConstant::kBitsPerByte)); 

  for (unsigned int i=1; i<m_xpir->getD(); i++){
  	nbr = ceil(nbr * double(m_xpir->getCrypto()->getPublicParameters().getCiphBitsizeFromRecLvl(i)/GlobalConstant::kBitsPerByte) / double(m_xpir->getCrypto()->getPublicParameters().getAbsorptionBitsize(i) / GlobalConstant::kBitsPerByte));
  } 
	
  cout << nbr << endl;
	for (unsigned int i=0 ; i<nbr; i++){
    if (i==0) cout << "PIRClient: Starting reply element reception"  << endl;

  	char* recvBuf = new char[message_length];

  	// Get a reply element
    readXBytes(message_length,(void*)recvBuf);
  	m_xpir->getRExtractor()->repliesBuffer.push(recvBuf);
  }
  cout << "PIRClient: Finish reply element reception" << endl;
}

//Sets reply extractor and launches parallely reply download and reply extraction.
void PIRClientParallel::startProcessResult(){
  m_xpir->getRExtractor()->startExtractReply(m_maxFileBytesize*m_xpir->getAlpha(),m_xpir->getReplyWriter()->getClearDataQueue());
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
    }
  }
  std::cout << "PIRClient: Query sent" << "\n";
}

void PIRClientParallel::startProcessQuery(uint64_t pack_pos){
	m_xpir->getQGenerator()->setChosenElement(pack_pos);
  m_xpir->getQGenerator()->startGenerateQuery();
  uploadWorker();
}

void PIRClientParallel::joinAllThreads(){
  m_xpir->getRExtractor()->replyThread.join();
  m_xpir->getReplyWriter()->join();
}

//***PUBLIC METHODS***//
std::string PIRClientParallel::searchQuery(uint64_t num_entries,std::map<char,std::string> entry){
    m_xpir= new XPIRcParallel(readParamsPIR(num_entries),1,nullptr);

    m_maxFileBytesize = readuInt64();

    string query_str=entry['c']+" "+entry['p']+" # "+entry['r']+" "+entry['a'];
    uint64_t pos=m_SHA_256->hash(query_str);
    uint64_t pack_pos=considerPacking(m_SHA_256->hash(query_str),m_xpir->getAlpha());

    startProcessQuery(pack_pos);
    startProcessResult();
    joinAllThreads();

    cout << pos << endl;

    char* tmp = m_xpir->getReplyWriter()->extractResponse(pos,m_maxFileBytesize,m_xpir->getAlpha(),m_xpir->getCrypto()->getPublicParameters().getAbsorptionBitsize()/GlobalConstant::kBitsPerByte);

    for(int i=0;i<(m_xpir->getCrypto()->getPublicParameters().getAbsorptionBitsize()/GlobalConstant::kBitsPerByte) ; i++){
      printf("%c",tmp[i]);

    }
    return "";
}

void PIRClientParallel::cleanup(){
	delete m_SHA_256;
  delete m_xpir;
}