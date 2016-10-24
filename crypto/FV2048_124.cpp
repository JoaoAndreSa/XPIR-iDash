
//#define PolyD 2048
//#define NbMod 124

//#define CAT_NX(A, B) A ## B

//#define PCAT(A, B) PPCAT_NX(A, B)

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
    ret.isnull=false;

    return ret;
}

FV::ciphertext_t transformtoFV_cipher(lwe_query c){

    FV::ciphertext_t ret;
    ret.c0 = transformtopolyp(c.a);
    ret.c1 = transformtopolyp(c.b);
    ret.isnull=false;

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
c_t.isnull=false;
c_t.pk = const_cast<FV::pk_t*>(this->public_key);
std::array<mpz_t, FV::params::poly_p::degree> polym;
  for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
    mpz_inits(polym[i], nullptr);
  }
  polym.begin();
FV::decrypt_poly(polym, *secret_key, *public_key, c_t);
//FV::params::poly_p p1;

    bzero(m,FV::params::poly_p::degree*FV::params::poly_p::nmoduli*sizeof(uint64_t));
    for (unsigned int i = 0 ; i < FV::params::poly_p::degree ; i++)
	  {
    	// Combien d'uint32 ?
	    int combien = ceil(nbrbit/32);
	    mpz_export(((uint32_t*)m)+i*combien, NULL, -1, sizeof(uint32_t), 0, 0, polym[i]);
	  }



/*

p1.mpz2poly(polym);

for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++) {
    for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
        m[cm * FV::params::poly_p::degree + i] = p1(cm, i);
    }
   }*/
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
rop_t.isnull=false;
op1_t.isnull=false;
op2_t.isnull=false;
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

    if (rec_lvl==1){

FV::ciphertext_t rop_t = transformtoFV_cipher(rop);
    FV::ciphertext_t op2_t = transformtoFV_cipher(op2);

/*
if(rop_t.c0(0,0)!=0){
    std::cout << "AVANT OPERATION rop";
    std::cout<<rop_t.c0(0,0)<< " ";
    std::cout << endl;
}*/
    rop_t.pk = const_cast<FV::pk_t*>(this->public_key);
    op2_t.pk = const_cast<FV::pk_t*>(this->public_key);

    FV::params::poly_p tmp = transformtopolyp(op1.p[current_poly]);
//    std::cout << "AVANT OPERATION tmp ";
//    std::cout<<tmp(0,0)<< " ";
 //   std::cout << endl;

//    std::cout << "AVANT OPERATION op2_t ";
 //   std::cout<<op2_t.c0(0,0)<< " ";
 //   std::cout << endl;

    rop_t = rop_t + op2_t*tmp;

  //  std::cout << "ApRES OPERATION ";
  //  std::cout<<rop_t.c0(0,0)<< " ";
  //  std::cout << endl;


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

    else {
    FV::ciphertext_t rop_t = transformtoFV_cipher(rop);
    FV::ciphertext_t op2_t = transformtoFV_cipher(op2);


    //std::cout << "AVANT OPERATION rop";
   // std::cout<<rop_t.c0(0,0)<< " ";
    //std::cout << endl;

    rop_t.pk = const_cast<FV::pk_t*>(this->public_key);
    op2_t.pk = const_cast<FV::pk_t*>(this->public_key);

    FV::params::poly_p tmp = transformtopolyp(op1.p[current_poly]);
   // std::cout << "AVANT OPERATION tmp ";
   // std::cout<<tmp(0,0)<< " ";
   // std::cout << endl;

   // std::cout << "AVANT OPERATION op2_t ";
   // std::cout<<op2_t.c0(0,0)<< " ";
   // std::cout << endl;

    rop_t = op2_t*tmp + rop_t;

   // std::cout << "ApRES OPERATION ";
   // std::cout<<rop_t.c0(0,0)<< " ";
   // std::cout << endl;


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

//bitsplit.deserializeDataNFL(inArrayOfBuffers,nbrOfBuffers,dataBitsizePerBuffer,bitsPerCoordinate,polyNumber);


  // We need to handle dataBitsize bits of data per buffer
  // each poly can take publicParams.getAbsorptionBitsize() bits so
  polyNumber = ceil((double)dataBitsizePerBuffer*(double)nbrOfBuffers/(double)(bitsPerCoordinate*FV::params::poly_p::degree));

  // The uint64_t arrays are allocated and filled with zeros
  // So that we do not have to pad with zeros beyond the limit
  poly64* deserData = (poly64 *) calloc(polyNumber, sizeof(poly64));

  // bitsplitter does all the hard work WITHOUT using large numbers !
  deserData[0] = bitsplit.bitsplitter(inArrayOfBuffers, nbrOfBuffers, dataBitsizePerBuffer, bitsPerCoordinate);

  // We finish the work by applying the NTT transform
#ifdef MULTI_THREAD
  #pragma omp parallel for
#endif
  for (unsigned int i = 0 ; i < polyNumber ; i++)
  {
    deserData[i] = deserData[0]+i*FV::params::poly_p::nmoduli*FV::params::poly_p::degree;
#ifndef SIMULATE_PRE_NTT_DATA
FV::params::poly_p tmp{deserData[i], deserData[i]+FV::params::poly_p::degree*FV::params::poly_p::nmoduli};
    tmp.FV::params::poly_p::ntt_pow_phi();
     for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++) {
    for (size_t j = 0; j < FV::params::poly_p::degree; j++) {
        deserData[i][cm * FV::params::poly_p::degree + j] = tmp(cm, j);
    }
   }
#endif
  }

  return deserData;

}

void FV2048_124c::serializeData64 (uint64_t* indata, unsigned char* outdata, unsigned int bitsPerChunk, uint64_t nb_of_uint64)
{
  bitsplit.serializeData64(indata,outdata,bitsPerChunk,nb_of_uint64);
}



void FV2048_124c::serializeData32 (uint32_t* indata, unsigned char* outdata, unsigned int bitsPerChunk, uint64_t nb_of_uint32){

  bitsplit.serializeData32(indata,outdata,bitsPerChunk,nb_of_uint32);

}

void FV2048_124c::setnbrbits(double nbr_bits){
nbrbit=nbr_bits;
}


}

FV2048_124::FV2048_124c::FV2048_124c()/*:
 secret_key(),
 evaluation_key(secret_key,32),
 public_key(secret_key,evaluation_key)*/{secret_key = new FV::sk_t();
 evaluation_key = new FV::evk_t(*secret_key,32);
 public_key = new FV::pk_t(*secret_key,*evaluation_key);
moduli = new uint64_t[FV::params::poly_p::nmoduli];
  for(unsigned short currentModulus=0;currentModulus<FV::params::poly_p::nmoduli;currentModulus++)
  {
    moduli[currentModulus] = FV::params::poly_p::get_modulus(currentModulus);;
  }
bitsplit = Bitsplit(moduli,FV::params::poly_p::nmoduli,FV::params::poly_p::degree);
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
c_t.isnull=false;
c_t.pk = const_cast<FV::pk_t*>(this->public_key);
std::array<mpz_t, FV::params::poly_p::degree> polym;
  for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
    mpz_inits(polym[i], nullptr);
  }
  polym.begin();
FV::decrypt_poly(polym, *secret_key, *public_key, c_t);
//FV::params::poly_p p1;

    bzero(m,FV::params::poly_p::degree*FV::params::poly_p::nmoduli*sizeof(uint64_t));
    for (unsigned int i = 0 ; i < FV::params::poly_p::degree ; i++)
	  {
    	// Combien d'uint32 ?
	    int combien = ceil(nbrbit/32);
	    mpz_export(((uint32_t*)m)+i*combien, NULL, -1, sizeof(uint32_t), 0, 0, polym[i]);
	  }



/*

p1.mpz2poly(polym);

for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++) {
    for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
        m[cm * FV::params::poly_p::degree + i] = p1(cm, i);
    }
   }*/
     for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
    mpz_clears(polym[i], nullptr);
  }

}

void FV1024_62c::add(lwe_cipher rop, lwe_cipher op1, lwe_cipher op2, int d)
{
FV::ciphertext_t rop_t,op1_t,op2_t;
rop_t = transformtoFV_cipher(rop);
op1_t = transformtoFV_cipher(op1);
op2_t = transformtoFV_cipher(op2);
rop_t.isnull=false;
op1_t.isnull=false;
op2_t.isnull=false;
rop_t = op1_t+op2_t;

  transformtolwe_cipher(rop_t);
}

void FV1024_62c::mulandadd(lwe_cipher rop, lwe_in_data op1, lwe_query op2, uint64_t current_poly, int rec_lvl)
{

    FV::ciphertext_t rop_t = transformtoFV_cipher(rop);
 //   FV::ciphertext_t rop_t2 = transformtoFV_cipher(rop);
    FV::ciphertext_t op2_t = transformtoFV_cipher(op2);

    rop_t.isnull=false;
    op2_t.isnull=false;

    rop_t.pk = const_cast<FV::pk_t*>(this->public_key);
    op2_t.pk = const_cast<FV::pk_t*>(this->public_key);

    FV::params::poly_p tmp = transformtopolyp(op1.p[current_poly]);

    rop_t = op2_t*tmp + rop_t;




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

void FV1024_62c::setnbrbits(double nbr_bits){
nbrbit=nbr_bits;
}

}


FV1024_62::FV1024_62c::FV1024_62c()/*:
 secret_key(),
 evaluation_key(secret_key,32),
 public_key(secret_key,evaluation_key)*/{secret_key = new FV::sk_t();
 evaluation_key = new FV::evk_t(*secret_key,32);
 public_key = new FV::pk_t(*secret_key,*evaluation_key);
  moduli = new uint64_t[FV::params::poly_p::nmoduli];
  for(unsigned short currentModulus=0;currentModulus<FV::params::poly_p::nmoduli;currentModulus++)
  {
    moduli[currentModulus] = FV::params::poly_p::get_modulus(currentModulus);;
  }

bitsplit = Bitsplit(moduli,FV::params::poly_p::nmoduli,FV::params::poly_p::degree);
 };


