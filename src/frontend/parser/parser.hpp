#pragma once

#include <memory>

#include "../ast/stmt.hpp"
#include "../ast/expr.hpp"

namespace sysp::parser {

class Parser {

public:

    Parser();

    std::unique_ptr<sysp::ast::Stmt> parse_statement();

    std::unique_ptr<sysp::ast::ExpressionStmt> parse_expression_stmt();

    std::unique_ptr<sysp::ast::VarDeclStmt> parse_var();

    std::unique_ptr<sysp::ast::ConstDeclStmt> parse_const();

    std::unique_ptr<sysp::ast::AssignStmt> parse_assignment();

    std::unique_ptr<sysp::ast::ReturnStmt> parse_return();

    std::unique_ptr<sysp::ast::IfStmt> parse_if();

    std::unique_ptr<sysp::ast::WhileStmt> parse_while();

    std::unique_ptr<sysp::ast::ForStmt> parse_for();

};

}
