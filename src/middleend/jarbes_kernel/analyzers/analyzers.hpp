#pragma once
#include "../core/metatron_graph.hpp"
#include <unordered_set>

// Built-in function node IDs that are always valid
// (println, print, etc. don't need a producer node)
extern std::unordered_set<uint32_t> builtin_nodes;

bool check_use_before_production(const MetatronGraph& graph);
bool detect_cycle(const MetatronGraph& graph);
bool detect_data_race(const MetatronGraph& graph);
