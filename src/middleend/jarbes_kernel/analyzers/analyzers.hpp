#pragma once
#include "../core/metatron_graph.hpp"
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>

// ================================================================
// SysP Jarbes Kernel — Security Analyzers
// All run on the MetatronGraph before code generation
// ================================================================

// ── Global registries ─────────────────────────────────────────────
extern std::unordered_set<uint32_t>              builtin_nodes;
extern std::unordered_map<uint32_t, bool>         consumed_nodes;
extern std::unordered_map<uint32_t, int>          node_region;
extern std::unordered_map<uint32_t, std::string>  node_names;
extern std::unordered_map<uint32_t, bool>         unsafe_nodes;
extern std::unordered_map<uint32_t, std::string>  node_types;
extern std::unordered_map<uint32_t, int>          array_bounds;
extern std::unordered_set<uint32_t>               freed_nodes;
extern std::unordered_map<uint32_t, uint32_t>     node_owner;
extern std::unordered_set<uint32_t>               spawn_nodes;
extern std::unordered_set<uint32_t>               channel_nodes;

// ── Accessor functions ────────────────────────────────────────────
inline std::unordered_set<uint32_t>&             get_builtin_nodes()  { return builtin_nodes; }
inline std::unordered_map<uint32_t, bool>&        get_consumed_nodes() { return consumed_nodes; }
inline std::unordered_map<uint32_t, int>&         get_node_region()    { return node_region; }
inline std::unordered_map<uint32_t, std::string>& get_node_names()     { return node_names; }
inline std::unordered_map<uint32_t, bool>&        get_unsafe_nodes()   { return unsafe_nodes; }
inline std::unordered_map<uint32_t, std::string>& get_node_types()     { return node_types; }
inline std::unordered_map<uint32_t, int>&         get_array_bounds()   { return array_bounds; }
inline std::unordered_set<uint32_t>&             get_freed_nodes()    { return freed_nodes; }
inline std::unordered_map<uint32_t, uint32_t>&   get_node_owner()     { return node_owner; }
inline std::unordered_set<uint32_t>&             get_spawn_nodes()    { return spawn_nodes; }
inline std::unordered_set<uint32_t>&             get_channel_nodes()  { return channel_nodes; }

// ── Checkers ──────────────────────────────────────────────────────
bool check_use_before_production(const MetatronGraph& graph);
bool check_use_after_move(const MetatronGraph& graph);
bool check_region_escape(const MetatronGraph& graph);
bool check_double_free(const MetatronGraph& graph);
bool check_dangling_pointer(const MetatronGraph& graph);
bool check_buffer_overflow(const MetatronGraph& graph);
bool check_unsafe_audit(const MetatronGraph& graph);
bool check_type_mismatch(const MetatronGraph& graph);
bool detect_cycle(const MetatronGraph& graph);
bool detect_data_race(const MetatronGraph& graph);
