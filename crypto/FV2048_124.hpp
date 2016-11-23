#ifndef DEF_FV2048
#define DEF_FV2048

#include "NFLFV.hpp"
#include "Bitsplit.hpp"
#include "FVobject.hpp"

namespace FV2048_124{
namespace FV {
class sk_t;
class evk_t;
class pk_t;
}

class FV2048_124c : public FVobject {

public:
FV2048_124c();
virtual void encNTT(lwe_cipher* c, poly64 m);
virtual void enc(lwe_cipher* c, poly64 m);
virtual void dec(poly64 m, lwe_cipher *c);
virtual void add(lwe_cipher rop, lwe_cipher op1, lwe_cipher op2, int d);
virtual void sub(lwe_cipher rop, lwe_cipher op1, lwe_cipher op2, int d);
virtual void mulrdm(lwe_cipher rop, poly64 rdm);
virtual void mul(lwe_cipher rop, lwe_in_data op1, lwe_query op2, uint64_t current_poly, int rec_lvl);
virtual void mulandadd(lwe_cipher rop, lwe_in_data op1, lwe_query op2, uint64_t current_poly, int rec_lvl);
virtual void mulandadd(lwe_cipher rop, const lwe_in_data op1, const lwe_query op2, const lwe_query op2prime, const uint64_t current_poly, int rec_lvl);
virtual uint64_t* getmoduli();
virtual long getnoise();
virtual void setnbrbits(double nbr_bits);

virtual poly64* deserializeDataNFL(unsigned char **inArrayOfBuffers, uint64_t nbrOfBuffers,
        uint64_t dataBitsizePerBuffer, unsigned bitsPerCoordinate, uint64_t &polyNumber);
virtual void serializeData64 (uint64_t* indata, unsigned char* outdata,
        unsigned int bitsPerChunk, uint64_t nb_of_uint64);
virtual void serializeData32 (uint32_t* indata, unsigned char* outdata, unsigned int bitsPerChunk,
        uint64_t nb_of_uint32);

protected:
const FV::sk_t * secret_key;
const FV::evk_t * evaluation_key;
const FV::pk_t * public_key;
Bitsplit bitsplit;
uint64_t * moduli;
double nbrbit;

};
}



namespace FV1024_62 {
namespace FV {
class sk_t;
class evk_t;
class pk_t;

}
class FV1024_62c : public FVobject {

public:
FV1024_62c();
virtual void encNTT(lwe_cipher* c, poly64 m);
virtual void enc(lwe_cipher* c, poly64 m);
virtual void dec(poly64 m, lwe_cipher *c);
virtual void add(lwe_cipher rop, lwe_cipher op1, lwe_cipher op2, int d);
virtual void sub(lwe_cipher rop, lwe_cipher op1, lwe_cipher op2, int d);
virtual void mulrdm(lwe_cipher rop, poly64 rdm);
virtual void mul(lwe_cipher rop, lwe_in_data op1, lwe_query op2, uint64_t current_poly, int rec_lvl);
virtual void mulandadd(lwe_cipher rop, lwe_in_data op1, lwe_query op2, uint64_t current_poly, int rec_lvl);
virtual void mulandadd(lwe_cipher rop, const lwe_in_data op1, const lwe_query op2, const lwe_query op2prime, const uint64_t current_poly, int rec_lvl);
virtual uint64_t* getmoduli();
virtual long getnoise();
virtual void setnbrbits(double nbr_bits);

virtual poly64* deserializeDataNFL(unsigned char **inArrayOfBuffers, uint64_t nbrOfBuffers,
        uint64_t dataBitsizePerBuffer, unsigned bitsPerCoordinate, uint64_t &polyNumber);
virtual void serializeData64 (uint64_t* indata, unsigned char* outdata,
        unsigned int bitsPerChunk, uint64_t nb_of_uint64);
virtual void serializeData32 (uint32_t* indata, unsigned char* outdata, unsigned int bitsPerChunk,
        uint64_t nb_of_uint32);

protected:
const FV::sk_t * secret_key;
const FV::evk_t * evaluation_key;
const FV::pk_t * public_key;
Bitsplit bitsplit;
uint64_t * moduli;
double nbrbit;
poly64 mask;


};
}



#endif
