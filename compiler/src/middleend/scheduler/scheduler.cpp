#include "scheduler.hpp"

#include <unordered_set>

namespace sysp::scheduler {

using sysp::metatron::Graph;
using sysp::metatron::Node;

Scheduler::Scheduler()
{}

static void visit(
    Node* node,
    std::unordered_set<Node*>& visited,
    std::vector<Node*>& order
)
{
    if (!node)
        return;

    if (visited.count(node))
        return;

    visited.insert(node);

    for (auto input : node->inputs)
        visit(input, visited, order);

    order.push_back(node);
}

std::vector<Node*> Scheduler::schedule(const Graph& graph)
{
    std::vector<Node*> order;
    std::unordered_set<Node*> visited;

    for (auto& n : graph.nodes()) {
        visit(n.get(), visited, order);
    }

    return order;
}

}
