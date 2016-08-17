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
    Sets reply extractor and launches parallely reply download and reply extraction.

    @param
    @return
*/
void PIRClientPipeline::startProcessResult(int maxFileSize,XPIRcPipeline* xpir){
  xpir->getRExtractor()->startExtractReply(maxFileSize*xpir->getAlpha(),xpir->getReplyWriter()->getClearDataQueue());
  downloadWorker(maxFileSize,xpir);
}

/**
    Upload query to the server and delete its' parts from the shared query queue.

    @param
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
    Upload query to the server and delete its' parts from the shared query queue.

    @param
    @return
*/
void PIRClientPipeline::startProcessQuery(uint64_t pack_pos,XPIRcPipeline* xpir){
	xpir->getQGenerator()->setChosenElement(pack_pos);
  xpir->getQGenerator()->startGenerateQuery();
  uploadWorker(xpir);
}

/**
    Wait for helper threads to finish. These threads include the ones used by the reply extractor and reply writer
    (search for the variant in the extracted result)

    @param
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

    @param num_entries total number of entries (size of database)
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

    std::vector<XPIRcPipeline*> container;
    for(int k=0;k<Tools::tokenize(entry['f'],",").size();k++){
        for(int i=0;i<pos.size();i++){
            imported_database_t garbage;
            XPIRcPipeline* xpir= new XPIRcPipeline(Tools::readParamsPIR(Constants::num_entries),1,nullptr,garbage);
            container.push_back(xpir);

            uint64_t pack_pos=considerPacking(pos[i].first,xpir->getAlpha());

            cout << endl;
            //#-------QUERY PHASE--------#
            startProcessQuery(pack_pos,xpir);

            //#-------REPLY PHASE--------#
            startProcessResult(max_bytesize,xpir);
        }
    }
    joinAllThreads(container);

    int k=0;
    for(int i=0;i<container.size();i++,k++){
        k=k%pos.size();
        char* response = container[i]->getReplyWriter()->extractResponse(pos[k].first,max_bytesize,container[i]->getAlpha(),container[i]->getCrypto()->getPublicParameters().getAbsorptionBitsize()/GlobalConstant::kBitsPerByte);
        string response_s;
        if(!Constants::encrypted){   //if PLAINTEXT
            response_s = extractPlaintext(response,1,max_bytesize,pos[k].first);
        }else{                       //if CIPHERTEXT
            response_s = extractCiphertext(response,1,max_bytesize,pos[k].first);
        }

        for(int j=0;j<pos[k].second.size();j++){
            if(m_SHA_256->search(pos[k].second[j],response_s)==false){
                check=false;
            }
        }

        //#-------CLEANUP PHASE--------#
        delete[] response;
        delete container[i];
    }

    delete m_AES_256;
    delete m_SHA_256;

    return check;
}

