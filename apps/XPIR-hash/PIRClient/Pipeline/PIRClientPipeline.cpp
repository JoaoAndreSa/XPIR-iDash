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
void PIRClientPipeline::downloadWorker(int maxFileSize){
    double start_t,end_t;

    unsigned int message_length=m_xpir->getRsize(m_xpir->getD());

    double nbr = ceil(static_cast<double>(maxFileSize*m_xpir->getAlpha())/double(m_xpir->getAbsorptionSize(0))); 

    for (unsigned int i=1; i<m_xpir->getD(); i++){
    	 nbr = ceil(nbr * double(m_xpir->getRsize(i)) / double(m_xpir->getAbsorptionSize(i)));
    }

    start_t = omp_get_wtime();
  	for (unsigned int i=0 ; i<nbr; i++){
        if (i==0) cout << "PIRClient: Starting reply element reception"  << endl;

      	char* recvBuf = new char[message_length];

      	// Get a reply element
        m_socket.readXBytes(message_length,(void*)recvBuf);
      	m_xpir->getRExtractor()->repliesBuffer.push(recvBuf);
    }
    end_t = omp_get_wtime();
    if(Constants::bandwith_limit!=0) m_socket.sleepForBytes(nbr*message_length,end_t-start_t);

    cout << "PIRClient: Finish reply element reception" << endl;
}

/**
    Sets reply extractor and launches parallely reply download and reply extraction.

    @param
    @return
*/
void PIRClientPipeline::startProcessResult(int maxFileSize){
  m_xpir->getRExtractor()->startExtractReply(maxFileSize*m_xpir->getAlpha(),m_xpir->getReplyWriter()->getClearDataQueue());
  downloadWorker(maxFileSize);
}

/**
    Upload query to the server and delete its' parts from the shared query queue.

    @param
    @return
*/
void PIRClientPipeline::uploadWorker(){
	double start = omp_get_wtime(),start_t,end_t,total=0;

  unsigned int length=0;
	char *tmp;

  uint64_t total_bytes=0;
	for (unsigned int j=1; j<=m_xpir->getD(); j++){
		length=m_xpir->getQsize(j);

    start_t = omp_get_wtime();
		for (unsigned int i=0; i<m_xpir->getN()[j-1]; i++){
			tmp = m_xpir->getQGenerator()->queryBuffer.pop_front();
			m_socket.sendXBytes(length,(void*)tmp);
			free(tmp);
    }
    end_t = omp_get_wtime();
    total+=end_t-start_t;
    total_bytes+=m_xpir->getN()[j-1]*length;
  }
  if(Constants::bandwith_limit!=0) m_socket.sleepForBytes(total_bytes,total);

  double end = omp_get_wtime();
  cout << "PIRClient: Send query took " << end-start << " seconds" << endl;
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
bool PIRClientPipeline::searchQuery(uint64_t num_entries,std::map<char,std::string> entry){
    imported_database_t garbage;
    m_xpir= new XPIRcPipeline(Tools::readParamsPIR(num_entries),1,nullptr,garbage);

    //#-------SETUP PHASE--------#
    std::vector<std::pair<uint64_t,std::vector<std::string>>> pos = listQueryPos(entry);

    //TODO: Search multiple variants at the same time
    int max_bytesize = getInfoVCF(Tools::tokenize(entry['f'],",")[0]);
    uint64_t pack_pos=considerPacking(pos[0].first,m_xpir->getAlpha());

    //#-------QUERY PHASE--------#
    m_socket.sendInt(entry['f'].length()+1);
    m_socket.sendChar_s(const_cast<char*>(entry['f'].c_str()),entry['f'].length()+1);

    startProcessQuery(pack_pos);

    //#-------REPLY PHASE--------#
    startProcessResult(max_bytesize);
    joinAllThreads();

    char* response = m_xpir->getReplyWriter()->extractResponse(pos[0].first,max_bytesize,m_xpir->getAlpha(),m_xpir->getCrypto()->getPublicParameters().getAbsorptionBitsize()/GlobalConstant::kBitsPerByte);

    string response_s;
    if(!Constants::encrypted){   //if PLAINTEXT
        response_s = extractPlaintext(response,1,max_bytesize,pos[0].first,pos[0].second);
    }else{                       //if CIPHERTEXT
        response_s = extractCiphertext(response,1,max_bytesize,pos[0].first,pos[0].second);
    }

    bool check=true;
    for(int i=0;i<pos[0].second.size();i++){
        if(m_SHA_256->search(pos[0].second[i],response_s)==false){
            check=false;
        }
    }

    //#-------CLEANUP PHASE--------#
    delete[] response;
    delete m_SHA_256;
    delete m_xpir;

    return check;
}

