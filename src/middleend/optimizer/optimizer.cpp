#include "optimizer.hpp"

#include <unordered_map>
#include <unordered_set>

namespace sysp::optimizer {

using sysp::metatron::Graph;
using sysp::metatron::Node;

Optimizer::Optimizer()
{}

void Optimizer::run(Graph& graph)
{
    constant_folding(graph);
    common_subexpression(graph);
    eliminate_dead_nodes(graph);
}

void Optimizer::constant_folding(Graph& graph)
{
    for (auto& node_ptr : graph.nodes()) {

        Node* node = node_ptr.get();

        if (node->inputs.size() != 2)
            continue;

        auto lhs = node->inputs[0];
        auto rhs = node->inputs[1];

        if (!lhs || !rhs)
            continue;

        if (lhs->opcode == sysp::ir::Opcode::ConstInt &&
            rhs->opcode == sysp::ir::Opcode::ConstInt) {

            node->opcode = sysp::ir::Opcode::ConstInt;
            node->inputs.clear();
        }
    }
}

void Optimizer::common_subexpression(Graph& graph)
{
    std::unordered_map<std::string, Node*> table;

    for (auto& node_ptr : graph.nodes()) {

        Node* node = node_ptr.get();

        std::string key = std::to_string((int)node->opcode);

        for (auto in : node->inputs)
            key += "_" + std::to_string(in->id);

        if (table.count(key)) {
            node->inputs.clear();
            node->inputs.push_back(table[key]);
        } else {
            table[key] = node;
        }
    }
}

void Optimizer::eliminate_dead_nodes(Graph& graph)
{
    std::unordered_set<Node*> used;

    for (auto& node_ptr : graph.nodes()) {

        Node* node = node_ptr.get();

        for (auto in : node->inputs)
            used.insert(in);
    }

    for (auto& node_ptr : graph.nodes()) {

        Node* node = node_ptr.get();

        if (!used.count(node) && node->inputs.empty()) {
            node->opcode = sysp::ir::Opcode::Nop;
        }
    }
}

}
