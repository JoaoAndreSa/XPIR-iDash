/**
    XPIR-hash
    PIRClientPipeline.hpp
    Purpose: Child class that executes client using pipelien PIR.
       NOTE: In pipeline PIR, client sends query to the server while he is generating it server.
             Furthermore, he does not need to wait and get all reply elements before starting the reply extraction.

    @author Marc-Olivier Killijian, Carlos Aguillar & Joao Sa
    @version 1.0 01/07/16
*/

/**

                  PIRClient
                      |
           ----------- -----------
           |                     |
  PIRClientSequential    PIRClientPipeline(*)

*/

#include "PIRClientPipeline.hpp"

//***PRIVATE METHODS***//
/**
    Download reply from the server and stores it chunks in shared replies queue.

    @param
    @return
*/
void PIRClientPipeline::downloadWorker(){
    unsigned int message_length=m_xpir->getRsize(m_xpir->getD());

    double nbr = ceil(static_cast<double>(m_xpir->getMaxFileSize()*m_xpir->getAlpha())/double(m_xpir->getAbsorptionSize(0))); 

    for (unsigned int i=1; i<m_xpir->getD(); i++){
    	 nbr = ceil(nbr * double(m_xpir->getRsize(i)) / double(m_xpir->getAbsorptionSize(i)));
    }

  	for (unsigned int i=0 ; i<nbr; i++){
        if (i==0) cout << "PIRClient: Starting reply element reception"  << endl;

      	char* recvBuf = new char[message_length];

      	// Get a reply element
        m_socket.readXBytes(message_length,(void*)recvBuf);
      	m_xpir->getRExtractor()->repliesBuffer.push(recvBuf);
    }
    cout << "PIRClient: Finish reply element reception" << endl;
}

/**
    Sets reply extractor and launches parallely reply download and reply extraction.

    @param
    @return
*/
void PIRClientPipeline::startProcessResult(){
  m_xpir->getRExtractor()->startExtractReply(m_xpir->getMaxFileSize()*m_xpir->getAlpha(),m_xpir->getReplyWriter()->getClearDataQueue());
  downloadWorker();
}

/**
    Upload query to the server and delete its' parts from the shared query queue.

    @param
    @return
*/
void PIRClientPipeline::uploadWorker(){
	unsigned int length=0;
	char *tmp;

	for (unsigned int j=1; j<=m_xpir->getD(); j++){
		length=m_xpir->getQsize(j);
		for (unsigned int i=0; i<m_xpir->getN()[j-1]; i++){
			tmp = m_xpir->getQGenerator()->queryBuffer.pop_front();
			m_socket.sendXBytes(length,(void*)tmp);
			free(tmp);
    }
  }
  std::cout << "PIRClient: Query sent" << "\n";
}

/**
    Upload query to the server and delete its' parts from the shared query queue.

    @param
    @return
*/
void PIRClientPipeline::startProcessQuery(uint64_t pack_pos){
	m_xpir->getQGenerator()->setChosenElement(pack_pos);
  m_xpir->getQGenerator()->startGenerateQuery();
  uploadWorker();
}

/**
    Wait for helper threads to finish. These threads include the ones used by the reply extractor and reply writer
    (search for the variant in the extracted result)

    @param
    @return
*/
void PIRClientPipeline::joinAllThreads(){
  m_xpir->getRExtractor()->replyThread.join();
  m_xpir->getReplyWriter()->join();
}

//***PUBLIC METHODS***//
/**
    Main function of PIRClientParallel class. Queries server!

    @param num_entries total number of entries (size of database)
    @param entry a map/dictionary the stores que variant(s) beeing queried in a key-value way

    @return response_s stores the variant(s) we are looking for or "" otherwise
*/
std::string PIRClientPipeline::searchQuery(uint64_t num_entries,std::map<char,std::string> entry){
    m_xpir= new XPIRcPipeline(Tools::readParamsPIR(num_entries),1,nullptr);

    //#-------SETUP PHASE--------#
    m_xpir->setMaxFileSize(m_socket.readuInt64());

    string query_str=entry['c']+" "+entry['p']+" # "+entry['r']+" "+entry['a'];
    uint64_t pos=m_SHA_256->hash(query_str);
    uint64_t pack_pos=considerPacking(pos,m_xpir->getAlpha());

    //#-------QUERY PHASE--------#
    startProcessQuery(pack_pos);
    //#-------REPLY PHASE--------#
    startProcessResult();
    joinAllThreads();

    char* tmp = m_xpir->getReplyWriter()->extractResponse(pos,m_xpir->getMaxFileSize(),m_xpir->getAlpha(),m_xpir->getCrypto()->getPublicParameters().getAbsorptionBitsize()/GlobalConstant::kBitsPerByte);

    std::string response_s(reinterpret_cast<char*>(tmp));
    cout << "Reply: " << response_s << endl << endl;

    if(response_s!="") response_s = m_SHA_256->search(response_s,query_str);

    //#-------CLEANUP PHASE--------#
    delete m_SHA_256;
    delete m_xpir;
    
    return response_s;
}