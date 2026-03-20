#pragma once
#include "core/metatron_graph.hpp"
#include "frontend/ast/decl.hpp"
#include <cstdint>
#include <string>

namespace sysp {

class JarbesKernel {
public:
    void build_metatron(double version);
    void build_honeycomb(int layers);
    void distribute_transistors();
    void step();

    // Main analysis — runs all checkers
    void analyze(MetatronGraph& graph, const sysp::ast::Program& program);

    // Legacy interface (kept for compatibility)
    void analyze(MetatronGraph& graph);

    // Registration methods
    void register_builtin(uint32_t node_id);
    void register_move(uint32_t source_node_id);
    void register_region_node(uint32_t node_id, int region_id);
    void register_name(uint32_t node_id, const std::string& name);
    void register_unsafe(uint32_t node_id);
    void register_array(uint32_t node_id, int size);
    void register_freed(uint32_t node_id);
    void register_spawn(uint32_t node_id);
    void register_channel(uint32_t node_id);
};

} // namespace sysp
