#include "optimizer.hpp"
#include <iostream>
#include <unordered_set>

namespace sysp::optimizer {

Optimizer::Optimizer() {}

int Optimizer::run(MetatronGraph& graph) {
    int total = 0;
    total += constant_folding(graph);
    total += eliminate_dead_nodes(graph);
    if (total > 0) std::cout << "    [Optimizer] " << total << " optimizations applied\n";
    else           std::cout << "    [Optimizer] No optimizations needed\n";
    return total;
}

int Optimizer::eliminate_dead_nodes(MetatronGraph& graph) {
    std::unordered_set<uint32_t> used;
    for (const auto& node : graph.nodes)
        for (auto input_id : node.inputs)
            used.insert(input_id);
    int removed = 0;
    auto it = graph.nodes.begin();
    while (it != graph.nodes.end()) {
        if (!used.count(it->id) && it->outputs.empty() && it->inputs.empty()) {
            it = graph.nodes.erase(it);
            removed++;
        } else { ++it; }
    }
    return removed;
}

int Optimizer::constant_folding(MetatronGraph& graph) {
    (void)graph;
    return 0;
}

} // namespace sysp::optimizer
