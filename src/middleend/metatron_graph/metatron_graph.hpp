#pragma once

#include <vector>
#include <memory>

#include "metatron_node.hpp"

namespace sysp::metatron {

class Graph {

public:

    Graph();

    Node* create_node(sysp::ir::Opcode op);

    const std::vector<std::unique_ptr<Node>>& nodes() const;

private:

    int next_id;

    std::vector<std::unique_ptr<Node>> node_list;

};

}
