#pragma once

#include <vector>
#include <memory>

#include "../metatron_graph/metatron_node.hpp"

namespace sysp::cfg {

struct BasicBlock {

    int id;

    std::vector<sysp::metatron::Node*> instructions;

    std::vector<BasicBlock*> successors;

    std::vector<BasicBlock*> predecessors;

    BasicBlock(int block_id)
        : id(block_id)
    {
    }

};

}
