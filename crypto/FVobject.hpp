#ifndef DEF_FVOBJECT
#define DEF_FVOBJECT

#include "NFLFV.hpp"
#include "NFLlib_old.hpp"
#include "NFLLWEDatatypes.hpp"
#include "Bitsplit.hpp"

#include <string>
#include <set>
#include <boost/algorithm/string.hpp>
#include "../pir/PIRParameters.hpp"
#include "AbstractPublicParameters.hpp"

class FVobject
{

public :
virtual void enc(lwe_cipher* c, poly64 m)=0;
virtual void encNTT(lwe_cipher* c, poly64 m)=0;
virtual void dec(poly64 m, lwe_cipher *c)=0;
virtual void add(lwe_cipher rop, lwe_cipher op1, lwe_cipher op2, int d)=0;
virtual void sub(lwe_cipher rop, lwe_cipher op1, lwe_cipher op2, int d)=0;
virtual void mulrdm(lwe_cipher rop, poly64 rdm)=0;
virtual void mul(lwe_cipher rop, lwe_in_data op1, lwe_query op2, uint64_t current_poly, int rec_lvl)=0;
virtual void mulandadd(lwe_cipher rop, lwe_in_data op1, lwe_query op2, uint64_t current_poly, int rec_lvl)=0;
virtual void mulandadd(lwe_cipher rop, const lwe_in_data op1, const lwe_query op2, const lwe_query op2prime, const uint64_t current_poly, int rec_lvl)=0;
virtual uint64_t* getmoduli()=0;
virtual long getnoise()=0;
virtual void setnbrbits(double nbr_bits)=0;

virtual poly64* deserializeDataNFL(unsigned char **inArrayOfBuffers, uint64_t nbrOfBuffers,
        uint64_t dataBitsizePerBuffer, unsigned bitsPerCoordinate, uint64_t &polyNumber)=0;
virtual void serializeData64 (uint64_t* indata, unsigned char* outdata,
        unsigned int bitsPerChunk, uint64_t nb_of_uint64)=0;
virtual void serializeData32 (uint32_t* indata, unsigned char* outdata, unsigned int bitsPerChunk,
        uint64_t nb_of_uint32)=0;

};

#endif
