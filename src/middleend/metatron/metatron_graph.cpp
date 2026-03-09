#include "metatron_graph.hpp"

MetatronGraph::MetatronGraph(){}

MetatronNode* MetatronGraph::create_node(const std::string& label)
{

    auto node = std::make_unique<MetatronNode>();

    node->id = next_id++;
    node->label = label;

    node_list.push_back(std::move(node));

    return node_list.back().get();

}

void MetatronGraph::connect(MetatronNode* a, MetatronNode* b)
{

    a->outputs.push_back(b);
    b->inputs.push_back(a);

}

const std::vector<std::unique_ptr<MetatronNode>>& MetatronGraph::nodes() const
{
    return node_list;
}
