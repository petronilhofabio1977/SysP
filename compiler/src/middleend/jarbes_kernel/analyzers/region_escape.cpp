#include "analyzers.hpp"
#include <iostream>

bool check_region_escape(const MetatronGraph& graph) {
    bool ok = true;
    for (const auto& node : graph.nodes) {
        int consumer_region = -1;
        auto cr = node_region.find(node.id);
        if (cr != node_region.end()) consumer_region = cr->second;
        for (auto input_id : node.inputs) {
            int producer_region = -1;
            auto pr = node_region.find(input_id);
            if (pr != node_region.end()) producer_region = pr->second;
            if (producer_region >= 0 && consumer_region != producer_region) {
                std::string name;
                auto nit = node_names.find(input_id);
                if (nit != node_names.end()) name = " ('" + nit->second + "')";
                std::cerr << "[Jarbes] Error: region-escape — node "
                          << input_id << name
                          << " from region " << producer_region
                          << " escapes to region " << consumer_region << "\n";
                ok = false;
            }
        }
    }
    if (ok) std::cout << "    [Jarbes] region-escape: OK\n";
    return ok;
}
