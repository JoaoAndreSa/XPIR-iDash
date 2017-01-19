
#include "Bitsplit.hpp"
#include <x86intrin.h>

Bitsplit::Bitsplit(){}
    Bitsplit::Bitsplit(uint64_t *moduli_,
    unsigned short nbModuli_,
    unsigned int polyDegree_):
    moduli(moduli_),
    nbModuli(nbModuli_),
    polyDegree(polyDegree_)
{
}
// *********************************************************
// Data import and export main functions
// *********************************************************

// Takes an array of buffers and:
// 1) Converts them into a set of polynomials with arbitrary large coefficients
// 2) Reduces the polys through CRT to have nbmoduli contiguous polys with uint64_t coefficients
// 3) Does the NTT transform
// - inArrayOfBuffers array of buffers to take the bits from
// - nbrOfBuffers nbr of buffers in the array
// - dataBitsizePerBuffer bits that can be taken from each buffer
// - bitsPerCoordinate bits used to create each coefficient (can be > 64 !)
// - polyNumber set by the function to say how many polynomials are in the returned pointer
poly64 *Bitsplit::deserializeDataNFL(unsigned char **inArrayOfBuffers, uint64_t nbrOfBuffers, uint64_t dataBitsizePerBuffer, unsigned bitsPerCoordinate, uint64_t &polyNumber) {

  // We need to handle dataBitsize bits of data per buffer
  // each poly can take publicParams.getAbsorptionBitsize() bits so
  polyNumber = ceil((double)dataBitsizePerBuffer*(double)nbrOfBuffers/(double)(bitsPerCoordinate*polyDegree));

  // The uint64_t arrays are allocated and filled with zeros
  // So that we do not have to pad with zeros beyond the limit
  poly64* deserData = (poly64 *) calloc(polyNumber, sizeof(poly64));

  // bitsplitter does all the hard work WITHOUT using large numbers !
  deserData[0] = bitsplitter(inArrayOfBuffers, nbrOfBuffers, dataBitsizePerBuffer, bitsPerCoordinate);

  // We finish the work by applying the NTT transform
#ifdef MULTI_THREAD
  #pragma omp parallel for
#endif
  for (unsigned int i = 0 ; i < polyNumber ; i++)
  {
    deserData[i] = deserData[0]+i*nbModuli*polyDegree;
#ifndef SIMULATE_PRE_NTT_DATA
   // nttAndPowPhi(deserData[i]);
#endif
  }

  return deserData;
}

// Serialize an array of poly64 elements into a compact byte buffer
// Takes a set of polynomial coefficients and outputs their concatenation
// - indata points to the polynomial coefficients
// - outdata points to the concatenation obtained
// - bitsPerChunk defines how many bits has each coefficient
// - nb_of_uint64 defines how many coefficients must be concatenated
// ASSUMPTION: all the polynomials are contiguously allocated
// ASSUMPTION: outdata has allocated one more uint64_t than needed
// ASSUMPTION:  all the coefficients have the same size which is below 56 bits
void Bitsplit::serializeData64 (uint64_t* indata, unsigned char* outdata, unsigned int bitsPerChunk, uint64_t nb_of_uint64)
{
  unsigned char *tmppointer;
  uint64_t *pointer64;
  pointer64 = (uint64_t *) outdata;
  uint32_t bitswritten=0;

  // Tricky approach playing with pointers to be able to infinitely add
  // up to 56 bits to any bit string present
  for (uint64_t i = 0 ; i < nb_of_uint64 ;)
  {
    while(bitswritten + bitsPerChunk <= 64)
    {
      *pointer64 |= (*indata++)<<bitswritten; i++;
      bitswritten += bitsPerChunk;
      if(i==nb_of_uint64) break;
    }
    tmppointer = (unsigned char*) pointer64;
    tmppointer+=bitswritten>>3;
    pointer64 = (uint64_t *) (tmppointer);
    bitswritten -=8*(bitswritten>>3);
  }
}


// Serialize an array of poly64 elements into a compact byte buffer
// Takes a set of polynomial coefficients and outputs their concatenation
// - indata points to the polynomial coefficients
// - outdata points to the concatenation obtained
// - bitsPerChunk defines how many bits has each coefficient
// - nb_of_uint64 defines how many coefficients must be concatenated
// ASSUMPTION: all the polynomials are contiguously allocated
// ASSUMPTION: outdata has allocated one more uint64_t than needed
// IMPORTANT NOTE: Unlike in serializeData64 bitsPerChunk can be arbitrarily large. This function considers that large coefficients are retrieved by blocks of varying size up to 32 bit. For example 100-bit coefficients will be retrieved by three blocks of 32 bits and a block of 4 looping that way for each 100-bit coefficient.
void Bitsplit::serializeData32 (uint32_t* indata, unsigned char* outdata, unsigned int bitsPerChunk, uint64_t nb_of_uint32){
  unsigned char *tmppointer;
  uint64_t *pointer64;
  pointer64 = (uint64_t *) outdata;
  uint32_t bitswritten=0;

  // See through how many block (=sub-chunk) we will have to loop to get each coefficient (=chunk)
  const double uint32PerChunk = (double)bitsPerChunk/32;
  const uint64_t int_uint32PerChunk = ceil(uint32PerChunk);
  const bool isint_uint32PerChunk = (uint32PerChunk==(double)int_uint32PerChunk);

  // Build masks for each sub-chunk
  uint64_t subchunkMasks[int_uint32PerChunk];
  unsigned int subchunkSizes[int_uint32PerChunk];

  // Increment with subchunkIndex=((subchunkIndex+1)%int_uint64PerChunk)
  // and use with subchunkSizes[subchunkIndex];
  unsigned int subchunkIndex = 0;
  for (int i = 0 ; i < int_uint32PerChunk - 1 ; i++)
  {
    subchunkSizes[i]=32;
    subchunkMasks[i] = (1ULL<<32)-1; // const mask for extracting 32 bits
  }
  subchunkSizes[int_uint32PerChunk-1] = bitsPerChunk - 32 * (int_uint32PerChunk - 1);
  subchunkMasks[int_uint32PerChunk-1] = (1ULL<<(subchunkSizes[int_uint32PerChunk-1]))-1;

  // Apply the same approach than in serializeData64 but with varying sizes
  for (uint64_t i = 0 ; i < nb_of_uint32 ;)
  {
    while(bitswritten + subchunkSizes[subchunkIndex] <= 64)
    {
      *pointer64 |= ((uint64_t)(*indata++))<<bitswritten; i++;
      bitswritten += subchunkSizes[subchunkIndex];
      subchunkIndex=((subchunkIndex+1)%int_uint32PerChunk);
      if(i==nb_of_uint32) break;
    }
    tmppointer = (unsigned char*) pointer64;
    tmppointer+=bitswritten>>3;
    pointer64 = (uint64_t *) (tmppointer);
    bitswritten -=8*(bitswritten>>3);
  }
}






// ****************************************************************************************
// THE DEN: Uncommented howling functions and pointer blood magic. Enter at your own risk.
// ****************************************************************************************

// We define first a back to back funtion to test our bitsplitter function
// If DEBUG_BITSPLIT_B2B the function is used, else it is ignored

//#define DEBUG_BITSPLIT_B2B
#ifdef DEBUG_BITSPLIT_B2B
#define DEBUG_BITSPLIT
#define B2BTEST(inDataBuffers, nbrOfBuffers, bitsPerBuffer, bitsPerChunk, totalbitsread, bitsread, pointer64, bitsToRead) bitsplitter_backtoback_internal_test (inDataBuffers, nbrOfBuffers, bitsPerBuffer, bitsPerChunk, totalbitsread, bitsread, pointer64, bitsToRead)
#else
#define B2BTEST(inDataBuffers, nbrOfBuffers, bitsPerBuffer, bitsPerChunk, totalbitsread, bitsread, pointer64, bitsToRead) if(0);
#endif

uint64_t* Bitsplit::bitsplitter_backtoback_internal_test (unsigned char** inDataBuffers, uint64_t nbrOfBuffers, uint64_t bitsPerBuffer, unsigned int bitsPerChunk, uint64_t totalbitsread,uint64_t bitsread, uint64_t* pointer64, unsigned int bitsToRead)
{
  unsigned int bufferIndex = totalbitsread/bitsPerBuffer;
  uint64_t bitPositionInBuffer = totalbitsread - bufferIndex*bitsPerBuffer;
  uint64_t bytePositionInBuffer = bitPositionInBuffer/8;
  unsigned int bitPositionInByte = bitPositionInBuffer%8;
  if (bitPositionInBuffer+sizeof(uint64_t)*8 > bitsPerBuffer)
  {
    std::cerr << "WARNING: Bitsplit goes beyond buffer size (let's hope it is allocated)" << std::endl;
  }
  if (bitPositionInBuffer + bitsToRead > bitsPerBuffer)
  {
    std::cerr << "CRITICAL: Bitsplit reads AND USES data beyond buffer space" << std::endl;
    std::cerr << "totalbitsread " << totalbitsread << std::endl;
    std::cerr << "bufferIndex " << bufferIndex << std::endl;
    std::cerr << "bitPositionInBuffer " << bitPositionInBuffer << std::endl;
    std::cerr << "bytePositionInBuffer " << bytePositionInBuffer << std::endl;
    std::cerr << "CRITICAL: On B2B test called for " << bitsToRead << " bits" << std::endl;
    exit(-1);
  }
  uint64_t b2bresult = ((*((uint64_t *)(inDataBuffers[bufferIndex]+bytePositionInBuffer)))>>bitPositionInByte) & ((1ULL<<bitsToRead)-1) ;
  uint64_t bitsplitresult = ((*pointer64)>>bitsread) & ((1ULL<<bitsToRead)-1);
  if (b2bresult  != bitsplitresult)
  {
    std::cerr << "CRITICAL: Bitsplit different from back to back function" << std::endl;
    std::cerr << "CRITICAL: Left " << b2bresult << std::endl;
    std::cerr << "CRITICAL: Right " << bitsplitresult << std::endl;
    exit(-1);
  }
  return 0;
}

inline void Bitsplit::bs_loop (unsigned char** inDataBuffers, uint64_t nbrOfBuffers, uint64_t bitsPerBuffer, unsigned int bitsPerChunk, uint64_t *&tmpdata, uint64_t bufferIndex, uint64_t &bitsread, size_t &subchunkIndex)
{
  // We redefine the amount of uint64 that can be produced given that we may have already read
  uint64_t bitstoread = bitsPerBuffer - bitsread;
  double nbChunks = (double)(bitsPerBuffer-bitsread)/bitsPerChunk;
  uint64_t int_nbChunks = floor(nbChunks);

  // How many uint64_t are needed to encode a chunk
  const double uint64PerChunk = (double)bitsPerChunk/56;
  const uint64_t int_uint64PerChunk = ceil(uint64PerChunk);
  const bool  isint_uint64PerChunk = (uint64PerChunk==(double)int_uint64PerChunk);

  // Compute subchunk sizes and masks
  uint64_t subchunkMasks[int_uint64PerChunk];
  // Increment with subchunkIndex=((subchunkIndex+1)%int_uint64PerChunk) and
  // use subchunkSizes[subchunkIndex];
  unsigned int subchunkSizes[int_uint64PerChunk];
  for (unsigned i = 0 ; i < int_uint64PerChunk - 1 ; i++)
  {
    subchunkSizes[i]=56;
    subchunkMasks[i] = (1ULL<<56)-1; // const mask for extracting 56 bits
  }
  subchunkSizes[int_uint64PerChunk-1] = bitsPerChunk - 56 * (int_uint64PerChunk - 1);
  subchunkMasks[int_uint64PerChunk-1] = (1ULL<<(subchunkSizes[int_uint64PerChunk-1]))-1;

#ifdef DEBUG_BITSPLIT

  for (int i = 0 ; i < int_uint64PerChunk ; i++)
  {
    std::cerr<<"bitsplit0 i="<<i<<std::endl;
    std::cerr<<"bitsplit0 subchunkSizes[i]="<<subchunkSizes[i]<<std::endl;
    std::cerr<<"bitsplit0 subchunkMasks[i]="<<subchunkMasks[i]<<std::endl;
  }
#endif

  // We compute how many extra subchunks are available taking into account that we may
  // start this new buffer on the middle of a chunk
  uint64_t supplementalSubchunks = 0;
  uint64_t cumulatedsize = 0;
  for (unsigned i = 0 ; i < int_uint64PerChunk ; i++)
  {
    if (cumulatedsize + subchunkSizes[(subchunkIndex + i) % int_uint64PerChunk] <= (bitsPerBuffer-bitsread)-int_nbChunks*bitsPerChunk)
    {
      supplementalSubchunks++;
      cumulatedsize += subchunkSizes[(subchunkIndex + i) % int_uint64PerChunk];
    }
    else
    {
      break;
    }
  }
  uint64_t totalSubChunks=int_nbChunks*int_uint64PerChunk + supplementalSubchunks;
#ifdef DEBUG_BITSPLIT
  std::cerr<<"bitsplit1 nbrOfBuffers="<<nbrOfBuffers<<std::endl;
  std::cerr<<"bitsplit1 nbChunks="<<nbChunks<<std::endl;
  std::cerr<<"bitsplit1 int_nbChunks="<<int_nbChunks<<std::endl;
  std::cerr<<"bitsplit1 uint64PerChunk="<<uint64PerChunk<<std::endl;
  std::cerr<<"bitsplit1 int_uint64PerChunk="<<int_uint64PerChunk<<std::endl;
  std::cerr<<"bitsplit1 isint_uint64PerChunk="<<isint_uint64PerChunk<<std::endl;
  std::cerr<<"bitsplit1 totalSubChunks="<<totalSubChunks<<std::endl;
  std::cerr<<"bitsplit1 cumulatedsize " << cumulatedsize << std::endl;
  std::cerr<<"bitsplit1 bitsPerBuffer " << bitsPerBuffer << std::endl;
  std::cerr<<"bitsplit1 bitsread " << bitsread << std::endl;
  std::cerr<<"bitsplit1 nextsubchunksize " <<  subchunkSizes[(subchunkIndex)]<< std::endl;
  std::cerr<<"bitsplit1 nextsubchunk " <<  subchunkIndex<< std::endl;
#endif

  unsigned  char *tmppointer;
  uint64_t *pointer64;
  pointer64 = (uint64_t *) inDataBuffers[bufferIndex];
  uint64_t bitsremaining=0;



  // Loop over the subchunks in the current buffer
  for (uint64_t i = 0 ; i < totalSubChunks ; )
  {
    // Get up to 64bits
    while (bitsread + subchunkSizes[subchunkIndex] <= 64)
    {
      *tmpdata = ((*pointer64)>>bitsread) & subchunkMasks[subchunkIndex];
      tmpdata++;i++;
      bitsread += subchunkSizes[subchunkIndex];
      subchunkIndex= (subchunkIndex+1 == int_uint64PerChunk ? 0 : subchunkIndex + 1);
      if(i==totalSubChunks) break;
    }
    if (bitstoread > 128)
    {
      unsigned shift = bitsread >>3;
      tmppointer = (unsigned char*) pointer64;
      tmppointer += shift;
      pointer64 = (uint64_t *) (tmppointer);
      bitstoread-= shift<<3;
      bitsread -= shift<<3;
    }
    else
    {
      tmppointer = (unsigned char*) pointer64;
      while ((64 - bitsread < subchunkSizes[subchunkIndex]) && bitstoread > 0)
      {
        tmppointer++;
        bitsread -= 8;
        bitstoread -= 8;
      }
      pointer64 = (uint64_t *) (tmppointer);
    }
  }

  // If there is a last partial subchunk in this buffer, read it part from this buffer and part
  // from next buffer if available
  bitsremaining = (uint64_t) round((nbChunks-int_nbChunks)*bitsPerChunk - cumulatedsize );
#ifdef DEBUG_BITSPLIT
    std::cout<<"bitsplit2 bitsremaining (should be <56)="<<bitsremaining<<std::endl;
#endif
  if (bitsremaining !=0)
  {
    size_t shift=(64-(bitsread+bitsremaining))/8;
    bitsread+=(shift<<3);
    tmppointer = (unsigned char*) pointer64;
    tmppointer-=shift;
    pointer64 = (uint64_t *) (tmppointer);
    *tmpdata = ((*pointer64)>>bitsread) & ((1ULL<<bitsremaining)-1);
    // If there is another buffer to deal with, finish the current tmpdata uint64_t
    if (bufferIndex < nbrOfBuffers - 1)
    {
      pointer64 = (uint64_t *) inDataBuffers[bufferIndex+1];
      *tmpdata |= ((*pointer64)<<bitsremaining) & subchunkMasks[subchunkIndex];
      // We restart bitsread to the bits read in the new buffer
      bitsread = subchunkSizes[subchunkIndex] - bitsremaining;
      subchunkIndex= (subchunkIndex+1 == int_uint64PerChunk ? 0 : subchunkIndex + 1);
      tmpdata++;
    }
  }
  else
  {
    bitsread = 0;
  }
}


inline void Bitsplit::bs_finish(poly64 &outdata, uint64_t int_uint64PerChunk, uint64_t polyNumber, uint64_t* splitData, uint64_t nbrOfBuffers, uint64_t bitsPerBuffer, unsigned int bitsPerChunk)
{
  if(int_uint64PerChunk>1) {
    outdata=(poly64) calloc(polyNumber*nbModuli*polyDegree + 1,sizeof(uint64_t));
    internalLongIntegersToCRT( splitData, outdata,   int_uint64PerChunk, ceil(((double)bitsPerBuffer*nbrOfBuffers)/bitsPerChunk));
    free(splitData);
  }
  else
  {
    if (nbModuli > 1)
    {
      outdata=(poly64) calloc(polyNumber*nbModuli*polyDegree + 1,sizeof(uint64_t));
      for (unsigned i = 0 ; i < polyNumber ; i++)
      {
        for (int cm = 0 ; cm < nbModuli ; cm++)
        {
          memcpy(outdata + i*polyDegree*nbModuli + cm*polyDegree,
              splitData + i*polyDegree, polyDegree*sizeof(uint64_t));
        }
      }
	  free(splitData);
    }
    else
    {
      outdata=splitData;
    }
  }
}


// This function does all the hard work of deserializeDataNFL
// 1) Converts input into a set of polynomials with arbitrary large coefficients
// 2) Reduces the polys through CRT to have nbModuli contiguous polys with uint64_t coefficients
// Do not try to understand it, it is a nightmare, we won't try to explain it :)
uint64_t* Bitsplit::bitsplitter (unsigned char** inDataBuffers, uint64_t nbrOfBuffers, uint64_t bitsPerBuffer, unsigned int bitsPerChunk)
{
  // If you don't need to change me don't try to understand me
  // If you need to change me, build me again from scratch :)

  // How many uint64_t are needed to encode a chunk
  const double uint64PerChunk = (double)bitsPerChunk/56;
  const uint64_t int_uint64PerChunk = ceil(uint64PerChunk);
  // How many polynomials are needed to encode the data
  uint64_t polyNumber =
    ceil((double)bitsPerBuffer*(double)nbrOfBuffers/(double)(bitsPerChunk*polyDegree));
  uint64_t* splitData =
    (uint64_t*)(calloc(polyNumber*polyDegree*int_uint64PerChunk+1,sizeof(uint64_t)));
  uint64_t* tmpdata=splitData;

  uint64_t bitsread=0;
  size_t subchunkIndex=0;

  // Loop over the buffers
  for (uint64_t h = 0 ; h < nbrOfBuffers ; h++)
  {
    bs_loop (inDataBuffers, nbrOfBuffers, bitsPerBuffer, bitsPerChunk,
        tmpdata, h, bitsread, subchunkIndex);
  }

  poly64 outdata;

  bs_finish(outdata, int_uint64PerChunk, polyNumber, splitData, nbrOfBuffers, bitsPerBuffer, bitsPerChunk);

  return outdata;
}



// Subroutine for bitsplitter, the daemonic function. This is the function that allows
// us to circumvect GMP
void Bitsplit::internalLongIntegersToCRT(uint64_t* tmpdata, poly64 outdata, uint64_t int_uint64PerChunk, uint64_t totalNbChunks)
{
  uint64_t* outdataPtr=outdata;
  uint64_t* indataPtr=tmpdata;
  uint64_t multiplier[nbModuli][int_uint64PerChunk];

  uint64_t* chunkParts[int_uint64PerChunk];

  for(int cm=0;cm<nbModuli;cm++)
  {
    multiplier[cm][0]=1;
    for(unsigned j=1;j<int_uint64PerChunk;j++)
    {
      multiplier[cm][j] = mulmod(multiplier[cm][j-1],1ULL<<56,moduli[cm]);
    }
  }

  for(unsigned i=0;i<totalNbChunks;i++)
  {
    for(unsigned j=0;j<int_uint64PerChunk;j++)
    {
      for(int cm=0;cm<nbModuli;cm++)
      {
      // set to zero before computation if not calloc'd
      *(outdataPtr+cm*polyDegree) += mulmod(*(indataPtr+j), multiplier[cm][j],moduli[cm]);
      }
    }
    indataPtr+=int_uint64PerChunk;
    outdataPtr++;
    if((i+1)%polyDegree==0)
    {
      outdataPtr += polyDegree*(nbModuli-1);
    }
  }
}

