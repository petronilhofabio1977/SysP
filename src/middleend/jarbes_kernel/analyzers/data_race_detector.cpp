#include "analyzers.hpp"
#include <iostream>
#include <unordered_set>

std::unordered_set<uint32_t> spawn_nodes;
std::unordered_set<uint32_t> channel_nodes;

// Data race detection:
// A data race happens when two concurrent tasks (spawn)
// access the same node without going through a channel.
//
// Safe patterns:
//   - Task reads/writes only its own local nodes
//   - Communication happens only via channels
//
// Unsafe pattern:
//   - Two spawn nodes share an input node
//   - Without a channel node between them

bool detect_data_race(const MetatronGraph& graph) {
    bool ok = true;

    if (spawn_nodes.empty()) {
        std::cout << "    [Jarbes] data-race: OK (no concurrent tasks)\n";
        return true;
    }

    // Build map: node_id → list of spawn tasks that use it
    std::unordered_map<uint32_t, std::vector<uint32_t>> shared;

    for (auto spawn_id : spawn_nodes) {
        // Find the spawn node
        for (const auto& node : graph.nodes) {
            if (node.id == spawn_id) {
                for (auto input_id : node.inputs) {
                    // Skip channel nodes — they are safe
                    if (channel_nodes.count(input_id)) continue;
                    // Skip built-ins
                    if (builtin_nodes.count(input_id)) continue;
                    shared[input_id].push_back(spawn_id);
                }
            }
        }
    }

    // Check for shared nodes accessed by multiple tasks
    for (auto& kv : shared) {
        if (kv.second.size() > 1) {
            std::string name;
            auto nit = node_names.find(kv.first);
            if (nit != node_names.end()) name = " ('" + nit->second + "')";
            std::cerr << "[Jarbes] Error: data-race — node "
                      << kv.first << name
                      << " accessed by " << kv.second.size()
                      << " concurrent tasks without channel\n";
            ok = false;
        }
    }

    if (ok) std::cout << "    [Jarbes] data-race: OK\n";
    return ok;
}
