#pragma once

#include <vector>
#include <memory>

#include "../ir/opcode.hpp"

namespace sysp::metatron {

struct Node {

    int id;

    sysp::ir::Opcode opcode;

    std::vector<Node*> inputs;

    Node(int node_id, sysp::ir::Opcode op)
        : id(node_id), opcode(op)
    {}

};

}
