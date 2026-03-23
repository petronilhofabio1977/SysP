#pragma once
#include <cstdint>

struct MetaNode {

    uint32_t id;

    uint32_t inputs[4];

    uint32_t outputs[2];

    uint32_t dependency_count;

};
