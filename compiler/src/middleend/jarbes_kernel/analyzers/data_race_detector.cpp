#include "analyzers.hpp"
#include <iostream>

std::unordered_set<uint32_t> spawn_nodes;
std::unordered_set<uint32_t> channel_nodes;

bool detect_data_race(const MetatronGraph& graph) {
    bool ok = true;

    if (spawn_nodes.empty()) {
        std::cout << "    [Jarbes] data-race: OK (no concurrent tasks)\n";
        return true;
    }

    // Build map: shared_node_id → list of spawn tasks that access it
    std::unordered_map<uint32_t, std::vector<uint32_t>> shared_access;

    for (auto spawn_id : spawn_nodes) {
        for (const auto& node : graph.nodes) {
            if (node.id != spawn_id) continue;
            for (auto input_id : node.inputs) {
                if (channel_nodes.count(input_id)) continue; // channel = safe
                if (builtin_nodes.count(input_id))  continue; // builtin = safe
                shared_access[input_id].push_back(spawn_id);
            }
        }
    }

    // Also check: if two different spawn nodes share ANY common ancestor
    // that is not a channel — potential race
    for (auto& [shared_id, tasks] : shared_access) {
        // Sort and deduplicate task list
        std::sort(tasks.begin(), tasks.end());
        tasks.erase(std::unique(tasks.begin(), tasks.end()), tasks.end());

        if (tasks.size() > 1) {
            std::string name;
            auto nit = node_names.find(shared_id);
            if (nit != node_names.end()) name = " ('" + nit->second + "')";
            std::cerr << "[Jarbes] Error: data-race — node "
                      << shared_id << name
                      << " accessed by " << tasks.size()
                      << " concurrent tasks without synchronization\n";
            std::cerr << "    Hint: use channels to safely share data between tasks\n";
            ok = false;
        }
    }

    if (ok) std::cout << "    [Jarbes] data-race: OK\n";
    return ok;
}
