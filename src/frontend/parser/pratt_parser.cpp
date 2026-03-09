#include "frontend/parser/pratt_parser.hpp"

PrattParser::PrattParser(const std::vector<Token>& tokens)
: tokens(tokens), pos(0) {}

Token PrattParser::peek() {

    if(pos >= tokens.size())
        return tokens.back();

    return tokens[pos];
}

Token PrattParser::advance() {

    Token t = peek();
    pos++;

    return t;
}

Expr* PrattParser::parse_expression(int precedence) {

    Expr* left = parse_prefix();

    while(precedence < get_precedence(peek().type)) {

        Token op = advance();
        left = parse_infix(left, op);

    }

    return left;
}

Expr* PrattParser::parse_prefix() {

    Token t = advance();

    if(t.type == TokenType::INTEGER || t.type == TokenType::FLOAT) {

        auto* expr = new LiteralExpr();
        expr->value = t.lexeme;

        return expr;
    }

    if(t.type == TokenType::IDENT) {

        auto* expr = new IdentifierExpr();
        expr->name = t.lexeme;

        return expr;
    }

    return nullptr;
}

Expr* PrattParser::parse_infix(Expr* left, Token op) {

    int precedence = get_precedence(op.type);

    Expr* right = parse_expression(precedence);

    auto* expr = new BinaryExpr();

    expr->left = left;
    expr->right = right;
    expr->op = op.lexeme;

    return expr;
}

int PrattParser::get_precedence(TokenType type) {

    switch(type) {

        case TokenType::PLUS:
        case TokenType::MINUS:
            return 10;

        case TokenType::STAR:
        case TokenType::SLASH:
            return 20;

        default:
            return 0;
    }
}
