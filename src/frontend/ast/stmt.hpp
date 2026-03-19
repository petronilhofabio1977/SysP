#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>

#include "expr.hpp"

// ================================================================
// SysP Statement AST — Grammar v7.0 Final
// ================================================================

namespace sysp::ast {

    // ── Base ─────────────────────────────────────────────────────────

    struct Stmt {
        virtual ~Stmt() = default;
    };

    using StmtPtr = std::unique_ptr<Stmt>;

    // ── Block ─────────────────────────────────────────────────────────

    struct BlockStmt : Stmt {
        std::vector<StmtPtr> statements;
    };

    // ── Expressions as statements ─────────────────────────────────────

    struct ExpressionStmt : Stmt {
        ExprPtr expression;
    };

    // ── Variable declarations ─────────────────────────────────────────

    // let x = expr
    // x: i32 = expr
    // let (a, b) = expr   (tuple destructuring)
    struct VarDeclStmt : Stmt {
        std::vector<std::string> names;     // single name or tuple names
        std::string              type;      // optional explicit type
        bool                     is_let       = true;
        bool                     is_tuple_destructure = false;
        ExprPtr                  initializer;
    };

    // const PI: f64 = 3.14
    struct ConstDeclStmt : Stmt {
        bool        is_pub = false;
        std::string name;
        std::string type;
        ExprPtr     value;
    };

    // ── Assignment ───────────────────────────────────────────────────

    // x = expr   x += expr   x[i] = expr   x.field = expr
    struct AssignStmt : Stmt {
        ExprPtr     target;     // assignable expression
        std::string op;         // = += -= *= /= %= &= |= ^= <<= >>=
        ExprPtr     value;
    };

    // ── Return ───────────────────────────────────────────────────────

    struct ReturnStmt : Stmt {
        ExprPtr value;          // optional
    };

    // ── Break / Continue ─────────────────────────────────────────────

    // break [expr]  — break with optional value from loop { }
    struct BreakStmt : Stmt {
        ExprPtr value;          // optional — for loop { break value }
    };

    struct ContinueStmt : Stmt {};

    // ── If / Else ────────────────────────────────────────────────────

    struct IfStmt : Stmt {
        ExprPtr                    condition;
        std::unique_ptr<BlockStmt> then_block;
        StmtPtr                    else_stmt;  // else block or else if
    };

    // ── While ────────────────────────────────────────────────────────

    struct WhileStmt : Stmt {
        ExprPtr                    condition;
        std::unique_ptr<BlockStmt> body;
    };

    // ── Loop ─────────────────────────────────────────────────────────

    // loop { ... break value ... }
    struct LoopStmt : Stmt {
        std::unique_ptr<BlockStmt> body;
    };

    // ── For ──────────────────────────────────────────────────────────

    // for x in iterable { }
    // iterable can be: range (0..n), slice, array, channel, custom Iterator
    struct ForStmt : Stmt {
        std::string                iterator;
        ExprPtr                    iterable;
        std::unique_ptr<BlockStmt> body;
    };

    // ── Match ────────────────────────────────────────────────────────

    // Pattern hierarchy
    enum class PatternKind {
        Wildcard,       // _
        Identifier,     // name  (binds value)
        Literal,        // 42  "hello"  true
        Range,          // 1..10  1..=10
        Tuple,          // (a, b)
        EnumVariant,    // Variant(p1, p2)
        Some,           // Some(p)
        None,           // None
        Ok,             // Ok(p)
        Err,            // Err(p)
    };

    struct Pattern {
        PatternKind kind = PatternKind::Wildcard;

        // Identifier / Wildcard
        std::string name;

        // Literal
        ExprPtr literal_value;

        // Range: low..high or low..=high
        std::string range_low;
        std::string range_high;
        bool        range_inclusive = false;

        // Tuple / EnumVariant / Some / Ok / Err
        std::vector<std::unique_ptr<Pattern>> sub_patterns;

        // EnumVariant name
        std::string variant_name;
    };

    // One arm: pattern [if guard] => stmt_or_expr
    struct MatchArm {
        std::unique_ptr<Pattern> pattern;
        ExprPtr                  guard;     // optional: if expr
        StmtPtr                  body;      // block or expression stmt
    };

    struct MatchStmt : Stmt {
        ExprPtr                  value;
        std::vector<MatchArm>    arms;
    };

    // ── Memory ───────────────────────────────────────────────────────

    // region name { }
    struct RegionStmt : Stmt {
        std::string                name;
        std::unique_ptr<BlockStmt> body;
    };

    // unsafe { }
    struct UnsafeStmt : Stmt {
        std::unique_ptr<BlockStmt> body;
    };

    // drop(expr) as statement
    struct DropStmt : Stmt {
        ExprPtr expr;
    };

    // ── Error ────────────────────────────────────────────────────────

    // panic("message")
    struct PanicStmt : Stmt {
        ExprPtr message;
    };

    // ── Concurrency ──────────────────────────────────────────────────

    // let handle = spawn expr
    struct SpawnStmt : Stmt {
        std::string name;   // variable receiving Task<T>
        ExprPtr     expr;
    };

    // send ch <- value
    struct SendStmt : Stmt {
        std::string channel;
        ExprPtr     value;
    };

    // select { recv ch -> v => { } ... default => { } }
    struct SelectArm {
        enum class Kind { Recv, Send, Default };
        Kind        kind;
        std::string channel;
        std::string bind_name;  // recv ch -> bind_name
        ExprPtr     send_value; // send ch <- value
        StmtPtr     body;
    };

    struct SelectStmt : Stmt {
        std::vector<SelectArm> arms;
    };

} // namespace sysp::ast
