#pragma once

#include "../../../frontend/ast/ast.hpp"
#include "../../../frontend/ast/expr.hpp"
#include "../../../frontend/ast/stmt.hpp"

#include "../metatron_graph.hpp"

class ASTToMetatron {

public:

    static void convert(ASTNodePtr root, MetatronGraph& graph);

private:

    static MetatronNode* visit_expr(Expr* node, MetatronGraph& graph);

};

