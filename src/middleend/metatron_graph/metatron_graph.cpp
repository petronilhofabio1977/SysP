#include "metatron_graph.hpp"

NodeId MetatronGraph::add_node(NodeKind kind, const std::vector<NodeId>& inputs) {

    NodeId id = nodes.size();

    Node node;

    node.id = id;
    node.kind = kind;
    node.inputs = inputs;

    nodes.push_back(node);

    return id;
}

const std::vector<Node>& MetatronGraph::get_nodes() const {
    return nodes;
}
