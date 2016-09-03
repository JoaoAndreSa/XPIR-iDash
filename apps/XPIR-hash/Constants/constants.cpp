/**
    XPIR-hash
    constants.cpp
    Purpose: Define global constants

    @author Joao Sa
    @version 1.0 07/09/16
*/

#include <cstdint>

namespace Constants{

    //client+server
    extern const char hostname[]{"localhost"};
    extern const int port(12345);
    extern const double bandwith_limit(10000000);   //=0bps (means no bandwith limit)
    //extern const uint64_t bandwith_limit(0);
    extern const bool pipeline(true);               //=false (sequential execution);    =true (pipeline execution)
    extern const bool encrypted(true);              //=true (DB is symmetrically encrypted); =false (DB is in plaintext)
    extern const bool pre_import(true);             //=true (pre-import data to RAM); =false (import data only during querying)
    //NOTE: pre_import should be false if the number of vcf files is higher than 80-100
    extern const uint64_t num_entries(8192);        //number of entries for each vcf file in the server database
    extern const int data_hash_size(56);            //number of bits of a mutations's hash to be stored
    extern const int padding_size(736);             //number of elements for each entry (we pad with zeros until we reach this number of elements)

    //server
    extern const int n_threads(10);
    extern const int max_connects(10);
}