#include "analyzers.hpp"
#include <iostream>
#include <unordered_map>

// Double-free: rastreia quantas vezes cada nó foi liberado
// Um nó é liberado quando: drop explícito ou região fecha
// freed_count > 1 = double-free

bool check_double_free(const MetatronGraph& graph) {
    bool ok = true;

    // Conta quantas vezes cada nó aparece em freed_nodes
    // Como freed_nodes é um set, precisamos rastrear via drop_count
    std::unordered_map<uint32_t, int> drop_count;

    // Percorre o grafo procurando padrões de drop duplo
    // Um drop duplo acontece quando um nó consumido (moved) também está em freed
    for (const auto& node : graph.nodes) {
        // Se o nó está em freed_nodes E está consumed — potencial double-free
        if (freed_nodes.count(node.id) && consumed_nodes.count(node.id)) {
            drop_count[node.id]++;
        }

        // Verifica inputs: se um input foi freed e o nó atual também o libera
        for (auto input_id : node.inputs) {
            if (freed_nodes.count(input_id)) {
                drop_count[input_id]++;
            }
        }
    }

    // Reporta double-frees
    for (auto& [node_id, count] : drop_count) {
        if (count > 1) {
            std::string name;
            auto nit = node_names.find(node_id);
            if (nit != node_names.end()) name = " ('" + nit->second + "')";
            std::cerr << "[Jarbes] Error: double-free — node "
                      << node_id << name << " freed " << count << " times\n";
            ok = false;
        }
    }

    if (ok) std::cout << "    [Jarbes] double-free: OK\n";
    return ok;
}
