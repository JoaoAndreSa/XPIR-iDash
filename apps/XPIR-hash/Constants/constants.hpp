 /**
    XPIR-hash
    constants.hpp
    Purpose: Define global constants

    @author Joao Sa
    @version 1.0 09/07/16
*/

#pragma once

namespace Constants{

    //client+server
    extern const char hostname[];
    extern const int port;
    extern const double bandwith_limit; //=0bps (means no bandwith limit)
    extern const bool pipeline;         //=false (sequential execution);         =true (pipeline execution)
    extern const bool encrypted;        //=true (DB is symmetrically encrypted); =false (DB is in plaintext)
    extern const bool pre_import;       //=true (pre-import data to RAM); =false (import data only during querying)
    //NOTE: pre_import should be false if the number of vcf files is higher than 80-100
    extern const uint64_t num_entries;  //number of entries for each vcf file in the server database
    extern const int data_hash_size;    //number of bits of a mutations's hash to be stored
    extern const int padding_size;      //number of elements for each entry (we pad with zeros until we reach this number of elements)
    extern const int abs_bits;          //to generate x number of noise bits   

    //server
    extern const int n_threads;
    extern const int max_connects;

}