#pragma once

#include "core/metatron_graph.hpp"

namespace sysp {

class JarbesKernel {

public:

    void build_metatron(double version);

    void build_honeycomb(int layers);

    void distribute_transistors();

    void step();

    void analyze(MetatronGraph &graph);

};

}
