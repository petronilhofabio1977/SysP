#pragma once
#include <cstdint>

struct CacheLine {

    uint64_t data[8];

};

struct LCache {

    CacheLine lines[256];

};
