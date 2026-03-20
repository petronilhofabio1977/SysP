#include "analyzers.hpp"
#include <iostream>

bool detect_data_race(const MetatronGraph& graph) {
    // TODO: implement data race detection for concurrent nodes
    // For now — no races detected in single-threaded programs
    (void)graph;
    return true;
}
