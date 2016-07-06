/**
    XPIR-hash
    constants.cpp
    Purpose: Define global constants

    @author Joao Sa
    @version 1.0 01/07/16
*/

#pragma once

namespace Constants{

    //client+server
    extern const char hostname[];
    extern const int port;
    extern const double bandwith_limit; //=0bps (means no bandwith limit)
    extern const bool pipeline;
    extern const bool encrypted; 

    //server
    extern const int n_threads;
    extern const int max_connects;

}