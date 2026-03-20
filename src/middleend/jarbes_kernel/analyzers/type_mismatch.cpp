#include "analyzers.hpp"
#include <iostream>

static bool is_integer_type(const std::string& t) {
    return t=="i8"||t=="i16"||t=="i32"||t=="i64"||
           t=="u8"||t=="u16"||t=="u32"||t=="u64";
}

static bool is_float_type(const std::string& t) {
    return t=="f32"||t=="f64";
}

static bool is_numeric_type(const std::string& t) {
    return is_integer_type(t) || is_float_type(t);
}

// Returns true if types are compatible for an operation
static bool types_compatible(const std::string& a, const std::string& b) {
    if (a.empty() || b.empty())          return true; // unknown — skip
    if (a == b)                          return true; // same type
    if (is_numeric_type(a) && is_numeric_type(b)) return true; // numeric widening OK
    // bool can be compared but not mixed with numeric in arithmetic
    if (a == "bool" && b == "bool")      return true;
    // string operations
    if (a == "string" && b == "string")  return true;
    return false;
}

// Returns false if types are CLEARLY incompatible (not just unknown)
static bool types_clearly_incompatible(const std::string& a, const std::string& b) {
    if (a.empty() || b.empty()) return false;
    if (is_numeric_type(a) && b == "string") return true;
    if (is_numeric_type(a) && b == "bool")   return true;
    if (a == "string" && is_numeric_type(b)) return true;
    if (a == "bool"   && is_numeric_type(b)) return true;
    if (a == "string" && b == "bool")        return true;
    if (a == "bool"   && b == "string")      return true;
    return false;
}

bool check_type_mismatch(const MetatronGraph& graph) {
    bool ok = true;

    for (const auto& node : graph.nodes) {
        if (node.inputs.size() < 2) continue;

        uint32_t left_id  = node.inputs[0];
        uint32_t right_id = node.inputs[1];

        auto lt = node_types.find(left_id);
        auto rt = node_types.find(right_id);

        if (lt == node_types.end() || rt == node_types.end()) continue;

        // Only report CLEAR incompatibilities — not numeric widening
        if (types_clearly_incompatible(lt->second, rt->second)) {
            std::string lname, rname;
            auto ln = node_names.find(left_id);
            auto rn = node_names.find(right_id);
            if (ln != node_names.end()) lname = "'" + ln->second + "' ";
            if (rn != node_names.end()) rname = "'" + rn->second + "' ";

            std::cerr << "[Jarbes] Error: type-mismatch — "
                      << lname << "(" << lt->second << ") "
                      << "cannot be used with "
                      << rname << "(" << rt->second << ")\n";
            ok = false;
        }
    }

    if (ok) std::cout << "    [Jarbes] type-mismatch: OK\n";
    return ok;
}
