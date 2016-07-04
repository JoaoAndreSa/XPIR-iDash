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
    extern const char hostname[]{"localhost"};
    extern const int port(12346);
    extern const uint64_t bandwith_limit(10000000); //=0bps (means no bandwith limit)
    extern const bool pipeline(true);
    extern const bool encrypted(false);             //=true (DB is symmetrically encrypted); =false (DB is in plaintext)

    //server
    extern const int n_threads(10);
    extern const int max_connects(10);
}