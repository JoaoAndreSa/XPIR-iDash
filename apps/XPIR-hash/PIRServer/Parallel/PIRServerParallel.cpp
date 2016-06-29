#include "PIRServerParallel.hpp"

//***PRIVATE METHODS***//
/**
 * Receive queries n messages with n = nbr of files.
 **/
void PIRServerParallel::downloadWorker(){
  double start = omp_get_wtime();

  //Allocate an array with d dimensions with pointers to arrays of n[i] lwe_query elements 
  m_xpir->getRGenerator()->initQueriesBuffer();

  uint64_t num_queries=0;
  for(unsigned int j=0; j<m_xpir->getD(); j++){
    //Compute and allocate the size in bytes of a query ELEMENT of dimension j 
    unsigned int message_length=m_xpir->getCrypto()->getPublicParameters().getQuerySizeFromRecLvl(j+1)/8;

    for (unsigned int i=0; i< m_xpir->getN()[j]; i++){
      if (i==0 && j == 0) cout << "PIRServer: Starting query element reception"  << endl;

      char* recvBuf = new char[message_length];

      // Get a query element
      readXBytes(message_length,(void*)recvBuf);
      m_xpir->getRGenerator()->pushQuery(recvBuf,message_length,j,i);
      num_queries++;
    }

  }

  // All the query elements received, unlock reply generation
  m_xpir->getRGenerator()->mutex.unlock();
  cout << "PIRServer: Finish query element reception" << endl;

  double end = omp_get_wtime();
  cout << "PIRServer: " << num_queries << " query elements received in " << end-start << endl;
}

/**
 * Send PIR's result, asynchronously. 
 **/
void PIRServerParallel::uploadWorker(){
  GenericPIRReplyGenerator* generator=m_xpir->getRGenerator();

  // Ciphertext byte size
  unsigned int length=m_xpir->getCrypto()->getPublicParameters().getCiphBitsizeFromRecLvl(m_xpir->getD())/GlobalConstant::kBitsPerByte;
  uint64_t bytes_sent=0;

  // Number of ciphertexts in the reply
  unsigned long reply_nbr=generator->computeReplySizeInChunks(m_xpir->getDB()->getmaxFileBytesize()),i=0;

  // Pointer for the ciphertexts to be sent
  char *ptr;

  // For each ciphertext in the reply
  for(unsigned i=0; i<reply_nbr; i++){
    while(generator->repliesArray == NULL || generator->repliesArray[i] == NULL){
      boost::this_thread::sleep(boost::posix_time::milliseconds(10));
    }
    ptr=generator->repliesArray[i];

    // Send it
    sendXBytes(length,(void*)ptr);
    bytes_sent+=length;

    // Free its memory
    free(ptr);
    generator->repliesArray[i]=NULL;
  }
  std::cout << "PIRServer: Reply sent" << "\n";
}

void PIRServerParallel::job (){
  std::cout << "THREAD [" << m_id << "]" << "\n";

  //#-------SETUP PHASE--------#
  //read file from client
	downloadData();

  DBDirectoryProcessor db;
  m_xpir = new XPIRcParallel(readParamsPIR(),0,&db);

  senduInt64(m_xpir->getDB()->getmaxFileBytesize());

  //#-------SETUP PHASE--------#
  // This is just a download thread. Reply generation is unlocked (by a mutex)
  // when this thread finishes.
 	m_downThread = boost::thread(&PIRServerParallel::downloadWorker, this);

  // Start reply generation when mutex unlocked
  // Start a thread which uploads the reply as it is generated
  m_upThread = boost::thread(&PIRServerParallel::uploadWorker, this);

  // Generate reply once unlocked by the query downloader thread
  // If we got a preimported database generate reply directly from it
  if(m_xpir->isImported()){
    m_xpir->getRGenerator()->generateReplyGenericFromData(m_xpir->getImportedDB());
  }else{
    m_xpir->setImportedDB(m_xpir->getRGenerator()->generateReplyGeneric(true));
    m_xpir->setImported(true);
  }
  
  // Wait for child threads
	if (m_upThread.joinable())  m_upThread.join();
	if (m_downThread.joinable()) m_downThread.join();

  // When everything is sent, close the socket
  close(m_connFd);
  std::cout << "THREAD [" << m_id << "] EXITED" << "\n";
}