#pragma once
#include "core/metatron_graph.hpp"
#include <cstdint>

namespace sysp {

    class JarbesKernel {
    public:
        void build_metatron(double version);
        void build_honeycomb(int layers);
        void distribute_transistors();
        void step();
        void analyze(MetatronGraph& graph);

        // Register a node ID as built-in (always valid, no producer needed)
        void register_builtin(uint32_t node_id);
    };

} // namespace sysp
