/*
D. J. Bernstein
Public domain.
*/
#ifndef CRYPTO_STREAM_H_old
#define CRYPTO_STREAM_H_old

#define crypto_stream_salsa20_KEYBYTES 32
#define crypto_stream_salsa20_NONCEBYTES 8

#ifdef __cplusplus
#include <string>
extern "C" {
#endif
extern
int crypto_stream_salsa20_amd64_xmm6_old(
        unsigned char *c,unsigned long long clen,
  const unsigned char *n,
  const unsigned char *k
);
    
#ifdef __cplusplus
}
#endif
#define crypto_stream_salsa20_old crypto_stream_salsa20_amd64_xmm6_old

#endif
