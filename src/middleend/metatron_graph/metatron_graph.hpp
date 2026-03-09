#pragma once

#include "metatron_node.hpp"
#include <vector>

class MetatronGraph {

public:

    NodeId add_node(NodeKind kind, const std::vector<NodeId>& inputs);

    const std::vector<Node>& get_nodes() const;

private:

    std::vector<Node> nodes;
};
