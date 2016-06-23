#include "PIRServerParallel.hpp"

//***PRIVATE METHODS***//
/**
 * Receive queries n messages with n = nbr of files.
 **/
void PIRServerParallel::downloadWorker(){
  //Allocate an array with d dimensions with pointers to arrays of n[i] lwe_query elements 
  m_xpir->getRGenerator()->initQueriesBuffer();

  for(unsigned int j=0; j<m_xpir->getD(); j++){
    //Compute and allocate the size in bytes of a query ELEMENT of dimension j 
    unsigned int message_length=m_xpir->getCryptoServer()->getPublicParameters().getQuerySizeFromRecLvl(j+1)/8;

    for (unsigned int i=0; i< m_xpir->getN()[j]; i++){
      if (i==0 && j == 0) cout << "PIRSession: Starting query element reception"  << endl;
      char* recvBuf = new char[message_length];

      // Get a query element
      readXBytes(message_length,(void*)recvBuf);
      m_xpir->getRGenerator()->pushQuery(recvBuf,message_length,j,i);
    }

  }

  // All the query elements received, unlock reply generation
  m_xpir->getRGenerator()->mutex.unlock();
}

/**
 * Send PIR's result, asynchronously. 
 **/
void PIRServerParallel::uploadWorker(){
  GenericPIRReplyGenerator* generator=m_xpir->getRGenerator();


  // Ciphertext byte size
  unsigned int length=m_xpir->getCryptoServer()->getPublicParameters().getCiphBitsizeFromRecLvl(m_xpir->getD())/GlobalConstant::kBitsPerByte;
  uint64_t bytes_sent=0;

  // Number of ciphertexts in the reply
  unsigned long reply_nbr=generator->computeReplySizeInChunks(m_xpir->getMaxSize()),i=0;

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

    sleepForBytes(length);
    
    // Free its memory
    free(ptr);
    generator->repliesArray[i]=NULL;
  }
  return;
}

void PIRServerParallel::initXPIR(){
    DBDirectoryProcessor db;
    m_xpir = new XPIRcParallel(readParamsPIR(),0,&db);
}

void PIRServerParallel::job (){
  std::cout << "THREAD [" << m_id << "]" << "\n";

  //#-------SETUP PHASE--------#
  //read file from client
	downloadData();
  initXPIR();

  //#-------SETUP PHASE--------#
  // This is just a download thread. Reply generation is unlocked (by a mutex)
  // when this thread finishes.
 	m_downThread = boost::thread(&PIRServerParallel::downloadWorker, this);
  while(1);

  // Start reply generation when mutex unlocked
  // Start a thread which uploads the reply as it is generated
  m_upThread = boost::thread(&PIRServerParallel::uploadWorker, this);

  // Generate reply once unlocked by the query downloader thread
  // If we got a preimported database generate reply directly from it
  if(m_xpir->isImported()){
    m_xpir->setImportedDB(m_xpir->getRGenerator()->generateReplyGeneric(true));
  }else{
    m_xpir->getRGenerator()->generateReplyGenericFromData(m_xpir->getImportedDB());
  }
  
  // Wait for child threads
	if (m_upThread.joinable())  m_upThread.join();
	if (m_downThread.joinable()) m_downThread.join();

  // When everything is sent, close the socket
  close(m_connFd);
  std::cout << "THREAD [" << m_id << "] EXITED" << "\n";
}