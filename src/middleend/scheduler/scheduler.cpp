#include "scheduler.hpp"

std::vector<NodeId> Scheduler::schedule(const MetatronGraph& graph) {

    std::vector<NodeId> order;

    const auto& nodes = graph.get_nodes();

    for(const auto& n : nodes)
        order.push_back(n.id);

    return order;
}
