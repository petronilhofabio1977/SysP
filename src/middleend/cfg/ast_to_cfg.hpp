#pragma once
#include "basic_block.hpp"
#include "frontend/ast/decl.hpp"
#include "frontend/ast/stmt.hpp"
#include "frontend/ast/expr.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace sysp::cfg {

// ================================================================
// AST → CFG Builder
// Converts a SysP function's AST into a Control Flow Graph
// ================================================================

class ASTToCFG {
public:
    // Build CFG from a function declaration
    std::vector<std::unique_ptr<BasicBlock>>
    build(const sysp::ast::FunctionDecl* fn);

    // Build CFG from a block statement
    std::vector<std::unique_ptr<BasicBlock>>
    build_from_block(const sysp::ast::BlockStmt* block);

private:
    std::vector<std::unique_ptr<BasicBlock>> blocks_;
    BasicBlock* current_ = nullptr;
    int         next_id_ = 0;
    int         label_counter_ = 0;

    // ── Block management ──────────────────────────────────────────
    BasicBlock* new_block(const std::string& label = "");
    void        set_current(BasicBlock* b);
    void        connect(BasicBlock* from, BasicBlock* to);

    // ── Statement → CFG ──────────────────────────────────────────
    void emit_stmt(const sysp::ast::Stmt* stmt);
    void emit_block(const sysp::ast::BlockStmt* block);
    void emit_var_decl(const sysp::ast::VarDeclStmt* stmt);
    void emit_assign(const sysp::ast::AssignStmt* stmt);
    void emit_if(const sysp::ast::IfStmt* stmt);
    void emit_while(const sysp::ast::WhileStmt* stmt);
    void emit_for(const sysp::ast::ForStmt* stmt);
    void emit_loop(const sysp::ast::LoopStmt* stmt);
    void emit_return(const sysp::ast::ReturnStmt* stmt);
    void emit_region(const sysp::ast::RegionStmt* stmt);
    void emit_unsafe(const sysp::ast::UnsafeStmt* stmt);
    void emit_expr_stmt(const sysp::ast::ExpressionStmt* stmt);

    // ── Expression → instructions ─────────────────────────────────
    void emit_expr(const sysp::ast::Expr* expr, const std::string& result = "");

    std::string new_label(const std::string& prefix = "L");
    std::string new_temp();
    int         temp_counter_ = 0;

    // Break/continue targets for loops
    BasicBlock* break_target_    = nullptr;
    BasicBlock* continue_target_ = nullptr;
};

} // namespace sysp::cfg
