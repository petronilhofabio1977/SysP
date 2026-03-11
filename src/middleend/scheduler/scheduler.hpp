#pragma once

#include <vector>

#include "../metatron_graph/metatron_graph.hpp"

namespace sysp::scheduler {

class Scheduler {

public:

    Scheduler();

    std::vector<sysp::metatron::Node*> schedule(
        const sysp::metatron::Graph& graph
    );

};

}
