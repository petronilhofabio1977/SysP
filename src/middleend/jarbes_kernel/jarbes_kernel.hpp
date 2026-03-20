#pragma once
#include "core/metatron_graph.hpp"
#include <cstdint>
#include <string>

namespace sysp {

class JarbesKernel {
public:
    void build_metatron(double version);
    void build_honeycomb(int layers);
    void distribute_transistors();
    void step();
    void analyze(MetatronGraph& graph);
    void register_builtin(uint32_t node_id);
    void register_move(uint32_t source_node_id);
    void register_region_node(uint32_t node_id, int region_id);
    void register_name(uint32_t node_id, const std::string& name);
};

} // namespace sysp
