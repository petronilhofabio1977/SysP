#include "pratt_parser.hpp"

namespace sysp::parser {

PrattParser::PrattParser(const std::vector<Token>& tokens)
    : tokens(tokens)
{
}

Expr* PrattParser::parse_expression(int precedence)
{
    auto left = parse_prefix();

    while (!is_at_end() && precedence < get_precedence(peek().type))
    {
        Token op = advance();
        left = parse_infix(left, op);
    }

    return left;
}

Expr* PrattParser::parse_prefix()
{
    Token tok = advance();

    switch (tok.type)
    {

        case TokenType::INTEGER:
        case TokenType::FLOAT:
        case TokenType::STRING:
        case TokenType::BOOL_TRUE:
        case TokenType::BOOL_FALSE:
        case TokenType::IDENT:
        {
            auto node = new sysp::ast::LiteralExpr();
            node->value = tok.lexeme;
            return node;
        }

        case TokenType::LPAREN:
        {
            auto expr = parse_expression(0);

            if (!is_at_end() && peek().type == TokenType::RPAREN)
                advance();

            return expr;
        }

        default:
        {
            auto node = new sysp::ast::LiteralExpr();
            node->value = "0";
            return node;
        }

    }
}

Expr* PrattParser::parse_infix(Expr* left, Token op)
{
    int precedence = get_precedence(op.type);

    auto right = parse_expression(precedence);

    auto node = new sysp::ast::BinaryExpr();

    node->left.reset(left);
    node->right.reset(right);
    node->op = op.lexeme;

    return node;
}

Expr* PrattParser::parse_call_expr(Expr* callee)
{
    auto node = new sysp::ast::BinaryExpr();

    node->left.reset(callee);

    auto arg = std::make_unique<sysp::ast::LiteralExpr>();
    arg->value = "arg";

    node->right = std::move(arg);

    node->op = "call";

    return node;
}

Expr* PrattParser::parse_member_expr(Expr* object, const Token& name)
{
    auto node = new sysp::ast::BinaryExpr();

    node->left.reset(object);

    auto member = std::make_unique<sysp::ast::LiteralExpr>();
    member->value = name.lexeme;

    node->right = std::move(member);

    node->op = ".";

    return node;
}

Token PrattParser::peek() const
{
    return tokens[current];
}

Token PrattParser::advance()
{
    if (!is_at_end())
        current++;

    return previous();
}

Token PrattParser::previous() const
{
    return tokens[current - 1];
}

bool PrattParser::is_at_end() const
{
    return current >= tokens.size() ||
           tokens[current].type == TokenType::END;
}

int PrattParser::get_precedence(TokenType type) const
{
    switch (type)
    {
        case TokenType::STAR:
        case TokenType::SLASH:
        case TokenType::MOD:
            return 7;

        case TokenType::PLUS:
        case TokenType::MINUS:
            return 6;

        case TokenType::LT:
        case TokenType::GT:
        case TokenType::LE:
        case TokenType::GE:
            return 5;

        case TokenType::EQEQ:
        case TokenType::BANG_EQ:
            return 4;

        case TokenType::AND_AND:
            return 3;

        case TokenType::OR_OR:
            return 2;

        default:
            return 0;
    }
}

}
