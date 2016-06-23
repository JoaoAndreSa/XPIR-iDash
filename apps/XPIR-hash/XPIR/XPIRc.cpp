#include "XPIRc.hpp"

uint64_t XPIRc::getAlpha(){
	return m_params.alpha;
}

uint64_t XPIRc::getD(){
	return m_params.d;
}

unsigned int* XPIRc::getN(){
	return m_params.n;
}

uint64_t XPIRc::getMaxSize(){
	return m_maxFileSize;
}

HomomorphicCrypto* XPIRc::getCrypto(){
	return m_crypto;
}

uint32_t XPIRc::getQsize(uint64_t d){
	return (uint32_t) m_crypto->getPublicParameters().getQuerySizeFromRecLvl(d)/GlobalConstant::kBitsPerByte;
}

uint32_t XPIRc::getRsize(){
	return (uint32_t) m_crypto->getPublicParameters().getCiphBitsizeFromRecLvl(m_params.d)/GlobalConstant::kBitsPerByte;
}