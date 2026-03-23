#pragma once
#include <vector>
#include <cstdint>

struct MetatronNode {

    uint32_t id;

    std::vector<uint32_t> inputs;
    std::vector<uint32_t> outputs;

};
