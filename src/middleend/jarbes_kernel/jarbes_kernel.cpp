#include "jarbes_kernel.hpp"
#include "analyzers/analyzers.hpp"
#include <iostream>
#include <stdexcept>

namespace sysp {

void JarbesKernel::build_metatron(double version) {
    std::cout << "[JarbesKernel] Metatron initialized version " << (int)version << "\n";
}
void JarbesKernel::build_honeycomb(int layers) {
    std::cout << "[JarbesKernel] Honeycomb grid with " << layers << " layers created\n";
}
void JarbesKernel::distribute_transistors() {
    std::cout << "[JarbesKernel] Distributing logical transistors\n";
}
void JarbesKernel::step() {
    std::cout << "[JarbesKernel] Kernel logical step executed\n";
}
void JarbesKernel::register_builtin(uint32_t id)                      { builtin_nodes.insert(id); }
void JarbesKernel::register_move(uint32_t id)                         { consumed_nodes[id] = true; }
void JarbesKernel::register_region_node(uint32_t id, int region_id)   { node_region[id] = region_id; }
void JarbesKernel::register_name(uint32_t id, const std::string& name){ node_names[id] = name; }

void JarbesKernel::analyze(MetatronGraph& graph) {
    std::cout << "[JarbesKernel] Analyzing " << graph.nodes.size() << " nodes\n";
    bool ok = true;
    if (!check_use_before_production(graph)) ok = false;
    if (!check_use_after_move(graph))        ok = false;
    if (!check_region_escape(graph))         ok = false;
    if (!detect_cycle(graph))                ok = false;
    if (!detect_data_race(graph))            ok = false;
    if (!ok) throw std::runtime_error("JarbesKernel: memory safety violations detected");
    std::cout << "[JarbesKernel] All checks passed — program is memory safe\n";
}

} // namespace sysp
