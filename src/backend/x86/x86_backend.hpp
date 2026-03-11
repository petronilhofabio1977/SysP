#pragma once

#include <ostream>

#include "../../middleend/metatron_graph/metatron_graph.hpp"

namespace sysp::backend::x86 {

class Backend {

public:

    Backend();

    void generate(
        const sysp::metatron::Graph& graph,
        std::ostream& out
    );

};

}
