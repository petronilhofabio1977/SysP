#include "ast_to_metatron.hpp"

void ASTToMetatron::convert(ASTNodePtr root, MetatronGraph& graph)
{
    if(auto block = dynamic_cast<BlockStmt*>(root))
    {
        for(auto stmt : block->statements)
        {
            if(auto expr_stmt = dynamic_cast<ExprStmt*>(stmt))
            {
                visit_expr(expr_stmt->expr,graph);
            }
        }
    }
}

MetatronNode* ASTToMetatron::visit_expr(Expr* node, MetatronGraph& graph)
{

    if(!node)
        return nullptr;

    if(auto literal = dynamic_cast<LiteralExpr*>(node))
    {
        return graph.create_node("literal:" + literal->value);
    }

    if(auto id = dynamic_cast<IdentifierExpr*>(node))
    {
        return graph.create_node("id:" + id->name);
    }

    if(auto bin = dynamic_cast<BinaryExpr*>(node))
    {
        auto left = visit_expr(bin->left,graph);
        auto right = visit_expr(bin->right,graph);

        auto op = graph.create_node("op:" + bin->op);

        if(left) graph.connect(left,op);
        if(right) graph.connect(right,op);

        return op;
    }

    if(auto un = dynamic_cast<UnaryExpr*>(node))
    {
        auto expr = visit_expr(un->expr,graph);

        auto op = graph.create_node("unary:" + un->op);

        if(expr) graph.connect(expr,op);

        return op;
    }

    if(auto call = dynamic_cast<CallExpr*>(node))
    {
        auto call_node = graph.create_node("call:" + call->callee);

        for(auto arg : call->args)
        {
            auto a = visit_expr(arg,graph);

            if(a)
                graph.connect(a,call_node);
        }

        return call_node;
    }

    return graph.create_node("unknown");

}

