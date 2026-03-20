#pragma once
#include "../core/metatron_graph.hpp"
#include <unordered_set>
#include <unordered_map>
#include <string>

extern std::unordered_set<uint32_t>              builtin_nodes;
extern std::unordered_map<uint32_t, bool>         consumed_nodes;
extern std::unordered_map<uint32_t, int>          node_region;
extern std::unordered_map<uint32_t, std::string>  node_names;

// Accessor functions — guaranteed single instance across all translation units
inline std::unordered_set<uint32_t>&             get_builtin_nodes()  { return builtin_nodes; }
inline std::unordered_map<uint32_t, bool>&        get_consumed_nodes() { return consumed_nodes; }
inline std::unordered_map<uint32_t, int>&         get_node_region()    { return node_region; }
inline std::unordered_map<uint32_t, std::string>& get_node_names()     { return node_names; }

bool check_use_before_production(const MetatronGraph& graph);
bool check_use_after_move(const MetatronGraph& graph);
bool check_region_escape(const MetatronGraph& graph);
bool detect_cycle(const MetatronGraph& graph);
bool detect_data_race(const MetatronGraph& graph);
