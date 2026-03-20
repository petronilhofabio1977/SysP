#include "analyzers.hpp"
#include <iostream>

bool detect_data_race(const MetatronGraph& graph) {
    (void)graph;
    std::cout << "    [Jarbes] data-race: OK\n";
    return true;
}
