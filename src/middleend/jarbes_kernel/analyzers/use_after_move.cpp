#include "analyzers.hpp"
#include <iostream>

std::unordered_map<uint32_t, bool> consumed_nodes;

bool check_use_after_move(const MetatronGraph& graph) {

    bool ok = true;
    for (const auto& node : graph.nodes) {
        for (auto input_id : node.inputs) {
            auto it = consumed_nodes.find(input_id);
            if (it != consumed_nodes.end() && it->second) {
                std::string name;
                auto nit = node_names.find(input_id);
                if (nit != node_names.end()) name = " ('" + nit->second + "')";
                std::cerr << "[Jarbes] Error: use-after-move — node "
                          << input_id << name << " was moved and cannot be used\n";
                ok = false;
            }
        }
    }
    if (ok) std::cout << "    [Jarbes] use-after-move: OK\n";
    return ok;
}
