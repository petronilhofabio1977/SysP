#include "analyzers.hpp"
#include <iostream>

// Type mismatch checker
// Uses node_types map: node_id → type string
// Checks binary operations and assignments for type compatibility

static bool numeric_type(const std::string& t) {
    return t=="i8"||t=="i16"||t=="i32"||t=="i64"||
           t=="u8"||t=="u16"||t=="u32"||t=="u64"||
           t=="f32"||t=="f64";
}

static bool types_compatible(const std::string& a, const std::string& b) {
    if (a == b)             return true;
    if (a.empty()||b.empty()) return true;  // unknown type — skip
    if (numeric_type(a) && numeric_type(b)) return true;  // numeric widening
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

        if (!types_compatible(lt->second, rt->second)) {
            std::string lname, rname;
            auto ln = node_names.find(left_id);
            auto rn = node_names.find(right_id);
            if (ln != node_names.end()) lname = "'" + ln->second + "'";
            if (rn != node_names.end()) rname = "'" + rn->second + "'";

            std::cerr << "[Jarbes] Error: type-mismatch — "
                      << lname << " (" << lt->second << ") vs "
                      << rname << " (" << rt->second << ")\n";
            ok = false;
        }
    }

    if (ok) std::cout << "    [Jarbes] type-mismatch: OK\n";
    return ok;
}
