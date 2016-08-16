/**
    XPIR-hash
    constants.hpp
    Purpose: Define global constants

    @author Joao Sa
    @version 1.0 01/07/16
*/

#include <cstdint>

namespace Constants{

    //client+server
    extern const char hostname[]{"127.0.0.1"};
    extern const int port(12345);
    extern const double bandwith_limit(10000000);   //=0bps (means no bandwith limit)
    //extern const uint64_t bandwith_limit(0);
    extern const bool pipeline(false);              //=false (sequential execution);    =true (pipeline execution)
    extern const bool encrypted(false);              //=true (DB is symmetrically encrypted); =false (DB is in plaintext)
    extern const bool pre_import(false);
    extern const uint64_t num_entries(8192);
    extern const int data_hash_size(56);
    extern const int padding_size(736);

    //server
    extern const int n_threads(10);
    extern const int max_connects(10);
}