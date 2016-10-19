
#define PolyD 2048
#define NbMod 124

#define CAT_NX(A, B) A ## B

#define PCAT(A, B) PPCAT_NX(A, B)

#include "NFLFV.hpp"
#include "FVobject.hpp"
#include "FV2048_124.hpp"

#include <cstddef>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <nfl.hpp>



namespace FV2048_124{


namespace FV {
namespace params {
using poly_t = nfl::poly_from_modulus<uint64_t, 1 << 11, 124>;
template <typename T> struct plaintextModulus;
template <> struct plaintextModulus<mpz_class> {
  static mpz_class value() { return mpz_class("1073479681"); }
};
using gauss_struct = nfl::gaussian<uint16_t, uint64_t, 2>;
using gauss_t = nfl::FastGaussianNoise<uint16_t, uint64_t, 2>;
gauss_t fg_prng_sk(8.0, 128, 1 << 14);
gauss_t fg_prng_evk(8.0, 128, 1 << 14);
gauss_t fg_prng_pk(8.0, 128, 1 << 14);
gauss_t fg_prng_enc(8.0, 128, 1 << 14);



}
}  // namespace FV::params

#include "external_components/FV-NFLlib/FV.hpp"

FV::params::poly_p transformtopolyp(poly64 p){
    FV::params::poly_p ret{p, p+FV::params::poly_p::degree*FV::params::poly_p::nmoduli};
    return ret;
}

poly64 transformtopoly64(FV::params::poly_p p){
    poly64 ret = (poly64) calloc(FV::params::poly_p::degree * FV::params::poly_p::nmoduli,  sizeof(uint64_t));
  for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++) {
    for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
        ret[cm * FV::params::poly_p::degree + i] = p(cm, i);
    }
   }
    return ret;
}

void transformtolwe_cipher(FV::ciphertext_t c,lwe_cipher ret){

   // lwe_cipher ret;
   // ret.a =(poly64) calloc(FV::params::poly_p::degree * 2 * FV::params::poly_p::nmoduli,  sizeof(uint64_t));
  for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++) {
    for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
        ret.a[cm * FV::params::poly_p::degree + i] = c.c0(cm, i);
    }
   }
     for (size_t cm = FV::params::poly_p::nmoduli; cm < 2*FV::params::poly_p::nmoduli; cm++) {
    for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
        ret.b[cm * FV::params::poly_p::degree + i] = c.c1(cm-FV::params::poly_p::nmoduli, i);
    }
   }
   // ret.b = ret.a + FV::params::poly_p::degree * FV::params::poly_p::nmoduli;
   // return ret;
}

FV::ciphertext_t transformtoFV_cipher(lwe_cipher c){

    FV::ciphertext_t ret;
    ret.c0 = transformtopolyp(c.a);
    ret.c1 = transformtopolyp(c.b);

    return ret;
}

FV::ciphertext_t transformtoFV_cipher(lwe_query c){

    FV::ciphertext_t ret;
    ret.c0 = transformtopolyp(c.a);
    ret.c1 = transformtopolyp(c.b);

    return ret;
}

void  FV2048_124c::enc(lwe_cipher* c, poly64 m)
{
    c->a = (poly64) calloc(FV::params::poly_p::degree * 2 * FV::params::poly_p::nmoduli,  sizeof(uint64_t));
    c->b = c->a + FV::params::poly_p::degree * FV::params::poly_p::nmoduli;
    FV::ciphertext_t c_t;
    FV::params::poly_p m_t = transformtopolyp(m);
    FV::encrypt_poly(c_t, *public_key, m_t);
      for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++) {
    for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
        c->a[cm * FV::params::poly_p::degree + i] = c_t.c0(cm, i);
    }
   }
     for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++) {
    for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
        c->b[cm * FV::params::poly_p::degree + i] = c_t.c1(cm, i);
    }
   }

}

void FV2048_124c::dec(poly64 m, lwe_cipher *c)
{

FV::ciphertext_t c_t;
c_t = transformtoFV_cipher(*c);
std::array<mpz_t, FV::params::poly_p::degree> polym;
  for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
    mpz_inits(polym[i], nullptr);
  }
  polym.begin();
FV::decrypt_poly(polym, *secret_key, *public_key, c_t);
FV::params::poly_p p1;
p1.mpz2poly(polym);

for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++) {
    for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
        m[cm * FV::params::poly_p::degree + i] = p1(cm, i);
    }
   }
     for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
    mpz_clears(polym[i], nullptr);
  }

}

void FV2048_124c::add(lwe_cipher rop, lwe_cipher op1, lwe_cipher op2, int d)
{
FV::ciphertext_t rop_t,op1_t,op2_t;
rop_t = transformtoFV_cipher(rop);
op1_t = transformtoFV_cipher(op1);
op2_t = transformtoFV_cipher(op2);
rop_t = op1_t+op2_t;

	      for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++) {
    for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
        rop.a[cm * FV::params::poly_p::degree + i] = rop_t.c0(cm, i);
    }
   }
     for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++) {
    for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
        rop.b[cm * FV::params::poly_p::degree + i] = rop_t.c1(cm, i);
    }
   }

}

void FV2048_124c::mulandadd(lwe_cipher rop, lwe_in_data op1, lwe_query op2, uint64_t current_poly, int rec_lvl)
{

    FV::ciphertext_t rop_t = transformtoFV_cipher(rop);

    FV::ciphertext_t op2_t = transformtoFV_cipher(op2);
    rop_t.pk = const_cast<FV::pk_t*>(this->public_key);
    op2_t.pk = const_cast<FV::pk_t*>(this->public_key);
    rop_t = rop_t + op2_t*transformtopolyp(op1.p[current_poly]);

	      for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++) {
    for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
        rop.a[cm * FV::params::poly_p::degree + i] = rop_t.c0(cm, i);
    }
   }
     for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++) {
    for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
        rop.b[cm * FV::params::poly_p::degree + i] = rop_t.c1(cm, i);
    }
   }

}

void FV2048_124c::mulandadd(lwe_cipher rop, const lwe_in_data op1, const lwe_query op2, const lwe_query op2prime, const uint64_t current_poly, int rec_lvl)
{
}

uint64_t* FV2048_124c::getmoduli(){
return moduli;
}

long FV2048_124c::getnoise(){

return public_key->noise_max;
}

poly64 *FV2048_124c::deserializeDataNFL(unsigned char **inArrayOfBuffers, uint64_t nbrOfBuffers, uint64_t dataBitsizePerBuffer, unsigned bitsPerCoordinate, uint64_t &polyNumber) {

bitsplit.deserializeDataNFL(inArrayOfBuffers,nbrOfBuffers,dataBitsizePerBuffer,bitsPerCoordinate,polyNumber);

}

void FV2048_124c::serializeData64 (uint64_t* indata, unsigned char* outdata, unsigned int bitsPerChunk, uint64_t nb_of_uint64)
{
  bitsplit.serializeData64(indata,outdata,bitsPerChunk,nb_of_uint64);
}



void FV2048_124c::serializeData32 (uint32_t* indata, unsigned char* outdata, unsigned int bitsPerChunk, uint64_t nb_of_uint32){

  bitsplit.serializeData32(indata,outdata,bitsPerChunk,nb_of_uint32);

}



}

FV2048_124::FV2048_124c::FV2048_124c()/*:
 secret_key(),
 evaluation_key(secret_key,32),
 public_key(secret_key,evaluation_key)*/{secret_key = new FV::sk_t();
 evaluation_key = new FV::evk_t(*secret_key,32);
 public_key = new FV::pk_t(*secret_key,*evaluation_key);
moduli = new uint64_t[FV::params::poly_p::nmoduli/62];
  for(unsigned short currentModulus=0;currentModulus<FV::params::poly_p::nmoduli/62;currentModulus++)
  {
    moduli[currentModulus] = FV::params::poly_p::get_modulus(currentModulus);;
  }

bitsplit = Bitsplit(moduli,FV::params::poly_p::nmoduli/62,FV::params::poly_p::degree);
 };


namespace FV1024_62 {

namespace FV {
namespace params {
using poly_t = nfl::poly_from_modulus<uint64_t, 1 << 10, 62>;
template <typename T> struct plaintextModulus;
template <> struct plaintextModulus<mpz_class> {
  static mpz_class value() { return mpz_class("1073479681"); }
};
using gauss_struct = nfl::gaussian<uint16_t, uint64_t, 2>;
using gauss_t = nfl::FastGaussianNoise<uint16_t, uint64_t, 2>;
gauss_t fg_prng_sk(8.0, 128, 1 << 14);
gauss_t fg_prng_evk(8.0, 128, 1 << 14);
gauss_t fg_prng_pk(8.0, 128, 1 << 14);
gauss_t fg_prng_enc(8.0, 128, 1 << 14);
}
}  // namespace FV::params
#include "external_components/FV-NFLlib/FV.hpp"

FV::params::poly_p transformtopolyp(poly64 p){
    FV::params::poly_p ret{p, p+FV::params::poly_p::degree*FV::params::poly_p::nmoduli};
    return ret;
}
poly64 transformtopoly64(FV::params::poly_p p){
    poly64 ret = (poly64) calloc(FV::params::poly_p::degree * FV::params::poly_p::nmoduli,  sizeof(uint64_t));
  for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++) {
    for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
        ret[cm * FV::params::poly_p::degree + i] = p(cm, i);
    }
   }
    return ret;
}

lwe_cipher transformtolwe_cipher(FV::ciphertext_t c){

    poly64 ret_p =(poly64) calloc(FV::params::poly_p::degree * 2 * FV::params::poly_p::nmoduli,  sizeof(uint64_t));
  for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++) {
    for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
        ret_p[cm * FV::params::poly_p::degree + i] = c.c0(cm, i);
    }
   }
     for (size_t cm = FV::params::poly_p::nmoduli; cm < 2*FV::params::poly_p::nmoduli; cm++) {
    for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
        ret_p[cm * FV::params::poly_p::degree + i] = c.c1(cm-FV::params::poly_p::nmoduli, i);
    }
   }
    lwe_cipher ret;
    ret.a = ret_p;
    ret.b = ret.a + FV::params::poly_p::degree * FV::params::poly_p::nmoduli;
    return ret;
}

FV::ciphertext_t transformtoFV_cipher(lwe_cipher c){

    FV::ciphertext_t ret;
    ret.c0 = transformtopolyp(c.a);
    ret.c1 = transformtopolyp(c.b);

    return ret;
}

FV::ciphertext_t transformtoFV_cipher(lwe_query c){

    FV::ciphertext_t ret;
    ret.c0 = transformtopolyp(c.a);
    ret.c1 = transformtopolyp(c.b);

    return ret;
}

void  FV1024_62c::enc(lwe_cipher* c, poly64 m)
{

    FV::ciphertext_t c_t;
    FV::params::poly_p m_t = transformtopolyp(m);
    FV::encrypt_poly(c_t, *public_key, m_t);
    lwe_cipher c_tmp = transformtolwe_cipher(c_t);
    c->a=c_tmp.a;
    c->b=c_tmp.b;

}

void FV1024_62c::dec(poly64 m, lwe_cipher *c)
{

FV::ciphertext_t c_t;
c_t = transformtoFV_cipher(*c);
std::array<mpz_t, FV::params::poly_p::degree> polym;
  for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
    mpz_inits(polym[i], nullptr);
  }
  polym.begin();
FV::decrypt_poly(polym, *secret_key, *public_key, c_t);
FV::params::poly_p p1;
p1.mpz2poly(polym);
m = transformtopoly64(p1);

}

void FV1024_62c::add(lwe_cipher rop, lwe_cipher op1, lwe_cipher op2, int d)
{
FV::ciphertext_t rop_t,op1_t,op2_t;
rop_t = transformtoFV_cipher(rop);
op1_t = transformtoFV_cipher(op1);
op2_t = transformtoFV_cipher(op2);
rop_t = op1_t+op2_t;

  transformtolwe_cipher(rop_t);
}

void FV1024_62c::mulandadd(lwe_cipher rop, lwe_in_data op1, lwe_query op2, uint64_t current_poly, int rec_lvl)
{

    FV::ciphertext_t rop_t = transformtoFV_cipher(rop);
    FV::ciphertext_t op2_t = transformtoFV_cipher(op2);
    rop_t = rop_t + op2_t*transformtopolyp(op1.p[current_poly]);

	rop = transformtolwe_cipher(rop_t);

}

void FV1024_62c::mulandadd(lwe_cipher rop, const lwe_in_data op1, const lwe_query op2, const lwe_query op2prime, const uint64_t current_poly, int rec_lvl)
{
}

uint64_t* FV1024_62c::getmoduli(){
return moduli;
}

long FV1024_62c::getnoise(){
return public_key->noise_max;
}


poly64 *FV1024_62c::deserializeDataNFL(unsigned char **inArrayOfBuffers, uint64_t nbrOfBuffers, uint64_t dataBitsizePerBuffer, unsigned bitsPerCoordinate, uint64_t &polyNumber) {

bitsplit.deserializeDataNFL(inArrayOfBuffers,nbrOfBuffers,dataBitsizePerBuffer,bitsPerCoordinate,polyNumber);

}

void FV1024_62c::serializeData64 (uint64_t* indata, unsigned char* outdata, unsigned int bitsPerChunk, uint64_t nb_of_uint64)
{
  bitsplit.serializeData64(indata,outdata,bitsPerChunk,nb_of_uint64);
}



void FV1024_62c::serializeData32 (uint32_t* indata, unsigned char* outdata, unsigned int bitsPerChunk, uint64_t nb_of_uint32){

  bitsplit.serializeData32(indata,outdata,bitsPerChunk,nb_of_uint32);

}

}


FV1024_62::FV1024_62c::FV1024_62c()/*:
 secret_key(),
 evaluation_key(secret_key,32),
 public_key(secret_key,evaluation_key)*/{secret_key = new FV::sk_t();
 evaluation_key = new FV::evk_t(*secret_key,32);
 public_key = new FV::pk_t(*secret_key,*evaluation_key);
  moduli = new uint64_t[FV::params::poly_p::nmoduli/62];
  for(unsigned short currentModulus=0;currentModulus<FV::params::poly_p::nmoduli/62;currentModulus++)
  {
    moduli[currentModulus] = FV::params::poly_p::get_modulus(currentModulus);;
  }

bitsplit = Bitsplit(moduli,FV::params::poly_p::nmoduli/62,FV::params::poly_p::degree);
 };


