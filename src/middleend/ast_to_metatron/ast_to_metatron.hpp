#pragma once

#include "../../frontend/ast/expr.hpp"
#include "../metatron_graph/metatron_graph.hpp"

class ASTToMetatron {

public:

    ASTToMetatron(MetatronGraph& graph);

    NodeId emit_expr(Expr* expr);

private:

    MetatronGraph& graph;
};
