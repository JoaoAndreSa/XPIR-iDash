/**
    XPIR-hash
    PIRClientPipeline.cpp
    Purpose: Child class that executes client using pipelien PIR.
       NOTE: In pipeline PIR, client sends query to the server while he generates it.
             Furthermore, he does not need to wait and get all reply elements before starting the reply extraction.

    @author  Joao Sa, Marc-Olivier Killijian & Carlos Aguillar
    @version 1.0 07/09/16
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
    Download reply from the server and store its chunks in a shared replies queue.

    @param maxFileSize bytesize of an entire entry/line
    @param xpir XPIR object to perform PIR operations
    @return
*/
void PIRClientPipeline::downloadWorker(int maxFileSize,XPIRcPipeline* xpir){
    double start_t,end_t;

    unsigned int message_length=xpir->getRsize(xpir->getD());

    double nbr = ceil(static_cast<double>(maxFileSize*xpir->getAlpha())/double(xpir->getAbsorptionSize(0))); 

    for (unsigned int i=1; i<xpir->getD(); i++){
    	 nbr = ceil(nbr * double(xpir->getRsize(i)) / double(xpir->getAbsorptionSize(i)));
    }

    start_t = omp_get_wtime();
  	for (unsigned int i=0 ; i<nbr; i++){
        if (i==0) cout << "PIRClient: Starting reply element reception"  << endl;

      	char* recvBuf = new char[message_length];

      	// Get a reply element
        m_socket.readXBytes(message_length,(void*)recvBuf);
      	xpir->getRExtractor()->repliesBuffer.push(recvBuf);
    }
    end_t = omp_get_wtime();
    if(Constants::bandwith_limit!=0) m_socket.sleepForBytes(nbr*message_length,end_t-start_t);

    cout << "PIRClient: Finish reply element reception" << endl;
}

/**
    Starts reply extractor and launches parallely reply download and reply extraction.

    @param maxFileSize bytesize of an entire entry/line
    @param xpir XPIR object to perform PIR operations
    @return
*/
void PIRClientPipeline::startProcessResult(int maxFileSize,XPIRcPipeline* xpir){
  xpir->getRExtractor()->startExtractReply(maxFileSize*xpir->getAlpha(),xpir->getReplyWriter()->getClearDataQueue());
  downloadWorker(maxFileSize,xpir);
}

/**
    Upload query to the server and delete its parts from the shared queries queue.

    @param xpir XPIR object to perform PIR operations
    @return
*/
void PIRClientPipeline::uploadWorker(XPIRcPipeline* xpir){
	double start = omp_get_wtime(),total=0;
	char *tmp;

  uint64_t total_bytes=0;
	for (unsigned int j=1; j<=xpir->getD(); j++){
		unsigned int length=xpir->getQsize(j);

    double start_t = omp_get_wtime();
		for (unsigned int i=0; i<xpir->getN()[j-1]; i++){
			tmp = xpir->getQGenerator()->queryBuffer.pop_front();
			m_socket.sendXBytes(length,(void*)tmp);
			free(tmp);
    }
    double end_t = omp_get_wtime();
    total+=end_t-start_t;
    total_bytes+=xpir->getN()[j-1]*length;
  }
  if(Constants::bandwith_limit!=0) m_socket.sleepForBytes(total_bytes,total);

  double end = omp_get_wtime();
  cout << "PIRClient: Send query (" << total_bytes << " bytes) took " << end-start << " seconds" << endl;
  std::cout << "PIRClient: Query sent" << "\n";
}

/**
    Starts query generator and launches parallely query upload.

    @param pack_pos position to be queried
    @param xpir XPIR object to perform PIR operations
    @return
*/
void PIRClientPipeline::startProcessQuery(uint64_t pack_pos,XPIRcPipeline* xpir){
	xpir->getQGenerator()->setChosenElement(pack_pos);
  xpir->getQGenerator()->startGenerateQuery();
  uploadWorker(xpir);
}

/**
    Wait for helper threads to finish. These threads include those used by the reply extractor and reply writer
    (search for the variant in the extracted result)

    @param container contains all threads
    @return
*/
void PIRClientPipeline::joinAllThreads(vector<XPIRcPipeline*> container){
  for(int i=0;i<container.size();i++){
    container[i]->getRExtractor()->replyThread.join();
    container[i]->getReplyWriter()->join();
  }
}

//***PUBLIC METHODS***//
/**
    Main function of PIRClientParallel class. Queries server!

    @param entry a map/dictionary the stores que variant(s) beeing queried in a key-value way

    @return response_s stores the variant(s) we are looking for or "" otherwise
*/
bool PIRClientPipeline::searchQuery(std::map<char,std::string> entry){
    bool check=true;

    //#-------SETUP PHASE--------#
    m_socket.sendInt(entry['f'].length()+1);
    m_socket.sendChar_s(const_cast<char*>(entry['f'].c_str()),entry['f'].length()+1);

    std::vector<std::pair<uint64_t,std::vector<std::string>>> pos = listQueryPos(entry);
    m_socket.sendInt(pos.size());

    int max_bytesize = Constants::padding_size*Constants::data_hash_size/8;
    int data_hash_size=ceil(Constants::data_hash_size/8);

    std::vector<XPIRcPipeline*> container;
    std::vector<string> files = Tools::tokenize(entry['f'],",");
    for(int k=0;k<files.size();k++){
        m_AES_256->setIV(files[k]);
        for(int i=0;i<pos.size();i++){
            imported_database_t garbage;
            XPIRcPipeline* xpir= new XPIRcPipeline(Tools::readParamsPIR(Constants::num_entries),1,nullptr,garbage);
            container.push_back(xpir);

            uint64_t pack_pos=considerPacking(pos[i].first,xpir->getAlpha());

            //#-------QUERY PHASE--------#
            startProcessQuery(pack_pos,xpir);

            //#-------REPLY PHASE--------#
            startProcessResult(max_bytesize,xpir);
        }
    }
    joinAllThreads(container);

    for(int k=0,l=0;k<files.size();k++){
        m_AES_256->setIV(files[k]);
        for(int i=0;i<pos.size();i++,l++){
            //#-------EXTRACTION PHASE--------#
            char* response = container[l]->getReplyWriter()->extractResponse(pos[i].first,max_bytesize,container[l]->getAlpha(),container[l]->getCrypto()->getPublicParameters().getAbsorptionBitsize()/GlobalConstant::kBitsPerByte);
            if(!checkContent(response,1,max_bytesize,pos[i])) check=false;
            delete container[l];
        }
    }

    delete m_AES_256;
    delete m_SHA_256;

    return check;
}

