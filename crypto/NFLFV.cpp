/* Copyright (C) 2014 Carlos Aguilar Melchor, Joris Barrier, Marc-Olivier Killijian
 * This file is part of XPIR.
 *
 *  XPIR is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  XPIR is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XPIR.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "FV2048_124.hpp"
#include "NFLFV.hpp"
#include <fstream>
//#define bench
//#define Repetition 10000


lwe_cipher NFLFV::chartocipher(char* c) {

lwe_cipher ret;

	ret.a = (uint64_t*) c;
	ret.b = ret.a + polyDegree * nbModuli;

return ret;

}

void NFLlwe_DEBUG_MESSAGE(const char *s,poly64 p, unsigned int n){
#ifdef CRYPTO_DEBUG
	std::cout<<s;
	NFLlib_old::print_poly64hex(p,n);
#endif
}

// *********************************************************
// Constructors and initialization
// The constructors are not able to set all the parameters
// and setNewParameters has to be called afterward,
// the attribute alreadyInit reflects this uninitialized
// status
// *********************************************************


NFLFV::NFLFV():
    LatticesBasedCryptosystem("LWE"),
    oldNbModuli(0),
    polyDegree(0),
    fvobject(nullptr)
{
  publicParams.setcrypto_container(this);
}


// Expected format of the parameters
// k:polyDegree:modululusBitsize:AbsorptionBitsize
void NFLFV::setNewParameters(const std::string& crypto_param_descriptor)
{
  unsigned int polyDegree_, aggregatedModulusBitsize_, plainbits_;
  int abspc_bitsize = -1; // We don't know the absorption bit size yet

  std::vector<std::string> fields;
  boost::algorithm::split(fields, crypto_param_descriptor, boost::algorithm::is_any_of(":"));

  setsecurityBits(atoi(fields[1].c_str()));
  polyDegree_ = atoi(fields[2].c_str());
  aggregatedModulusBitsize_ = atoi(fields[3].c_str());
  // Does the fourth parameter exist ? If so set it
plainbits_ =atoi(fields[4].c_str());
  if (fields.size() >= 6) abspc_bitsize = atoi(fields[5].c_str());

if (fvobject != nullptr){
delete(fvobject);
fvobject = nullptr;
}

if ((polyDegree_ == 2048) && (aggregatedModulusBitsize_ == 124) && (plainbits_ == 30) && (securityBits<90)){
fvobject = new FV2048_124::FV2048_124c();
plainbits = 30;
}
else if ((polyDegree_ == 1024) && (aggregatedModulusBitsize_ == 60)&& (securityBits<76)) {
fvobject = new FV1024_62::FV1024_62c();
plainbits = 14;
}

  setNewParameters(polyDegree_,aggregatedModulusBitsize_, abspc_bitsize);
}


// The setNewParameters method does the actual parameterization of the crypto object
// it sets the alreadyInit attribute to reflects this
void  NFLFV::setNewParameters(unsigned int polyDegree_, unsigned int aggregatedModulusBitsize_, int absPCBitsize_)
{
	// Our public parameters need a pointer on us
  publicParams.setcrypto_container(this);

	// We still need to transfer this two attributes to the crypto_object
	// for the transition towards public parameter elimination
	publicParams.setAbsPCBitsize(absPCBitsize_);

	publicParams.setnoiseUB(fvobject->getnoise());

//#ifdef DEBUG
//  std::cout << "Security bits " << getsecurityBits()<<std::endl;
//  std::cout << "Noise UB " << publicParams.getnoiseUB()<<std::endl;
//#endif

  // We don't use here the polyDegree setter as we would call twice NFLlib init
	polyDegree = polyDegree_;

  nbModuli = aggregatedModulusBitsize_/62;

  //moduli= nflInstance.getmoduli();
  moduli = fvobject->getmoduli();


}

// *********************************************************
// Getters
// *********************************************************
poly64* NFLFV::getsecretKey() { return nullptr; }
unsigned int NFLFV::getpolyDegree() { return polyDegree; }

// *********************************************************
// Setters
// *********************************************************
void NFLFV::setmodulus(uint64_t modulus_)
{
	// The modulus cannot be set from outside
	std::cout << "Warning(NFLFV.c): Modulus cannot be set externally." << std::endl;
}
void NFLFV::setpolyDegree(unsigned int polyDegree_)
{
  polyDegree = polyDegree_;

}

// *********************************************************
//         Serialize/Deserialize
// *********************************************************

poly64 *NFLFV::deserializeDataNFL(unsigned char **inArrayOfBuffers, uint64_t nbrOfBuffers, uint64_t dataBitsizePerBuffer, uint64_t &polyNumber) {
  return fvobject->deserializeDataNFL(inArrayOfBuffers, nbrOfBuffers, dataBitsizePerBuffer, publicParams.getAbsorptionBitsize()/polyDegree, polyNumber);
}



// *********************************************************
//         Additions and Multiplications of ciphertexts
// *********************************************************


void NFLFV::add(lwe_cipher rop, lwe_cipher op1, lwe_cipher op2, int d)
{
  fvobject->add(rop,op1,op2,d);
}

void NFLFV::sub(lwe_cipher rop, lwe_cipher op1, lwe_cipher op2, int d)
{
 // nflInstance.submodPoly(rop.a, op1.a, op2.a);
  //nflInstance.submodPoly(rop.b, op1.b, op2.b);
}

void NFLFV::mulandadd(lwe_cipher rop, lwe_in_data op1, lwe_query op2, uint64_t current_poly, int rec_lvl)
{
	fvobject->mulandadd(rop,op1,op2,current_poly,rec_lvl);
}

// Shoup version
void NFLFV::mulandadd(lwe_cipher rop, const lwe_in_data op1, const lwe_query op2, const lwe_query op2prime, const uint64_t current_poly, int rec_lvl)
{

  fvobject->mulandadd(rop,op1,op2,current_poly,rec_lvl);
  /*
  // Don't modify the pointers inside the data or it will be permanent
  poly64 ropa = rop.a, ropb = rop.b, op2a = op2.a, op2b = op2.b, op2primea = op2prime.a,
         op2primeb = op2prime.b, op1pcurrent = op1.p[current_poly];


	 	const unsigned int K = polyDegree;
		const unsigned int md = nbModuli;
	for(unsigned short currentModulus=0;currentModulus<md;currentModulus++)
  {

 		for (unsigned i = 0; i < K; i++)
		{
			nflInstance.mulandaddShoup(ropa[i],op1pcurrent[i],op2a[i],op2primea[i],moduli[currentModulus]);
		}
 		for (unsigned i = 0; i < K; i++)
		{
			nflInstance.mulandaddShoup(ropb[i],op1pcurrent[i],op2b[i],op2primeb[i],moduli[currentModulus]);
		}
		ropa+=K;
		ropb+=K;
		op1pcurrent+=K;
		op2a+=K;
		op2b+=K;
		op2primea+=K;
		op2primeb+=K;
	}

*/
}

void NFLFV::mul(lwe_cipher rop, const lwe_in_data op1, const lwe_query op2, const lwe_query op2prime, const uint64_t current_poly, int rec_lvl)
{

}

// Same comment as for musAndAddCiphertextNTT we do a simpler version above
void NFLFV::mulandadd(lwe_cipher rop, lwe_in_data op1, lwe_query op2, int rec_lvl)
{
/*
	NFLlwe_DEBUG_MESSAGE("in_data p: ",op1.p[0],4);
	NFLlwe_DEBUG_MESSAGE("in_data a: ",op2.a,4);
	NFLlwe_DEBUG_MESSAGE("in_data b: ",op2.b,4);

  	mulandaddCiphertextNTT(rop, op1, op2);

	NFLlwe_DEBUG_MESSAGE("out_data.a : ",rop.a,4);
	NFLlwe_DEBUG_MESSAGE("out_data.b : ",rop.b,4);
	*/
}

// Deal just with one polynomial
inline void NFLFV::mulandaddCiphertextNTT(lwe_cipher rop, lwe_in_data op1, lwe_query op2, uint64_t current_poly)
{
   // nflInstance.mulandaddPolyNTT(rop.a, op1.p[current_poly], op2.a);
   // nflInstance.mulandaddPolyNTT(rop.b, op1.p[current_poly], op2.b);
}

// Good method but too greedy in memory we start with a simpler one (below)
// Needs to change as we always write in the same rop
void NFLFV::mulandaddCiphertextNTT(lwe_cipher rop, lwe_in_data op1, lwe_query op2)
{
  for(uint64_t i=0;i<op1.nbPolys;i++)
  {
   // nflInstance.mulandaddPolyNTT(rop.a, op1.p[i], op2.a);
   // nflInstance.mulandaddPolyNTT(rop.b, op1.p[i], op2.b);
  }
}



//*********************************
// Encryption and decryption
//*********************************

// The internal encrypt method
void  NFLFV::enc(lwe_cipher *c, poly64 m)
{
}


void NFLFV::dec(poly64 m, lwe_cipher *c)
{

}

// MOK is here for the CRT modification

// encrypts a uint (e.g. for producing a equest element with a 0 or a 1)
// does not return a lwe_cipher but the (char*)pointer on two consecutively allocated poly64 (a and b)
char* NFLFV::encrypt(unsigned int ui, unsigned int d)
{

	if ( ceil(log2(static_cast<double>(ui))) >= publicParams.getAbsorptionBitsize())
	{
		std::cerr << "NFFLWE: The given unsigned int does not fit in " << publicParams.getAbsorptionBitsize() << " bits"<< std::endl;
		ui %= 1<<publicParams.getAbsorptionBitsize();
	}

	lwe_cipher c;
	poly64 m = (poly64)calloc(nbModuli*polyDegree,sizeof(uint64_t));
	for (unsigned int cm = 0 ; cm < nbModuli ; cm++)
  {
    m[cm*polyDegree]=(uint64_t)ui;
  }

	fvobject->enc(&c,m);
	free(m);
	return (char*) c.a;
}

char* NFLFV::encrypt(char* data, size_t s_hash, unsigned int s_list ){
    lwe_cipher c;
	poly64 m = (poly64)calloc(nbModuli*polyDegree,sizeof(uint64_t));
	uint64_t *ui;
	for (int j=0; j<s_list;j++){
        for (int i = 0; i<s_hash;i++){
            if (i==0) {ui[j]=uint64_t(data[i+j*s_hash]);}
            ui[j] = (ui[j]<<8) | data[i+j*s_hash];
        }
        for (unsigned int cm = 0 ; cm < nbModuli ; cm++){
            m[cm*polyDegree+j]=ui[j];
        }
    }

  	fvobject->enc(&c,m);
	free(m);
	return (char*) c.a;
}


// Do a ciphertext for a plaintext with alternating bits (for performance tests)
char* NFLFV::encrypt_perftest()
{
	lwe_cipher c;
  poly64 m = (poly64)calloc(nbModuli*polyDegree,sizeof(uint64_t));
	fvobject->enc(&c,m);
	free(m);
	return (char*) c.a;
}

char* NFLFV::decrypt(char* cipheredData, unsigned int rec_lvl, size_t, size_t)
{
  lwe_cipher ciphertext;
  ciphertext.a = (poly64)cipheredData;
  ciphertext.b = ciphertext.a + nbModuli * polyDegree;
  poly64 clear_data = (poly64) calloc(nbModuli * polyDegree, sizeof(uint64_t));
  unsigned int bits_per_coordinate = publicParams.getAbsorptionBitsize()/polyDegree;

#ifdef DEBUG
  std::cout<<"Allocated (bytes): "<<nbModuli * polyDegree * sizeof(uint64_t)<<std::endl;
  std::cout<<"Bits per coordinate: "<<bits_per_coordinate<<std::endl;
#endif

  fvobject->dec(clear_data, &ciphertext);
  //std::cout<<clear_data[0]<<std::endl;

  NFLlwe_DEBUG_MESSAGE("Decrypting ciphertext a: ",ciphertext.a, 4);
  NFLlwe_DEBUG_MESSAGE("Decrypting ciphertext b: ",ciphertext.b, 4);
  NFLlwe_DEBUG_MESSAGE("Result: ",clear_data, 4);

  // unsigned char* out_data = (unsigned char*) calloc(nbModuli * polyDegree+1, sizeof(uint64_t));
  // nflInstance.serializeData64 (clear_data, out_data, bits_per_coordinate, polyDegree);

  unsigned char* out_data = (unsigned char*) calloc(bits_per_coordinate*polyDegree/64 + 1, sizeof(uint64_t));
 // if (nbModuli == 2)
  //{
   //fvobject->serializeData64(clear_data, out_data, bits_per_coordinate, ceil((double)bits_per_coordinate/64)* polyDegree);
 // }
 // else // nbModuli > 1
 // {
    fvobject->serializeData32 ((uint32_t*)clear_data, out_data, bits_per_coordinate, ceil((double)bits_per_coordinate/32)* polyDegree);
 // }
#ifdef DEBUG
  //std::cout<<"Bitgrouped into: "<<out_data<<std::endl;
#endif
  free(clear_data);
  return (char*) out_data;
}


unsigned int NFLFV::getAllCryptoParams(std::set<std::string>& crypto_params)
{
  unsigned int params_nbr  = 0;
  unsigned int k_array_size = 5;
  unsigned int k[5] = {80, 100, 128, 192, 256};

  for (unsigned int i = 0 ; i < k_array_size ; i++)
  {
    params_nbr += getCryptoParams(k[i], crypto_params);
  }

  return params_nbr;
}


unsigned int NFLFV::getCryptoParams(unsigned int k, std::set<std::string>& crypto_params)
{
  using namespace std;
  unsigned int p_size, params_nbr = 0;
  string k_str  = to_string(k);

  for (unsigned int degree = kMinPolyDegree ; degree <= kMaxPolyDegree; degree <<= 1)
  {
    string param;
    p_size = findMaxModulusBitsize(k, degree);

    // We give a very small margin 59 instead of 60 so that 100:1024:60 passes the test
    //for (unsigned int i = 1; i * 59 <= p_size ; i++)//(p_size > 64) && ((p_size % 64) != 0))
    for (unsigned int i = 1; i * 61 <= p_size && i * 62 <= 248; i++)
    {
      param =  cryptoName + ":" + to_string(estimateSecurity(degree,i*62)) + ":" + to_string(degree) + ":" + to_string(i*62) ;
      if (crypto_params.insert(param).second) params_nbr++;
      param = "";
    }
  }

  return params_nbr;
}

void NFLFV::recomputeNoiseAmplifiers() {

}

unsigned int NFLFV::estimateSecurity(unsigned int n, unsigned int p_size)
{
  unsigned int estimated_k = 5;//Estimate K can not be too low

  while(!checkParamsSecure(estimated_k,n,p_size)) estimated_k++;

  return --estimated_k;
}


long NFLFV::setandgetAbsBitPerCiphertext(unsigned int elt_nbr)
{
    double Berr = static_cast<double>(publicParams.getnoiseUB());
    double nb_sum = elt_nbr;
    double p_size = getmodulusBitsize();
    double avail_bits = floor(( (p_size - 1) - log2(nb_sum) - log2(Berr) 
          -log2(static_cast<double>(polyDegree))) / 2.0);
    unsigned int nbr_bits = avail_bits;

    // The amount of absorbed bits cannot go beyond plaintext space ...
    if(nbr_bits > plainbits -1)
    {
      nbr_bits = plainbits - 1;
    }
    // ... nor be lesser than 0
    if(nbr_bits < 0)
    {
      nbr_bits = 0;
    }

    fvobject->setnbrbits(nbr_bits);
    publicParams.setAbsPCBitsize(nbr_bits);

    return long(nbr_bits);
}


unsigned int NFLFV::findMaxModulusBitsize(unsigned int k, unsigned int n)
{
  unsigned int p_size;
  //p_size can not be too low
  p_size = 10;
  while (!checkParamsSecure(k,n,p_size)) p_size++;

  return --p_size;
}


bool NFLFV::checkParamsSecure(unsigned int k, unsigned int n, unsigned int p_size)
{
  double p, beta, logBerr = 8, epsi, lll;

  //We take an advantage of 2**(-k/2) and an attack time of 2**(k/2)
  epsi = pow(2, -static_cast<double>(k/2));
  //log(time) = 1.8/ log(delta) − 110 and -80 to compute processor cycles so we take pow(2, k/2) = 1.8/log(delta) - 80
  double delta = pow(2,1.8/(k/2 + 80));

  p    = pow(2, p_size) -  1;
  beta = (p / logBerr) * sqrt(log1p( 1 / epsi) / M_PI);
  lll  = lllOutput(n, p, delta);

  // We love ugly tricks !
  return (lll < beta);// && cout << "beta : " << beta << " p_size : " << p_size << " n :"<< n << " k : "<< k << endl;
}


double NFLFV::lllOutput(unsigned int n, double& p, double delta)
{
  double m = 2*n + 128;

  //execution log(time) = 1.8/ log(delta) − 110 and -80 to compute processor cycles. We add a margin of 20 so we take k/2 = 1.8/log(delta) - 100
  double lll1 = pow(delta, m) * pow(p, n/m);

  double lll2 = 2 * sqrt(n * log2(p) * log2(delta));
  lll2 = pow(2, lll2);

  return std::min(lll1, lll2);
}

double NFLFV::estimateAbsTime(std::string crypto_param)
{
  using namespace std;
  vector<string> fields;
  boost::algorithm::split(fields, crypto_param, boost::algorithm::is_any_of(":"));
  unsigned int p_size = (unsigned) atoi(fields[3].c_str());
  double a = (p_size < 64) ? 1 : ceil(static_cast<double>(p_size)/64.0);
  unsigned int degree = (unsigned) atoi(fields[2].c_str());
  double b = degree/1024;

  return 1/(1.75 * pow(10, 5)/(a*b));
}

double NFLFV::estimatePrecomputeTime(std::string crypto_param)
{
  using namespace std;
  vector<string> fields;
  boost::algorithm::split(fields, crypto_param, boost::algorithm::is_any_of(":"));
  unsigned int p_size = (unsigned) atoi(fields[3].c_str());
  double a = (p_size < 64) ? 1 : ceil(static_cast<double>(p_size)/64.0);
  unsigned int degree = (unsigned) atoi(fields[2].c_str());
  double b = degree/1024;

  return 1/(0.75*pow(10, 5)/(a*b));
}

unsigned int NFLFV::getmodulusBitsize() {
	return nbModuli*62;
}

// *********************************************************
// AbstractPublicParameters stuff
// *********************************************************
AbstractPublicParameters& NFLFV::getPublicParameters()
{
	//This was bug chasing but should not be necessary!
	publicParams.setcrypto_container(this);
  	return publicParams;
}

std::string NFLFV::getSerializedCryptoParams(bool shortversion)
{
  return publicParams.getSerializedParams(shortversion);
}


NFLFV::~NFLFV()
{
  clearSecretKeys();
  delete fvobject;
}


std::string& NFLFV::toString()
{
  return cryptoName;
}

void NFLFV::clearSecretKeys()
{

}


//This main is for benchmarking and tests
//
// int main(int c,char **v) {
//
// // Benchs et correctness enc/dec
// 	 	NFLFV n;
// 		n.setNewParameters(1024,64,22);
//  		n.setmodulus(P64);
//  		n.getPublicParameters().computeNewParameters("lwe:80:1024:64:22");
//
//  		poly64 p=n.boundedRandomPoly(1024, 1023);
//  		poly64 result=(poly64)calloc(1024,sizeof(uint64_t));
//
//  	 	std::cout<<"0-RND polynom: ";n.print_poly64(p,4);std::cout<<std::endl;
//
//
//  		lwe_cipher cyph;
//  #ifdef bench
//  		double start     = omp_get_wtime();
//  		for(int i        = 0;i<Repetition;i++) {
//  #endif
//  			n.enc(&cyph,p);
//  #ifdef bench
//
//  			}
//  		double end        = omp_get_wtime();
//  		std::cout<<Repetition/(end-start)<<" chiffre/s"<<std::endl;
//
//
//  		 start     = omp_get_wtime();
//  		for(int i        = 0;i<Repetition;i++) {
//  #endif
//
//  			n.dec(result,&cyph);
//  #ifdef bench
//  		}
//  		 end        = omp_get_wtime();
//  		std::cout<<Repetition/(end-start)<<" dechiffre/s"<<std::endl;
//  #endif
// 		NFLlwe_DEBUG_MESSAGE("Encrypted into a",cyph.a,4);
// 		NFLlwe_DEBUG_MESSAGE("Encrypted into b",cyph.b,4);
//  	 	NFLlwe_DEBUG_MESSAGE("1-Encoded-Decoded (but not unshoupified): ",result,4);
//
//
//  		for(int i = 0;i<1024;i++) {
//  		  if((result[i]%P64)!=(result[i]%P64)) {
//  			std::cout<<"err "<<(p[i])<<" != "<<(result[i]%P64)<<std::endl;
//       	  	exit(1);
//  			break;
//  		  }
//  		}
//
// 	std::cout<< "enc/dec test passed"<<std::endl;
//
//
//
// 	// int bytesize=1024*22/8+1;
// 	//  	char* mydata=(char*)calloc(bytesize,1);
// 	//  	for(int i=0;i<bytesize;i++) {
// 	//  		mydata[i]='A'+(i%('Z'-'A'));
// 	//  	}
// 	//  	std::cout<<"Initial data : "<<mydata<<std::endl;
// 	//
// 	//  	uint64_t bitsize=bytesize*8;
// 	//  	uint64_t nbOfPolys;
// 	//  	Warning, need to tranform this line with the new version of deserializeDataNTT which takes 4 parameters instead of 3 poly64 *mydata_poly=n.deserializeDataNTT((unsigned char*)mydata,bitsize,nbOfPolys);
// 	//
// 	//  	std::cout<<"The string has been encoded into ";n.print_poly64hex(*mydata_poly,1024*nbOfPolys);
// 	//   	std::cout<<std::endl<<"nbOfPolys = "<<nbOfPolys<<std::endl;
// 	//
// 	//  	// encrypt of poly64 simulation
// 	//  	lwe_cipher *cyphertext=new lwe_cipher[nbOfPolys];
// 	// for(int i=0;i<nbOfPolys;i++) {
// 	//  		n.enc(&(cyphertext[i]),*(mydata_poly + i*1024));
// 	//  	//std::cout<<"The string has been cyphered into a["<<i<<"]";n.print_poly64(cyphertext[i].a,1024);
// 	//  	//std::cout<<"The string has been cyphered into b["<<i<<"]";n.print_poly64(cyphertext[i].a,1024);
// 	// }
// 	//
// 	//  	poly64 unciphereddata_poly[nbOfPolys];// = (poly64*)calloc(1024*nbOfPolys,sizeof(uint64_t));
// 	// for(int i=0;i<nbOfPolys;i++) {
// 	// 	unciphereddata_poly[i]=(poly64)n.decrypt((char*)(cyphertext[i].a), (unsigned int)0,(size_t) 0,(size_t) 0);
// 	//  	std::cout<<"Decoded into polynom: ";n.print_poly64hex((poly64)unciphereddata_poly[i],128);std::cout<<std::endl;
// 	// }
// 	//
// 	//
// 	//  	unsigned char* unciphereddata=n.serializeData(unciphereddata_poly[0], nbOfPolys,bitsize, true);
// 	//
// 	//  	std::cout<<"Decoded into "<<std::hex<<unciphereddata<<std::endl;
// 	//  	std::cout<<"A= "<<std::dec<<(short)'A'<<std::endl<<std::endl;
// 	//  	std::cout<<"G= "<<std::dec<<(short)'G'<<std::endl<<std::endl;
// 	//
// 	//
//  	//
//  	// //lwe_cipher *cypherui;
//  	// //cypherui=(lwe_cipher *)n.encrypt(1,0);
//  	// //unciphereddata_poly = (poly64)n.decrypt((char*)(cypherui->a), (unsigned int)0,(size_t) 0,(size_t) 0);
//  	// char *charptr;
//  	// charptr=n.encrypt(1,0);
//  	// unciphereddata_poly = (poly64)n.decrypt(charptr, (unsigned int)0,(size_t) 0,(size_t) 0);
//  	// std::cout<<"1-Decoded into polynom: ";n.print_poly64(unciphereddata_poly,1024);std::cout<<std::endl;
//  	// 	std::cout<<"Decoded into "<<std::hex<<unciphereddata_poly<<std::endl;
//  }
