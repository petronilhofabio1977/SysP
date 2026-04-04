#pragma once

#include <vector>
#include <memory>
#include <string>
#include <stdexcept>

#include "../../core/token.hpp"
#include "../ast/decl.hpp"
#include "../ast/stmt.hpp"
#include "../ast/expr.hpp"

// ================================================================
// SysP Recursive Descent Parser — Grammar v7.0 Final
// ================================================================

namespace sysp::parser {

    // Parse error with location
    struct ParseError : std::runtime_error {
        int line, column;
        ParseError(const std::string& msg, int line, int col)
        : std::runtime_error(msg), line(line), column(col) {}
    };

    class Parser {
    public:
        explicit Parser(std::vector<Token> tokens);

        // Parse the whole program
        sysp::ast::Program parse_program();

    private:
        // ── Token navigation ─────────────────────────────────────────
        const Token& current() const;
        const Token& peek(int offset = 1) const;
        Token        advance();
        bool         check(TokenType t) const;
        bool         check_any(std::initializer_list<TokenType> types) const;
        bool         match(TokenType t);
        bool         match_any(std::initializer_list<TokenType> types);
        Token        expect(TokenType t, const std::string& msg);
        bool         is_at_end() const;

        ParseError error(const std::string& msg) const;

        // ── Top level ─────────────────────────────────────────────────
        std::unique_ptr<sysp::ast::ModuleDecl>   parse_module_decl();
        sysp::ast::DeclPtr                       parse_declaration();

        // ── Declarations ─────────────────────────────────────────────
        std::unique_ptr<sysp::ast::FunctionDecl>  parse_function(bool is_pub);
        std::unique_ptr<sysp::ast::StructDecl>    parse_struct(bool is_pub);
        std::unique_ptr<sysp::ast::EnumDecl>      parse_enum(bool is_pub);
        std::unique_ptr<sysp::ast::TraitDecl>     parse_trait();
        std::unique_ptr<sysp::ast::ImplTraitDecl> parse_impl_trait(
            const std::string& first_name,
            const std::vector<sysp::ast::GenericParam>& first_generics);
        sysp::ast::DeclPtr                        parse_impl();
        std::unique_ptr<sysp::ast::TypeAliasDecl> parse_type_alias(bool is_pub);
        std::unique_ptr<sysp::ast::ConstDecl>     parse_const_decl(bool is_pub);

        // ── Generic params and where clause ──────────────────────────
        std::vector<sysp::ast::GenericParam> parse_generic_params();
        std::vector<sysp::ast::WhereItem>    parse_where_clause();
        std::vector<std::string>             parse_trait_bound();

        // ── Parameters ───────────────────────────────────────────────
        std::vector<sysp::ast::Parameter> parse_parameter_list();
        sysp::ast::Parameter              parse_parameter();

        // ── Types ────────────────────────────────────────────────────
        std::string parse_type();

        // ── Block and statements ──────────────────────────────────────
        std::unique_ptr<sysp::ast::BlockStmt> parse_block();
        sysp::ast::StmtPtr                    parse_statement();

        sysp::ast::StmtPtr parse_var_decl();
        sysp::ast::StmtPtr parse_const_stmt();
        sysp::ast::StmtPtr parse_if();
        sysp::ast::StmtPtr parse_while();
        sysp::ast::StmtPtr parse_loop();
        sysp::ast::StmtPtr parse_for();
        sysp::ast::StmtPtr parse_match();
        sysp::ast::StmtPtr parse_return();
        sysp::ast::StmtPtr parse_break();
        sysp::ast::StmtPtr parse_region();
        sysp::ast::StmtPtr parse_unsafe();
        sysp::ast::StmtPtr parse_drop_stmt();
        sysp::ast::StmtPtr parse_panic();
        sysp::ast::StmtPtr parse_spawn();
        sysp::ast::StmtPtr parse_send();
        sysp::ast::StmtPtr parse_select();

        // ── Match patterns ────────────────────────────────────────────
        std::unique_ptr<sysp::ast::Pattern> parse_pattern();

        // ── Expressions (Pratt parser) ────────────────────────────────
        sysp::ast::ExprPtr parse_expression();
        sysp::ast::ExprPtr parse_expr_bp(int min_bp);   // binding power
        sysp::ast::ExprPtr parse_prefix();
        sysp::ast::ExprPtr parse_postfix(sysp::ast::ExprPtr left);
        sysp::ast::ExprPtr parse_primary();

        // Primary sub-parsers
        sysp::ast::ExprPtr parse_alloc();
        sysp::ast::ExprPtr parse_lambda();
        sysp::ast::ExprPtr parse_tuple_or_grouped();
        sysp::ast::ExprPtr parse_interpolated_string();
        sysp::ast::ExprPtr parse_struct_init();

        // Argument list
        std::vector<sysp::ast::ExprPtr> parse_argument_list();

        // Operator helpers
        static int infix_binding_power(TokenType op);
        static bool is_infix_op(TokenType t);
        static bool is_assignment_op(TokenType t);

    private:
        std::vector<Token> tokens_;
        size_t             pos_ = 0;
    };

} // namespace sysp::parser
