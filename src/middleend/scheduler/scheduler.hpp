#pragma once

#include "../metatron_graph/metatron_graph.hpp"
#include <vector>

class Scheduler {

public:

    std::vector<NodeId> schedule(const MetatronGraph& graph);

};
