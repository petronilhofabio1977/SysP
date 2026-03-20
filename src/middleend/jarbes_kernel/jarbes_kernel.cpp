#include "jarbes_kernel.hpp"
#include "analyzers/analyzers.hpp"
#include <iostream>
#include <stdexcept>

namespace sysp {

    void JarbesKernel::build_metatron(double version) {
        std::cout << "[JarbesKernel] Metatron initialized version "
        << version << std::endl;
    }

    void JarbesKernel::build_honeycomb(int layers) {
        std::cout << "[JarbesKernel] Honeycomb grid with "
        << layers << " layers created" << std::endl;
    }

    void JarbesKernel::distribute_transistors() {
        std::cout << "[JarbesKernel] Distributing logical transistors" << std::endl;
    }

    void JarbesKernel::step() {
        std::cout << "[JarbesKernel] Kernel logical step executed" << std::endl;
    }

    void JarbesKernel::register_builtin(uint32_t node_id) {
        builtin_nodes.insert(node_id);
    }

    void JarbesKernel::analyze(MetatronGraph& graph) {
        std::cout << "[JarbesKernel] Graph nodes: "
        << graph.nodes.size() << std::endl;

        if (!check_use_before_production(graph))
            throw std::runtime_error("JarbesKernel: dependency error");

        if (!detect_cycle(graph))
            throw std::runtime_error("JarbesKernel: cycle detected");

        std::cout << "[Jarbes] Analysis complete — no violations.\n";
    }

} // namespace sysp
