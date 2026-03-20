#pragma once

#include "../../frontend/ast/decl.hpp"
#include "../jarbes_kernel/core/metatron_graph.hpp"
#include <string>

namespace sysp {

    class MetatronBuilder {
    public:
        // Old interface kept for compatibility
        MetatronGraph build_from_ast(const std::string& file);

        // New interface — receives real AST
        MetatronGraph build_from_program(const sysp::ast::Program& program);
    };

}
