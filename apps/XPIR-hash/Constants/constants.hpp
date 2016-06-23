#pragma once

namespace Constants{

    //client+server
    extern const char hostname[];
    extern const int port;
    extern const uint64_t bandwith_limit; //=0 (means no bandwith limit)
    extern const bool parallel; 
    
    //server
    extern const int n_threads;
    extern const int max_connects;

}