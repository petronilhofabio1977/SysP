#include "pratt_parser.hpp"

namespace sysp::parser {

PrattParser::PrattParser()
{
}

sysp::ast::Expr* PrattParser::parse_expression(int precedence)
{
    return parse_prefix();
}

sysp::ast::Expr* PrattParser::parse_prefix()
{
    auto node = new sysp::ast::LiteralExpr();
    node->value = "0";
    return node;
}

sysp::ast::Expr* PrattParser::parse_infix(sysp::ast::Expr* left, Token op)
{
    auto node = new sysp::ast::BinaryExpr();

    node->left.reset(left);

    auto right = std::make_unique<sysp::ast::LiteralExpr>();
    right->value = "0";

    node->right = std::move(right);

    node->op = op.lexeme;

    return node;
}

}
