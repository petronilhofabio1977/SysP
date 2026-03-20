#include "analyzers.hpp"
#include "frontend/ast/stmt.hpp"
#include "frontend/ast/decl.hpp"
#include <iostream>
#include <unordered_set>

// ================================================================
// Jarbes Checker: non-exhaustive-match
//
// Detects match expressions that don't cover all cases:
//   match x {
//       Some(v) => ...
//   }  -- ERROR: None not covered
// ================================================================

static bool has_wildcard(const std::vector<sysp::ast::MatchArm>& arms) {
    for (auto& arm : arms) {
        if (!arm.pattern) continue;
        if (arm.pattern->kind == sysp::ast::PatternKind::Wildcard)    return true;
        if (arm.pattern->kind == sysp::ast::PatternKind::Identifier)  return true;
    }
    return false;
}

static bool check_match_stmt(const sysp::ast::MatchStmt* stmt) {
    bool ok = true;

    // If there's a wildcard/catch-all pattern, it's always exhaustive
    if (has_wildcard(stmt->arms)) return true;

    // Check Option patterns
    bool has_some = false, has_none = false;
    bool has_ok   = false, has_err  = false;
    bool has_option_pattern = false;
    bool has_result_pattern = false;

    for (auto& arm : stmt->arms) {
        if (!arm.pattern) continue;
        switch (arm.pattern->kind) {
        case sysp::ast::PatternKind::Some: has_some = true; has_option_pattern = true; break;
        case sysp::ast::PatternKind::None: has_none = true; has_option_pattern = true; break;
        case sysp::ast::PatternKind::Ok:   has_ok   = true; has_result_pattern = true; break;
        case sysp::ast::PatternKind::Err:  has_err  = true; has_result_pattern = true; break;
        default: break;
        }
    }

    if (has_option_pattern) {
        if (!has_some) {
            std::cerr << "[Jarbes] Error: non-exhaustive-match — 'Some' case not covered\n";
            ok = false;
        }
        if (!has_none) {
            std::cerr << "[Jarbes] Error: non-exhaustive-match — 'None' case not covered\n";
            ok = false;
        }
    }

    if (has_result_pattern) {
        if (!has_ok) {
            std::cerr << "[Jarbes] Error: non-exhaustive-match — 'Ok' case not covered\n";
            ok = false;
        }
        if (!has_err) {
            std::cerr << "[Jarbes] Error: non-exhaustive-match — 'Err' case not covered\n";
            ok = false;
        }
    }

    return ok;
}

bool check_match_exhaustive(const sysp::ast::Program& program) {
    bool ok = true;

    for (auto& decl : program.declarations) {
        auto* fn = dynamic_cast<const sysp::ast::FunctionDecl*>(decl.get());
        if (!fn || !fn->body) continue;

        std::function<void(const sysp::ast::Stmt*)> walk;
        walk = [&](const sysp::ast::Stmt* stmt) {
            if (!stmt) return;
            if (auto* m = dynamic_cast<const sysp::ast::MatchStmt*>(stmt)) {
                if (!check_match_stmt(m)) ok = false;
            }
            if (auto* b = dynamic_cast<const sysp::ast::BlockStmt*>(stmt)) {
                for (auto& s : b->statements) walk(s.get());
            }
            if (auto* i = dynamic_cast<const sysp::ast::IfStmt*>(stmt)) {
                if (i->then_block) walk(i->then_block.get());
                if (i->else_stmt)  walk(i->else_stmt.get());
            }
            if (auto* w = dynamic_cast<const sysp::ast::WhileStmt*>(stmt)) {
                if (w->body) walk(w->body.get());
            }
            if (auto* f = dynamic_cast<const sysp::ast::ForStmt*>(stmt)) {
                if (f->body) walk(f->body.get());
            }
        };
        walk(fn->body.get());
    }

    if (ok) std::cout << "    [Jarbes] match-exhaustive: OK\n";
    return ok;
}
