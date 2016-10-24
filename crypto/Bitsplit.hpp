#ifndef DEF_BITSPLIT
#define DEF_BITSPLIT

#define SHOUP

#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include <cstddef>
#include <gmp.h>
#include <iostream>
#include "NFLParams.hpp"
#include "NFLlib_old.hpp"
#include "prng/fastrandombytes_old.h"
#include <cstring>
#include <fstream>
#include <cassert>
// SSE/AVX(2) instructions
#include <immintrin.h>
#include <x86intrin.h>


typedef uint64_t* poly64;


class Bitsplit

{

  public:
    // Constructors and initializing functions
    Bitsplit();
    Bitsplit(uint64_t *moduli_,
    unsigned short nbModuli_,
    unsigned int polyDegree_);

static uint64_t mulmod(uint64_t x, uint64_t y, uint64_t p);
    // Data import and export main functions
    poly64* deserializeDataNFL(unsigned char **inArrayOfBuffers, uint64_t nbrOfBuffers,
        uint64_t dataBitsizePerBuffer, unsigned bitsPerCoordinate, uint64_t &polyNumber);
    static void serializeData64 (uint64_t* indata, unsigned char* outdata,
        unsigned int bitsPerChunk, uint64_t nb_of_uint64);
    static void serializeData32 (uint32_t* indata, unsigned char* outdata, unsigned int bitsPerChunk,
        uint64_t nb_of_uint32);


  private:

    // Polynomial attributes
    uint64_t *moduli;
    unsigned short nbModuli;
    unsigned int polyDegree;

public :

    uint64_t* bitsplitter (unsigned char** inDataBuffers, uint64_t nbrOfBuffers,
        uint64_t bitsPerBuffer, unsigned int bitsPerChunk);
    void internalLongIntegersToCRT(uint64_t* tmpdata, poly64 outdata, uint64_t int_uint64PerChunk,
        uint64_t totalNbChunks) ;
    void bs_finish(poly64 &outdata, uint64_t int_uint64PerChunk, uint64_t polyNumber,
        uint64_t *splitData, uint64_t nbrOfBuffers, uint64_t bitsPerBuffer, unsigned int bitsPerChunk);
    void bs_loop (unsigned char** inDataBuffers, uint64_t nbrOfBuffers,
        uint64_t bitsPerBuffer, unsigned int bitsPerChunk, uint64_t *&tmpdata,
        uint64_t bufferIndex, uint64_t &bitsread, size_t &subchunkIndex);
    uint64_t* bitsplitter_backtoback_internal_test (unsigned char** inDataBuffers,
        uint64_t nbrOfBuffers, uint64_t bitsPerBuffer, unsigned int bitsPerChunk,
        uint64_t totalbitsread,uint64_t bitsread, uint64_t* pointer64, unsigned int bitsToRead);

};

inline uint64_t Bitsplit::mulmod(uint64_t x, uint64_t y, uint64_t p)
{
  return (uint128_t) x * y % p;
}

#endif
