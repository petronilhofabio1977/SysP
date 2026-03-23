#include "analyzers.hpp"
#include "frontend/ast/decl.hpp"
#include "frontend/ast/stmt.hpp"
#include "frontend/ast/expr.hpp"
#include <iostream>
#include <limits>

// ================================================================
// Jarbes Checker: integer-overflow (static)
//
// Detects constant integer overflow at compile time:
//   let x: i32 = 2147483647 + 1  -- ERROR
// ================================================================

static bool check_overflow_expr(
    const sysp::ast::Expr* expr,
    const std::string& type_hint,
    bool& ok)
{
    if (!expr) return true;

    auto* bin = dynamic_cast<const sysp::ast::BinaryExpr*>(expr);
    if (!bin) return true;

    auto* left  = dynamic_cast<const sysp::ast::LiteralExpr*>(bin->left.get());
    auto* right = dynamic_cast<const sysp::ast::LiteralExpr*>(bin->right.get());

    if (!left || !right) return true;
    if (left->kind != sysp::ast::LiteralKind::Int) return true;
    if (right->kind != sysp::ast::LiteralKind::Int) return true;

    try {
        int64_t l = std::stoll(left->value);
        int64_t r = std::stoll(right->value);
        int64_t result = 0;

        if      (bin->op == "+") result = l + r;
        else if (bin->op == "-") result = l - r;
        else if (bin->op == "*") result = l * r;
        else return true;

        // Check bounds based on type
        int64_t min_val = std::numeric_limits<int32_t>::min();
        int64_t max_val = std::numeric_limits<int32_t>::max();

        if (type_hint == "i8")  { min_val = -128;        max_val = 127; }
        if (type_hint == "i16") { min_val = -32768;       max_val = 32767; }
        if (type_hint == "i32") { min_val = INT32_MIN;    max_val = INT32_MAX; }
        if (type_hint == "i64") { min_val = INT64_MIN;    max_val = INT64_MAX; }
        if (type_hint == "u8")  { min_val = 0;            max_val = 255; }
        if (type_hint == "u16") { min_val = 0;            max_val = 65535; }
        if (type_hint == "u32") { min_val = 0;            max_val = UINT32_MAX; }

        if (result < min_val || result > max_val) {
            std::cerr << "[Jarbes] Error: integer-overflow — "
                      << l << " " << bin->op << " " << r
                      << " = " << result
                      << " overflows " << (type_hint.empty() ? "i32" : type_hint)
                      << " (range: " << min_val << " to " << max_val << ")\n";
            ok = false;
        }
    } catch (...) {}

    return ok;
}

bool check_integer_overflow(const sysp::ast::Program& program) {
    bool ok = true;

    for (auto& decl : program.declarations) {
        auto* fn = dynamic_cast<const sysp::ast::FunctionDecl*>(decl.get());
        if (!fn || !fn->body) continue;

        // Walk all statements looking for var_decl with binary init
        std::function<void(const sysp::ast::Stmt*)> walk_stmt;
        walk_stmt = [&](const sysp::ast::Stmt* stmt) {
            if (!stmt) return;

            if (auto* vd = dynamic_cast<const sysp::ast::VarDeclStmt*>(stmt)) {
                check_overflow_expr(vd->initializer.get(), vd->type, ok);
            }
            if (auto* blk = dynamic_cast<const sysp::ast::BlockStmt*>(stmt)) {
                for (auto& s : blk->statements) walk_stmt(s.get());
            }
            if (auto* ifs = dynamic_cast<const sysp::ast::IfStmt*>(stmt)) {
                if (ifs->then_block) walk_stmt(ifs->then_block.get());
                if (ifs->else_stmt)  walk_stmt(ifs->else_stmt.get());
            }
            if (auto* ws = dynamic_cast<const sysp::ast::WhileStmt*>(stmt)) {
                if (ws->body) walk_stmt(ws->body.get());
            }
        };
        walk_stmt(fn->body.get());
    }

    if (ok) std::cout << "    [Jarbes] integer-overflow: OK\n";
    return ok;
}
