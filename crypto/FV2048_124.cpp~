
//#define PolyD 2048
//#define NbMod 124

//#define CAT_NX(A, B) A ## B

//#define PCAT(A, B) PPCAT_NX(A, B)

#include "NFLFV.hpp"
#include "FVobject.hpp"
#include "FV2048_124.hpp"

#include <iostream>

#include <fstream>

#include <cstddef>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <nfl.hpp>
#include "../external_components/NFLlib/tests/tools.h"



namespace FV2048_124{


namespace FV {
namespace params {
using poly_t = nfl::poly_from_modulus<uint64_t, 1 << 11, 124>;
using polyP_p = nfl::poly_p<uint32_t, poly_t::degree,1>;
template <typename T> struct plaintextModulus;
template <> struct plaintextModulus<mpz_class> {
  static mpz_class value() { return mpz_class(polyP_p::get_modulus(0)); }
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

FV::params::polyP_p transformtopolyPp(poly64 p){
    FV::params::polyP_p ret{p, p+FV::params::polyP_p::degree*FV::params::polyP_p::nmoduli};
    return ret;
}


FV::params::poly_p transformtopolyp(poly64 p){
    FV::params::poly_p ret{p, p+FV::params::poly_p::degree*FV::params::poly_p::nmoduli};
    return ret;
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
    for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++)
    {
        for (size_t i = 0; i < FV::params::poly_p::degree; i++)
        {
            c->a[cm * FV::params::poly_p::degree + i] = c_t.c0(cm, i);
        }
    }
    for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++)
    {
        for (size_t i = 0; i < FV::params::poly_p::degree; i++)
        {
            c->b[cm * FV::params::poly_p::degree + i] = c_t.c1(cm, i);
        }
    }

}

void  FV2048_124c::encNTT(lwe_cipher* c, poly64 m)
{
    c->a = (poly64) calloc(FV::params::poly_p::degree * 2 * FV::params::poly_p::nmoduli,  sizeof(uint64_t));
    c->b = c->a + FV::params::poly_p::degree * FV::params::poly_p::nmoduli;
    FV::ciphertext_t c_t;
    FV::params::poly_p m_t = transformtopolyp(m);
    FV::encrypt_poly(c_t, *public_key, m_t);
    for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++)
    {
        for (size_t i = 0; i < FV::params::poly_p::degree; i++)
        {
            c->a[cm * FV::params::poly_p::degree + i] = c_t.c0(cm, i);
        }
    }
    for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++)
    {
        for (size_t i = 0; i < FV::params::poly_p::degree; i++)
        {
            c->b[cm * FV::params::poly_p::degree + i] = c_t.c1(cm, i);
        }
    }

}

void FV2048_124c::dec(poly64 m, lwe_cipher *c)
{

    FV::params::polyP_p p2;
    FV::ciphertext_t c_t;
    c_t = transformtoFV_cipher(*c);
    c_t.pk = const_cast<FV::pk_t*>(this->public_key);
    std::array<mpz_t, FV::params::poly_p::degree> polym;
    for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
        mpz_inits(polym[i], nullptr);
    }
    polym.begin();
    FV::decrypt_poly(polym, *secret_key, *public_key, c_t);

    p2.mpz2poly(polym);
    p2.ntt_pow_phi();
    p2.poly2mpz(polym);


    bzero(m,FV::params::polyP_p::degree*FV::params::polyP_p::nmoduli*sizeof(uint32_t));
    for (unsigned int i = 0 ; i < FV::params::polyP_p::degree ; i++)
    {
	    int combien = ceil(nbrbit/32);
	    mpz_export(((uint32_t*)m)+i*combien, NULL, -1, sizeof(uint32_t), 0, 0, polym[i]);
    }

    for (size_t i = 0; i < FV::params::polyP_p::degree; i++)
    {
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

void FV2048_124c::sub(lwe_cipher rop, lwe_cipher op1, lwe_cipher op2, int d)
{

    FV::ciphertext_t rop_t,op1_t,op2_t;
    rop_t = transformtoFV_cipher(rop);
    op1_t = transformtoFV_cipher(op1);
    op2_t = transformtoFV_cipher(op2);
    rop_t = op1_t-op2_t;

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

void FV2048_124c::mul(lwe_cipher rop, lwe_in_data op1, lwe_query op2, uint64_t current_poly, int rec_lvl)
{

    FV::ciphertext_t rop_t = transformtoFV_cipher(rop);
    FV::ciphertext_t op2_t = transformtoFV_cipher(op2);


    rop_t.pk = const_cast<FV::pk_t*>(this->public_key);
    op2_t.pk = const_cast<FV::pk_t*>(this->public_key);

    FV::params::poly_p tmp = transformtopolyp(op1.p[current_poly]);


    rop_t = op2_t*tmp;


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
void FV2048_124c::mulrdm(lwe_cipher rop, poly64 rdm)
{

}
void FV2048_124c::mulandadd(lwe_cipher rop, const lwe_in_data op1, const lwe_query op2, const lwe_query op2prime, const uint64_t current_poly, int rec_lvl)
{
}

uint64_t* FV2048_124c::getmoduli()
{

    return moduli;

}

long FV2048_124c::getnoise()
{

    return public_key->noise_max;

}

poly64 *FV2048_124c::deserializeDataNFL(unsigned char **inArrayOfBuffers, uint64_t nbrOfBuffers, uint64_t dataBitsizePerBuffer, unsigned bitsPerCoordinate, uint64_t &polyNumber,int first_)
{

  polyNumber = ceil((double)dataBitsizePerBuffer*(double)nbrOfBuffers/(double)(bitsPerCoordinate*FV::params::poly_p::degree));

  poly64* deserData = (poly64 *) calloc(polyNumber, sizeof(poly64));

  deserData[0] = bitsplit.bitsplitter(inArrayOfBuffers, nbrOfBuffers, dataBitsizePerBuffer, bitsPerCoordinate);

#ifdef MULTI_THREAD
  #pragma omp parallel for
#endif
  for (unsigned int i = 0 ; i < polyNumber ; i++)
  {
    deserData[i] = deserData[0]+i*FV::params::poly_p::nmoduli*FV::params::poly_p::degree;

    FV::params::polyP_p Pp{deserData[i], deserData[i]+FV::params::poly_p::degree, true};
    Pp.invntt_pow_invphi();
    FV::params::poly_p p;
    for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++)
    {
        for (size_t k = 0; k < FV::params::poly_p::degree; k++)
        {
            p(cm,k) = Pp(0, k);
        }
    }

    /*
    std::array<mpz_t, FV::params::poly_p::degree> polym;
    for (size_t j = 0; j < FV::params::poly_p::degree; j++) {
        mpz_inits(polym[j], nullptr);
    }

    p.poly2mpz(polym);
    FV::params::polyP_p Pp;
    Pp.mpz2poly(polym);
    Pp.invntt_pow_invphi();
    Pp.poly2mpz(polym);
    p.mpz2poly(polym);
    */

    p.ntt_pow_phi();
    for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++)
    {
        for (size_t k = 0; k < FV::params::poly_p::degree; k++)
        {
            deserData[i][cm * FV::params::poly_p::degree + k] = p(cm, k);
        }
    }
    /*
#ifndef SIMULATE_PRE_NTT_DATA
    FV::params::poly_p tmp{deserData[i], deserData[i]+FV::params::poly_p::degree*FV::params::poly_p::nmoduli};
    tmp.ntt_pow_phi();
    for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++) {
      for (size_t j = 0; j < FV::params::poly_p::degree; j++) {
        deserData[i][cm * FV::params::poly_p::degree + j] = tmp(cm, j);
      }
   }
#endif*/
  }

  return deserData;

}

poly64 *FV2048_124c::deserializeData(unsigned char **inArrayOfBuffers, uint64_t nbrOfBuffers, uint64_t dataBitsizePerBuffer, unsigned bitsPerCoordinate, uint64_t &polyNumber)
{
bitsplit.deserializeDataNFL(inArrayOfBuffers,nbrOfBuffers,dataBitsizePerBuffer,bitsPerCoordinate,polyNumber);
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

FV2048_124::FV2048_124c::FV2048_124c()
{
    secret_key = new FV::sk_t();
    evaluation_key = new FV::evk_t(*secret_key,32);
    public_key = new FV::pk_t(*secret_key,*evaluation_key);
    moduli = new uint64_t[FV::params::poly_p::nmoduli];
    for(unsigned short currentModulus=0;currentModulus < FV::params::poly_p::nmoduli;currentModulus++)
    {
        moduli[currentModulus] = FV::params::poly_p::get_modulus(currentModulus);
    }
    bitsplit = Bitsplit(moduli,FV::params::poly_p::nmoduli,FV::params::poly_p::degree);
 };


namespace FV1024_62 {

namespace FV {
namespace params {
using poly_t = nfl::poly_from_modulus<uint64_t, 1 << 10, 62>;
using polyP_p = nfl::poly_p<uint16_t, poly_t::degree,1>;
template <typename T> struct plaintextModulus;
template <> struct plaintextModulus<mpz_class> {
  static mpz_class value() { return mpz_class(polyP_p::get_modulus(0)); }
};
using gauss_struct = nfl::gaussian<uint16_t, uint64_t, 2>;
using gauss_t = nfl::FastGaussianNoise<uint16_t, uint64_t, 2>;
gauss_t fg_prng_sk(8.0, 124 ,1<< 14);
gauss_t fg_prng_evk(8.0, 124, 1 << 14);
gauss_t fg_prng_pk(8.0, 124, 1 << 14);
gauss_t fg_prng_enc(8.0, 124, 1 << 14);
}
}  // namespace FV::params
#include "external_components/FV-NFLlib/FV.hpp"

FV::params::polyP_p transformtopolyPp(poly64 p){
    FV::params::polyP_p ret{p, p+FV::params::polyP_p::degree*FV::params::polyP_p::nmoduli};
    return ret;
}


FV::params::poly_p transformtopolyp(poly64 p){
    FV::params::poly_p ret{p, p+FV::params::poly_p::degree*FV::params::poly_p::nmoduli};
    return ret;
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

void  FV1024_62c::encNTT(lwe_cipher* c, poly64 m)
{
    c->a = (poly64) calloc(FV::params::poly_p::degree * 2 * FV::params::poly_p::nmoduli,  sizeof(uint64_t));
    c->b = c->a + FV::params::poly_p::degree * FV::params::poly_p::nmoduli;
    FV::ciphertext_t c_t;
    FV::params::polyP_p m_t1 = transformtopolyPp(m);
    m_t1.invntt_pow_invphi();
    FV::params::poly_p m_t;
    for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++)
    {
        for (size_t i = 0; i < FV::params::poly_p::degree; i++)
        {
            m_t(cm, i) = m_t1(cm, i);
        }
    }
    FV::encrypt_poly(c_t, *public_key, m_t);
    for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++)
    {
        for (size_t i = 0; i < FV::params::poly_p::degree; i++)
        {
            c->a[cm * FV::params::poly_p::degree + i] = c_t.c0(cm, i);
            c->b[cm * FV::params::poly_p::degree + i] = c_t.c1(cm, i);
        }
    }

}

void  FV1024_62c::enc(lwe_cipher* c, poly64 m)
{
    c->a = (poly64) calloc(FV::params::poly_p::degree * 2 * FV::params::poly_p::nmoduli,  sizeof(uint64_t));
    c->b = c->a + FV::params::poly_p::degree * FV::params::poly_p::nmoduli;
    FV::ciphertext_t c_t;
    FV::params::poly_p m_t = transformtopolyp(m);
    FV::encrypt_poly(c_t, *public_key, m_t);
    for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++)
    {
        for (size_t i = 0; i < FV::params::poly_p::degree; i++)
        {
            c->a[cm * FV::params::poly_p::degree + i] = c_t.c0(cm, i);
            c->b[cm * FV::params::poly_p::degree + i] = c_t.c1(cm, i);
        }
    }

}

void FV1024_62c::dec(poly64 m, lwe_cipher *c)
{

    FV::params::polyP_p p2;
    FV::ciphertext_t c_t;
    c_t = transformtoFV_cipher(*c);
    c_t.pk = const_cast<FV::pk_t*>(this->public_key);
    std::array<mpz_t, FV::params::poly_p::degree> polym;
    for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
        mpz_inits(polym[i], nullptr);
    }
    polym.begin();
    FV::decrypt_poly(polym, *secret_key, *public_key, c_t);

    p2.mpz2poly(polym);
    p2.ntt_pow_phi();
    p2.poly2mpz(polym);


    bzero(m,FV::params::polyP_p::degree*FV::params::polyP_p::nmoduli*sizeof(uint32_t));
    for (unsigned int i = 0 ; i < FV::params::polyP_p::degree ; i++)
    {
	    int combien = ceil(nbrbit/32);
	    mpz_export(((uint32_t*)m)+i*combien, NULL, -1, sizeof(uint32_t), 0, 0, polym[i]);
    }

    for (size_t i = 0; i < FV::params::polyP_p::degree; i++)
    {
        mpz_clears(polym[i], nullptr);
    }
}

void FV1024_62c::add(lwe_cipher rop, lwe_cipher op1, lwe_cipher op2, int d)
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

void FV1024_62c::sub(lwe_cipher rop, lwe_cipher op1, lwe_cipher op2, int d)
{

    FV::ciphertext_t rop_t,op1_t,op2_t;
    rop_t = transformtoFV_cipher(rop);
    op1_t = transformtoFV_cipher(op1);
    op2_t = transformtoFV_cipher(op2);
    rop_t.pk = const_cast<FV::pk_t*>(this->public_key);
    op1_t.pk = const_cast<FV::pk_t*>(this->public_key);
    op2_t.pk = const_cast<FV::pk_t*>(this->public_key);

    rop_t = op1_t-op2_t;

    for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++) {
        for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
            rop.a[cm * FV::params::poly_p::degree + i] = rop_t.c0(cm, i);
            rop.b[cm * FV::params::poly_p::degree + i] = rop_t.c1(cm, i);
        }
    }


}

void FV1024_62c::mulandadd(lwe_cipher rop, lwe_in_data op1, lwe_query op2, uint64_t current_poly, int rec_lvl)
{


    FV::params::poly_p ropta{rop.a, rop.a+FV::params::poly_p::degree};
    FV::params::poly_p roptb{rop.b, rop.b+FV::params::poly_p::degree};

    FV::params::poly_p op2ta{op2.a, op2.a+FV::params::poly_p::degree};
    FV::params::poly_p op2tb{op2.b, op2.b+FV::params::poly_p::degree};

    FV::params::poly_p tmp{op1.p[current_poly], op1.p[current_poly]+FV::params::poly_p::degree};

    ropta=op2ta*tmp+ropta;
    roptb=op2tb*tmp+roptb;


int i;
        for (i = 0; i < FV::params::poly_p::degree; i++) {
            rop.a[i] = ropta(0, i);
            rop.b[i] = roptb(0, i);
        }


}

void FV1024_62c::mulrdm(lwe_cipher rop, poly64 rdm)
{
FV::ciphertext_t rop_t = transformtoFV_cipher(rop);
rop_t.pk = const_cast<FV::pk_t*>(this->public_key);
poly64 mask_t = (poly64) calloc(FV::params::poly_p::degree,  sizeof(uint64_t));
ifstream fichier("mask.txt", ios::in);
for(int i=0;i<FV::params::poly_p::degree;i++){
uint64_t entier;
fichier>>entier;
mask_t[i]=entier;
}

fichier.close();
FV::params::poly_p rdm_t = transformtopolyp(mask_t);
rop_t = rop_t*rdm_t;

int i;
        for (i = 0; i < FV::params::poly_p::degree; i++) {
            rop.a[i] = rop_t.c0(0, i);
            rop.b[i] = rop_t.c1(0, i);
        }
        free(mask_t);

}

void FV1024_62c::mul(lwe_cipher rop, lwe_in_data op1, lwe_query op2, uint64_t current_poly, int rec_lvl)
{

    FV::ciphertext_t rop_t = transformtoFV_cipher(rop);
    FV::ciphertext_t op2_t = transformtoFV_cipher(op2);


    rop_t.pk = const_cast<FV::pk_t*>(this->public_key);
    op2_t.pk = const_cast<FV::pk_t*>(this->public_key);

    FV::params::poly_p tmp = transformtopolyp(op1.p[current_poly]);


    rop_t = op2_t*tmp;


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

uint64_t* FV1024_62c::getmoduli()
{

    return moduli;

}

long FV1024_62c::getnoise()
{

    return public_key->noise_max;

}

poly64 *FV1024_62c::deserializeDataNFL(unsigned char **inArrayOfBuffers, uint64_t nbrOfBuffers, uint64_t dataBitsizePerBuffer, unsigned bitsPerCoordinate, uint64_t &polyNumber, int first_)
{
poly64 mask_t = (poly64) calloc(FV::params::poly_p::degree,  sizeof(uint64_t));
ifstream fichier("mask2.txt", ios::in);
for(int i=0;i<FV::params::poly_p::degree;i++){
uint64_t entier;
fichier>>entier;
mask_t[i]=entier;
}
fichier.close();

  polyNumber = ceil((double)dataBitsizePerBuffer*(double)nbrOfBuffers/(double)(bitsPerCoordinate*FV::params::poly_p::degree));
  poly64* deserData = (poly64 *) calloc(polyNumber, sizeof(poly64));

  deserData[0] = bitsplit.bitsplitter(inArrayOfBuffers, nbrOfBuffers, dataBitsizePerBuffer, bitsPerCoordinate);

#ifdef MULTI_THREAD
  #pragma omp parallel for
#endif
  for (unsigned int i = 0 ; i < polyNumber ; i++)
  {
    deserData[i] = deserData[0]+i*FV::params::poly_p::nmoduli*FV::params::poly_p::degree;
    FV::params::polyP_p Pp{deserData[i], deserData[i]+FV::params::poly_p::degree, true};
    if(first_==1){
    //Pp=Pp*transformtopolyPp(mask_t);
    }
    Pp.invntt_pow_invphi();
    FV::params::poly_p p;
    for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++)
    {
        for (size_t k = 0; k < FV::params::poly_p::degree; k++)
        {
            p(cm,k) = Pp(0, k);
        }
    }


    p.ntt_pow_phi();
    //p = p*transformtopolyp(mask_t);

    for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++)
    {
        for (size_t k = 0; k < FV::params::poly_p::degree; k++)
        {
            deserData[i][cm * FV::params::poly_p::degree + k] = p(cm, k);
        }
    }

  }
free(mask_t);
  return deserData;

}

poly64 *FV1024_62c::deserializeData(unsigned char **inArrayOfBuffers, uint64_t nbrOfBuffers, uint64_t dataBitsizePerBuffer, unsigned bitsPerCoordinate, uint64_t &polyNumber)
{
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

FV1024_62::FV1024_62c::FV1024_62c()
{
    secret_key = new FV::sk_t();
    evaluation_key = new FV::evk_t(*secret_key,32);
    public_key = new FV::pk_t(*secret_key,*evaluation_key);
    moduli = new uint64_t[FV::params::poly_p::nmoduli];
    for(unsigned short currentModulus=0;currentModulus < FV::params::poly_p::nmoduli;currentModulus++)
    {
        moduli[currentModulus] = FV::params::poly_p::get_modulus(currentModulus);
    }
    bitsplit = Bitsplit(moduli,FV::params::poly_p::nmoduli,FV::params::poly_p::degree);
    mask = (poly64) calloc(FV::params::poly_p::degree,  sizeof(uint64_t));
    poly64 mask2 = (poly64) calloc(FV::params::poly_p::degree,  sizeof(uint64_t));
    unsigned char * rnd = (unsigned char *)calloc (8*FV::params::poly_p::degree,8);
    nfl::fastrandombytes(rnd, FV::params::poly_p::degree*sizeof(uint64_t));
    mask = (poly64) rnd;
    for (int i =0;i<FV::params::poly_p::degree;i++){
        mask[i]=mask[i]&255;
        mask2[i]=mask[i]&255;
    }
    FV::params::polyP_p m_t1 = transformtopolyPp(mask);
    m_t1.invntt_pow_invphi();
    FV::params::poly_p m_t;
    for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++)
    {
        for (size_t i = 0; i < FV::params::poly_p::degree; i++)
        {
            m_t(cm, i) = m_t1(cm, i);
        }
    }
    m_t.ntt_pow_phi();
    for (int i =0;i<FV::params::poly_p::degree;i++){
        mask[i]=m_t(0,i);
    }

ifstream fichier("mask.txt", ios::in);
char caractere;
fichier.get(caractere);
if(fichier.eof()){
fichier.close();
ofstream fichier("mask.txt", ios::out | ios::trunc);
    for (int i =0;i<FV::params::poly_p::degree;i++){
        fichier<<mask[i]<<" ";
    }

}
fichier.close();
ifstream fichier2("mask2.txt", ios::in);
char caractere2;
fichier2.get(caractere2);
if(fichier2.eof()){
fichier2.close();
ofstream fichier2("mask2.txt", ios::out | ios::trunc);
    for (int i =0;i<FV::params::poly_p::degree;i++){
        fichier2<<mask2[i]<<" ";
    }

}
fichier2.close();
 };



 namespace FV2048_62 {

namespace FV {
namespace params {
using poly_t = nfl::poly_from_modulus<uint64_t, 1 << 10, 62>;
using polyP_p = nfl::poly_p<uint16_t, poly_t::degree,1>;
template <typename T> struct plaintextModulus;
template <> struct plaintextModulus<mpz_class> {
  static mpz_class value() { return mpz_class(polyP_p::get_modulus(0)); }
};
using gauss_struct = nfl::gaussian<uint16_t, uint64_t, 2>;
using gauss_t = nfl::FastGaussianNoise<uint16_t, uint64_t, 2>;
gauss_t fg_prng_sk(8.0, 124 ,1<< 14);
gauss_t fg_prng_evk(8.0, 124, 1 << 14);
gauss_t fg_prng_pk(8.0, 124, 1 << 14);
gauss_t fg_prng_enc(8.0, 124, 1 << 14);
}
}  // namespace FV::params
#include "external_components/FV-NFLlib/FV.hpp"

FV::params::polyP_p transformtopolyPp(poly64 p){
    FV::params::polyP_p ret{p, p+FV::params::polyP_p::degree*FV::params::polyP_p::nmoduli};
    return ret;
}


FV::params::poly_p transformtopolyp(poly64 p){
    FV::params::poly_p ret{p, p+FV::params::poly_p::degree*FV::params::poly_p::nmoduli};
    return ret;
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

void  FV2048_62c::encNTT(lwe_cipher* c, poly64 m)
{
    c->a = (poly64) calloc(FV::params::poly_p::degree * 2 * FV::params::poly_p::nmoduli,  sizeof(uint64_t));
    c->b = c->a + FV::params::poly_p::degree * FV::params::poly_p::nmoduli;
    FV::ciphertext_t c_t;
    FV::params::polyP_p m_t1 = transformtopolyPp(m);
    m_t1.invntt_pow_invphi();
    FV::params::poly_p m_t;
    for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++)
    {
        for (size_t i = 0; i < FV::params::poly_p::degree; i++)
        {
            m_t(cm, i) = m_t1(cm, i);
        }
    }
    FV::encrypt_poly(c_t, *public_key, m_t);
    for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++)
    {
        for (size_t i = 0; i < FV::params::poly_p::degree; i++)
        {
            c->a[cm * FV::params::poly_p::degree + i] = c_t.c0(cm, i);
            c->b[cm * FV::params::poly_p::degree + i] = c_t.c1(cm, i);
        }
    }

}

void  FV2048_62c::enc(lwe_cipher* c, poly64 m)
{
    c->a = (poly64) calloc(FV::params::poly_p::degree * 2 * FV::params::poly_p::nmoduli,  sizeof(uint64_t));
    c->b = c->a + FV::params::poly_p::degree * FV::params::poly_p::nmoduli;
    FV::ciphertext_t c_t;
    FV::params::poly_p m_t = transformtopolyp(m);
    FV::encrypt_poly(c_t, *public_key, m_t);
    for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++)
    {
        for (size_t i = 0; i < FV::params::poly_p::degree; i++)
        {
            c->a[cm * FV::params::poly_p::degree + i] = c_t.c0(cm, i);
            c->b[cm * FV::params::poly_p::degree + i] = c_t.c1(cm, i);
        }
    }

}

void FV2048_62c::dec(poly64 m, lwe_cipher *c)
{

    FV::params::polyP_p p2;
    FV::ciphertext_t c_t;
    c_t = transformtoFV_cipher(*c);
    c_t.pk = const_cast<FV::pk_t*>(this->public_key);
    std::array<mpz_t, FV::params::poly_p::degree> polym;
    for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
        mpz_inits(polym[i], nullptr);
    }
    polym.begin();
    FV::decrypt_poly(polym, *secret_key, *public_key, c_t);

    p2.mpz2poly(polym);
    p2.ntt_pow_phi();
    p2.poly2mpz(polym);


    bzero(m,FV::params::polyP_p::degree*FV::params::polyP_p::nmoduli*sizeof(uint32_t));
    for (unsigned int i = 0 ; i < FV::params::polyP_p::degree ; i++)
    {
	    int combien = ceil(nbrbit/32);
	    mpz_export(((uint32_t*)m)+i*combien, NULL, -1, sizeof(uint32_t), 0, 0, polym[i]);
    }

    for (size_t i = 0; i < FV::params::polyP_p::degree; i++)
    {
        mpz_clears(polym[i], nullptr);
    }
}

void FV2048_62c::add(lwe_cipher rop, lwe_cipher op1, lwe_cipher op2, int d)
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

void FV2048_62c::sub(lwe_cipher rop, lwe_cipher op1, lwe_cipher op2, int d)
{

    FV::ciphertext_t rop_t,op1_t,op2_t;
    rop_t = transformtoFV_cipher(rop);
    op1_t = transformtoFV_cipher(op1);
    op2_t = transformtoFV_cipher(op2);
    rop_t.pk = const_cast<FV::pk_t*>(this->public_key);
    op1_t.pk = const_cast<FV::pk_t*>(this->public_key);
    op2_t.pk = const_cast<FV::pk_t*>(this->public_key);

    rop_t = op1_t-op2_t;

    for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++) {
        for (size_t i = 0; i < FV::params::poly_p::degree; i++) {
            rop.a[cm * FV::params::poly_p::degree + i] = rop_t.c0(cm, i);
            rop.b[cm * FV::params::poly_p::degree + i] = rop_t.c1(cm, i);
        }
    }


}

void FV2048_62c::mulandadd(lwe_cipher rop, lwe_in_data op1, lwe_query op2, uint64_t current_poly, int rec_lvl)
{


    FV::params::poly_p ropta{rop.a, rop.a+FV::params::poly_p::degree};
    FV::params::poly_p roptb{rop.b, rop.b+FV::params::poly_p::degree};

    FV::params::poly_p op2ta{op2.a, op2.a+FV::params::poly_p::degree};
    FV::params::poly_p op2tb{op2.b, op2.b+FV::params::poly_p::degree};

    FV::params::poly_p tmp{op1.p[current_poly], op1.p[current_poly]+FV::params::poly_p::degree};

    ropta=op2ta*tmp+ropta;
    roptb=op2tb*tmp+roptb;


int i;
        for (i = 0; i < FV::params::poly_p::degree; i++) {
            rop.a[i] = ropta(0, i);
            rop.b[i] = roptb(0, i);
        }


}

void FV2048_62c::mulrdm(lwe_cipher rop, poly64 rdm)
{
FV::ciphertext_t rop_t = transformtoFV_cipher(rop);
rop_t.pk = const_cast<FV::pk_t*>(this->public_key);
poly64 mask_t = (poly64) calloc(FV::params::poly_p::degree,  sizeof(uint64_t));
ifstream fichier("mask.txt", ios::in);
for(int i=0;i<FV::params::poly_p::degree;i++){
uint64_t entier;
fichier>>entier;
mask_t[i]=entier;
}

fichier.close();
FV::params::poly_p rdm_t = transformtopolyp(mask_t);
rop_t = rop_t*rdm_t;

int i;
        for (i = 0; i < FV::params::poly_p::degree; i++) {
            rop.a[i] = rop_t.c0(0, i);
            rop.b[i] = rop_t.c1(0, i);
        }
        free(mask_t);

}

void FV2048_62c::mul(lwe_cipher rop, lwe_in_data op1, lwe_query op2, uint64_t current_poly, int rec_lvl)
{

    FV::ciphertext_t rop_t = transformtoFV_cipher(rop);
    FV::ciphertext_t op2_t = transformtoFV_cipher(op2);


    rop_t.pk = const_cast<FV::pk_t*>(this->public_key);
    op2_t.pk = const_cast<FV::pk_t*>(this->public_key);

    FV::params::poly_p tmp = transformtopolyp(op1.p[current_poly]);


    rop_t = op2_t*tmp;


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

void FV2048_62c::mulandadd(lwe_cipher rop, const lwe_in_data op1, const lwe_query op2, const lwe_query op2prime, const uint64_t current_poly, int rec_lvl)
{
}

uint64_t* FV2048_62c::getmoduli()
{

    return moduli;

}

long FV2048_62c::getnoise()
{

    return public_key->noise_max;

}

poly64 *FV2048_62c::deserializeDataNFL(unsigned char **inArrayOfBuffers, uint64_t nbrOfBuffers, uint64_t dataBitsizePerBuffer, unsigned bitsPerCoordinate, uint64_t &polyNumber, int first_)
{
poly64 mask_t = (poly64) calloc(FV::params::poly_p::degree,  sizeof(uint64_t));
ifstream fichier("mask2.txt", ios::in);
for(int i=0;i<FV::params::poly_p::degree;i++){
uint64_t entier;
fichier>>entier;
mask_t[i]=entier;
}
fichier.close();

  polyNumber = ceil((double)dataBitsizePerBuffer*(double)nbrOfBuffers/(double)(bitsPerCoordinate*FV::params::poly_p::degree));
  poly64* deserData = (poly64 *) calloc(polyNumber, sizeof(poly64));

  deserData[0] = bitsplit.bitsplitter(inArrayOfBuffers, nbrOfBuffers, dataBitsizePerBuffer, bitsPerCoordinate);

#ifdef MULTI_THREAD
  #pragma omp parallel for
#endif
  for (unsigned int i = 0 ; i < polyNumber ; i++)
  {
    deserData[i] = deserData[0]+i*FV::params::poly_p::nmoduli*FV::params::poly_p::degree;
    FV::params::polyP_p Pp{deserData[i], deserData[i]+FV::params::poly_p::degree, true};
    if(first_==1){
    //Pp=Pp*transformtopolyPp(mask_t);
    }
    Pp.invntt_pow_invphi();
    FV::params::poly_p p;
    for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++)
    {
        for (size_t k = 0; k < FV::params::poly_p::degree; k++)
        {
            p(cm,k) = Pp(0, k);
        }
    }


    p.ntt_pow_phi();
    //p = p*transformtopolyp(mask_t);

    for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++)
    {
        for (size_t k = 0; k < FV::params::poly_p::degree; k++)
        {
            deserData[i][cm * FV::params::poly_p::degree + k] = p(cm, k);
        }
    }

  }
free(mask_t);
  return deserData;

}

poly64 *FV2048_62c::deserializeData(unsigned char **inArrayOfBuffers, uint64_t nbrOfBuffers, uint64_t dataBitsizePerBuffer, unsigned bitsPerCoordinate, uint64_t &polyNumber)
{
bitsplit.deserializeDataNFL(inArrayOfBuffers,nbrOfBuffers,dataBitsizePerBuffer,bitsPerCoordinate,polyNumber);
}

void FV2048_62c::serializeData64 (uint64_t* indata, unsigned char* outdata, unsigned int bitsPerChunk, uint64_t nb_of_uint64)
{
  bitsplit.serializeData64(indata,outdata,bitsPerChunk,nb_of_uint64);
}



void FV2048_62c::serializeData32 (uint32_t* indata, unsigned char* outdata, unsigned int bitsPerChunk, uint64_t nb_of_uint32){

  bitsplit.serializeData32(indata,outdata,bitsPerChunk,nb_of_uint32);

}

void FV2048_62c::setnbrbits(double nbr_bits){

    nbrbit=nbr_bits;

}


}

FV2048_62::FV2048_62c::FV2048_62c()
{
    secret_key = new FV::sk_t();
    evaluation_key = new FV::evk_t(*secret_key,32);
    public_key = new FV::pk_t(*secret_key,*evaluation_key);
    moduli = new uint64_t[FV::params::poly_p::nmoduli];
    for(unsigned short currentModulus=0;currentModulus < FV::params::poly_p::nmoduli;currentModulus++)
    {
        moduli[currentModulus] = FV::params::poly_p::get_modulus(currentModulus);
    }
    bitsplit = Bitsplit(moduli,FV::params::poly_p::nmoduli,FV::params::poly_p::degree);
    mask = (poly64) calloc(FV::params::poly_p::degree,  sizeof(uint64_t));
    poly64 mask2 = (poly64) calloc(FV::params::poly_p::degree,  sizeof(uint64_t));
    unsigned char * rnd = (unsigned char *)calloc (8*FV::params::poly_p::degree,8);
    nfl::fastrandombytes(rnd, FV::params::poly_p::degree*sizeof(uint64_t));
    mask = (poly64) rnd;
    for (int i =0;i<FV::params::poly_p::degree;i++){
        mask[i]=mask[i]&255;
        mask2[i]=mask[i]&255;
    }
    FV::params::polyP_p m_t1 = transformtopolyPp(mask);
    m_t1.invntt_pow_invphi();
    FV::params::poly_p m_t;
    for (size_t cm = 0; cm < FV::params::poly_p::nmoduli; cm++)
    {
        for (size_t i = 0; i < FV::params::poly_p::degree; i++)
        {
            m_t(cm, i) = m_t1(cm, i);
        }
    }
    m_t.ntt_pow_phi();
    for (int i =0;i<FV::params::poly_p::degree;i++){
        mask[i]=m_t(0,i);
    }

ifstream fichier("mask.txt", ios::in);
char caractere;
fichier.get(caractere);
if(fichier.eof()){
fichier.close();
ofstream fichier("mask.txt", ios::out | ios::trunc);
    for (int i =0;i<FV::params::poly_p::degree;i++){
        fichier<<mask[i]<<" ";
    }

}
fichier.close();
ifstream fichier2("mask2.txt", ios::in);
char caractere2;
fichier2.get(caractere2);
if(fichier2.eof()){
fichier2.close();
ofstream fichier2("mask2.txt", ios::out | ios::trunc);
    for (int i =0;i<FV::params::poly_p::degree;i++){
        fichier2<<mask2[i]<<" ";
    }

}
fichier2.close();
 };
