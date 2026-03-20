#pragma once

#include <ostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "../../frontend/ast/decl.hpp"
#include "../../middleend/metatron_graph/metatron_graph.hpp"

// ================================================================
// SysP x86-64 Backend — Grammar v7.0 Final
// Generates NASM-compatible assembly from AST
// ================================================================

namespace sysp::backend::x86 {

    // A string constant collected during codegen
    struct StringConstant {
        std::string label;
        std::string value;
    };

    class Backend {
    public:
        Backend();

        // Old interface — kept for compatibility
        void generate(const sysp::metatron::Graph& graph, std::ostream& out);

        // New interface — generates from real AST
        void generate_program(const sysp::ast::Program& program, std::ostream& out);

    private:
        // ── Data section ─────────────────────────────────────────────
        std::vector<StringConstant> string_constants_;
        int                         str_counter_ = 0;

        // ── Register / variable tracking ─────────────────────────────
        std::unordered_map<std::string, int> var_offsets_; // var → stack offset
        int stack_offset_ = 0;

        // ── Code generation ───────────────────────────────────────────
        void gen_function(const sysp::ast::FunctionDecl* fn, std::ostream& out);
        void gen_block(const sysp::ast::BlockStmt* block, std::ostream& out);
        void gen_stmt(const sysp::ast::Stmt* stmt, std::ostream& out);
        void gen_expr(const sysp::ast::Expr* expr, std::ostream& out);

        // Specific statement generators
        void gen_var_decl(const sysp::ast::VarDeclStmt* stmt, std::ostream& out);
        void gen_assign(const sysp::ast::AssignStmt* stmt, std::ostream& out);
        void gen_return(const sysp::ast::ReturnStmt* stmt, std::ostream& out);
        void gen_if(const sysp::ast::IfStmt* stmt, std::ostream& out);
        void gen_while(const sysp::ast::WhileStmt* stmt, std::ostream& out);
        void gen_for(const sysp::ast::ForStmt* stmt, std::ostream& out);
        void gen_expr_stmt(const sysp::ast::ExpressionStmt* stmt, std::ostream& out);

        // Specific expression generators
        void gen_literal(const sysp::ast::LiteralExpr* expr, std::ostream& out);
        void gen_identifier(const sysp::ast::IdentifierExpr* expr, std::ostream& out);
        void gen_binary(const sysp::ast::BinaryExpr* expr, std::ostream& out);
        void gen_call(const sysp::ast::CallExpr* expr, std::ostream& out);
        void gen_println_call(const sysp::ast::CallExpr* expr, std::ostream& out);

        // Helpers
        std::string new_string_label(const std::string& value);
        std::string new_label(const std::string& prefix);
        int         label_counter_ = 0;

        int  alloc_stack_var(const std::string& name, int bytes = 8);
        void emit_function_prologue(const std::string& name, int stack_size, std::ostream& out);
        void emit_function_epilogue(std::ostream& out);

        void emit_data_section(std::ostream& out);
        void emit_println_int(std::ostream& out);
        void emit_println_float(std::ostream& out);
        void emit_println_bool(std::ostream& out);
    };

} // namespace sysp::backend::x86
