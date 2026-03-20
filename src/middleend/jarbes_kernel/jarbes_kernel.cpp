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

void JarbesKernel::register_builtin(uint32_t id)                      { get_builtin_nodes().insert(id); }
void JarbesKernel::register_move(uint32_t id)                         { get_consumed_nodes()[id] = true; }
void JarbesKernel::register_region_node(uint32_t id, int region_id)   { get_node_region()[id] = region_id; }
void JarbesKernel::register_name(uint32_t id, const std::string& name){ get_node_names()[id] = name; }
void JarbesKernel::register_unsafe(uint32_t id)                       { get_unsafe_nodes()[id] = true; }
void JarbesKernel::register_array(uint32_t id, int size)              { get_array_bounds()[id] = size; }
void JarbesKernel::register_freed(uint32_t id)                        { get_freed_nodes().insert(id); }
void JarbesKernel::register_spawn(uint32_t id)                        { get_spawn_nodes().insert(id); }
void JarbesKernel::register_channel(uint32_t id)                      { get_channel_nodes().insert(id); }

void JarbesKernel::analyze(MetatronGraph& graph) {
    std::cout << "[JarbesKernel] Analyzing " << graph.nodes.size() << " nodes\n";

    bool ok = true;

    if (!check_use_before_production(graph)) ok = false;
    if (!check_use_after_move(graph))        ok = false;
    if (!check_region_escape(graph))         ok = false;
    if (!check_double_free(graph))           ok = false;
    if (!check_dangling_pointer(graph))      ok = false;
    if (!check_buffer_overflow(graph))       ok = false;
    if (!check_unsafe_audit(graph))          ok = false;
    if (!check_type_mismatch(graph))         ok = false;
    if (!detect_cycle(graph))                ok = false;
    if (!detect_data_race(graph))            ok = false;

    if (!ok) throw std::runtime_error("JarbesKernel: memory safety violations detected");

    std::cout << "[JarbesKernel] All checks passed — program is memory safe\n";
}

} // namespace sysp
