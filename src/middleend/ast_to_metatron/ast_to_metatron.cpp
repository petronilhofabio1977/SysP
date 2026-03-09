#include "../metatron_graph/metatron_graph.hpp"
#include "ast_to_metatron.hpp"

ASTToMetatron::ASTToMetatron(MetatronGraph& graph)
: graph(graph) {}

NodeId ASTToMetatron::emit_expr(Expr* expr) {

    if(auto bin = dynamic_cast<BinaryExpr*>(expr)) {

        NodeId left = emit_expr(bin->left);
        NodeId right = emit_expr(bin->right);

        if(bin->op == "+")
            return graph.add_node(NodeKind::Add,{left,right});

        if(bin->op == "-")
            return graph.add_node(NodeKind::Sub,{left,right});

        if(bin->op == "*")
            return graph.add_node(NodeKind::Mul,{left,right});

        if(bin->op == "/")
            return graph.add_node(NodeKind::Div,{left,right});
    }

    if(auto lit = dynamic_cast<LiteralExpr*>(expr)) {

        return graph.add_node(NodeKind::Const,{});
    }

    if(auto id = dynamic_cast<IdentifierExpr*>(expr)) {

        return graph.add_node(NodeKind::Load,{});
    }

    return graph.add_node(NodeKind::Const,{});
}
