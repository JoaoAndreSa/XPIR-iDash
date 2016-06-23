#include <cstdint>

namespace Constants{

    //client+server
    extern const char hostname[]{"localhost"};
    extern const int port(12345);
    extern const uint64_t bandwith_limit(10000000);
    extern const bool parallel(true);

    //server
    extern const int n_threads(10);
    extern const int max_connects(5);
}