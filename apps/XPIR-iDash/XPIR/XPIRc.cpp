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