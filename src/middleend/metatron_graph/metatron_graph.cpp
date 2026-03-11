#include "metatron_graph.hpp"

namespace sysp::metatron {

Graph::Graph()
    : next_id(0)
{}

Node* Graph::create_node(sysp::ir::Opcode op)
{
    auto node = std::make_unique<Node>(next_id++, op);
    Node* ptr = node.get();

    node_list.push_back(std::move(node));

    return ptr;
}

const std::vector<std::unique_ptr<Node>>& Graph::nodes() const
{
    return node_list;
}

}
