cmake_minimum_required(VERSION 2.6.0)

set(CMAKE_CXX_FLAGS "-std=c++11")

add_library(pir_crypto_prng STATIC crypto_stream_salsa20_amd64_xmm6_old.s fastrandombytes_old.cpp randombytes_old.cpp)
set_property(TARGET pir_crypto_prng PROPERTY COMPILE_DEFINITIONS SHARED_C)
