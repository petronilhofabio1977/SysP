#pragma once
#include "../jarbes_kernel/core/metatron_graph.hpp"

namespace sysp::optimizer {

class Optimizer {
public:
    Optimizer();
    int run(MetatronGraph& graph);
private:
    int eliminate_dead_nodes(MetatronGraph& graph);
    int constant_folding(MetatronGraph& graph);
};

} // namespace sysp::optimizer
