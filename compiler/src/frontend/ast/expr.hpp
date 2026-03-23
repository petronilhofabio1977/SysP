#pragma once

#include <string>
#include <vector>
#include <memory>

// ================================================================
// SysP Expression AST — Grammar v7.0 Final
// ================================================================

namespace sysp::ast {

    struct BlockStmt; // forward declaration

    // ── Base ─────────────────────────────────────────────────────────

    struct Expr {
        virtual ~Expr() = default;
    };

    using ExprPtr = std::unique_ptr<Expr>;

    // ── Literals ─────────────────────────────────────────────────────

    enum class LiteralKind { Int, Float, String, Bool };

    struct LiteralExpr : Expr {
        std::string  value;
        LiteralKind  kind;
    };

    // Interpolated string segment: either raw text or an expression
    struct InterpPart {
        bool        is_expr = false;
        std::string raw_text;               // when !is_expr
        ExprPtr     expr;                   // when is_expr
    };

    // f"Hello, {nome}! You are {age} years old."
    struct InterpolatedStringExpr : Expr {
        std::vector<InterpPart> parts;
    };

    // ── Identifier ───────────────────────────────────────────────────

    struct IdentifierExpr : Expr {
        std::string name;
    };

    // ── Tuple ────────────────────────────────────────────────────────

    // (10, "hello", true)
    struct TupleExpr : Expr {
        std::vector<ExprPtr> elements;
    };

    // ── Range ────────────────────────────────────────────────────────

    // 0..10   0..=10
    struct RangeExpr : Expr {
        ExprPtr start;
        ExprPtr end;
        bool    inclusive = false;
    };

    // ── Arithmetic / logical / bitwise ───────────────────────────────

    struct BinaryExpr : Expr {
        ExprPtr     left;
        std::string op;
        ExprPtr     right;
    };

    struct UnaryExpr : Expr {
        std::string op;        // - ! ~ * &
        ExprPtr     operand;
    };

    // ── Cast ─────────────────────────────────────────────────────────

    // expr as Type
    struct CastExpr : Expr {
        ExprPtr     expr;
        std::string target_type;
    };

    // ── Call and access ───────────────────────────────────────────────

    // foo(a, b)
    struct CallExpr : Expr {
        ExprPtr                  callee;
        std::vector<ExprPtr>     arguments;
    };

    // obj.method(a, b)
    struct MethodCallExpr : Expr {
        ExprPtr                  object;
        std::string              method;
        std::vector<ExprPtr>     arguments;
    };

    // array[index]
    struct IndexExpr : Expr {
        ExprPtr object;
        ExprPtr index;
    };

    // obj.field
    struct MemberExpr : Expr {
        ExprPtr     object;
        std::string field;
    };

    // tuple.0   tuple.1
    struct TupleAccessExpr : Expr {
        ExprPtr object;
        int     index = 0;
    };

    // ── Memory ───────────────────────────────────────────────────────

    // new Type(args)   new [n]Type
    struct AllocExpr : Expr {
        std::string          alloc_type;
        std::vector<ExprPtr> arguments;
        bool                 is_array = false;
        ExprPtr              array_size;     // only when is_array
    };

    // move expr
    struct MoveExpr : Expr {
        ExprPtr expr;
    };

    // drop(expr) as expression (returns unit)
    struct DropExpr : Expr {
        ExprPtr expr;
    };

    // ── Error handling ───────────────────────────────────────────────

    // Ok(expr)
    struct OkExpr : Expr {
        ExprPtr expr;
    };

    // Err(expr)
    struct ErrExpr : Expr {
        ExprPtr expr;
    };

    // Some(expr)
    struct SomeExpr : Expr {
        ExprPtr expr;
    };

    // None
    struct NoneExpr : Expr {};

    // ── Concurrency ──────────────────────────────────────────────────

    // channel()
    struct ChannelExpr : Expr {};

    // recv channel_expr
    struct RecvExpr : Expr {
        ExprPtr channel;
    };

    // await task_expr
    struct AwaitExpr : Expr {
        ExprPtr task;
    };

    // spawn expr (used inside SpawnStmt, but also valid as expression)
    struct SpawnExpr : Expr {
        ExprPtr expr;
    };

    // ── Lambda ───────────────────────────────────────────────────────

    struct LambdaParam {
        std::string name;
        std::string type;
        bool        is_ref = false;
    };

    // fn(x i32, y i32) -> i32 { ... }
    struct LambdaExpr : Expr {
        std::vector<LambdaParam>          parameters;
        std::string                       return_type;
        std::unique_ptr<BlockStmt>        body;
    };

} // namespace sysp::ast
