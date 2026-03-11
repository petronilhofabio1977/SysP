#pragma once

#include "../metatron_graph/metatron_graph.hpp"

namespace sysp::optimizer {

class Optimizer {

public:

    Optimizer();

    void run(sysp::metatron::Graph& graph);

private:

    void constant_folding(sysp::metatron::Graph& graph);
    void eliminate_dead_nodes(sysp::metatron::Graph& graph);
    void common_subexpression(sysp::metatron::Graph& graph);

};

}
