#include "metatron_builder.hpp"
#include "../jarbes_kernel/analyzers/analyzers.hpp"
#include "symbol_table.hpp"
#include "../../frontend/ast/decl.hpp"
#include "../../frontend/ast/stmt.hpp"
#include "../../frontend/ast/expr.hpp"
#include "../../middleend/jarbes_kernel/analyzers/analyzers.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>
#include <unordered_map>

namespace sysp {

    // ================================================================
    // Old file-based interface (kept for compatibility)
    // ================================================================

    static bool is_identifier(const std::string& s) {
        if (s.empty()) return false;
        if (!std::isalpha(s[0]) && s[0] != '_') return false;
        for (char c : s) {
            if (!std::isalnum(c) && c != '_') return false;
        }
        return true;
    }

    MetatronGraph MetatronBuilder::build_from_ast(const std::string& file) {
        MetatronGraph graph;
        SymbolTable symbols;
        std::ifstream input(file);
        std::string line;
        uint32_t node_id = 0;

        while (std::getline(input, line)) {
            if (line.find("=") == std::string::npos) continue;
            std::stringstream ss(line);
            std::string lhs, eq;
            ss >> lhs >> eq;
            MetatronNode node;
            node.id = node_id;
            std::string token;
            while (ss >> token) {
                if (is_identifier(token))
                    node.inputs.push_back(node_id + 1);
            }
            node.outputs.push_back(node_id);
            graph.nodes.push_back(node);
            symbols.set(lhs, node_id);
            node_id++;
        }
        return graph;
    }

    // ================================================================
    // New AST-based interface
    // ================================================================

    // Context passed through the traversal
    struct BuildContext {
        MetatronGraph&  graph;
        SymbolTable     symbols;
        uint32_t        next_id = 0;

        uint32_t new_node() {
            MetatronNode node;
            node.id = next_id++;
            graph.nodes.push_back(node);
            return node.id;
        }

        uint32_t new_node_with_dep(uint32_t dep_id) {
            MetatronNode node;
            node.id = next_id++;
            node.inputs.push_back(dep_id);
            graph.nodes.push_back(node);
            return node.id;
        }
    };

    // ── Forward declarations ──────────────────────────────────────────
    static void process_block(BuildContext& ctx, const sysp::ast::BlockStmt* block);
    static void process_stmt(BuildContext& ctx, const sysp::ast::Stmt* stmt);
    static uint32_t process_expr(BuildContext& ctx, const sysp::ast::Expr* expr);

    // ── Expression processing ─────────────────────────────────────────

    static uint32_t process_expr(BuildContext& ctx, const sysp::ast::Expr* expr) {
        if (!expr) return ctx.new_node();

        // Literal — register type and value for checkers
        if (auto* e = dynamic_cast<const sysp::ast::LiteralExpr*>(expr)) {
            uint32_t lit_node = ctx.new_node();
            switch (e->kind) {
            case sysp::ast::LiteralKind::Int:
                get_node_types()[lit_node] = "i32";
                get_node_names()[lit_node] = e->value; // for buffer-overflow
                try { get_node_const_values()[lit_node] = std::stoll(e->value); }
                catch (...) {}
                break;
            case sysp::ast::LiteralKind::Float:
                get_node_types()[lit_node] = "f64";
                break;
            case sysp::ast::LiteralKind::String:
                get_node_types()[lit_node] = "string";
                break;
            case sysp::ast::LiteralKind::Bool:
                get_node_types()[lit_node] = "bool";
                break;
            }
            return lit_node;
        }

        // Identifier — look up in symbol table
        if (auto* e = dynamic_cast<const sysp::ast::IdentifierExpr*>(expr)) {
            int id = ctx.symbols.get(e->name);
            if (id >= 0) return static_cast<uint32_t>(id);
            return ctx.new_node();
        }

        // Binary expression: left op right
        if (auto* e = dynamic_cast<const sysp::ast::BinaryExpr*>(expr)) {
            uint32_t left  = process_expr(ctx, e->left.get());
            uint32_t right = process_expr(ctx, e->right.get());
            MetatronNode node;
            node.id = ctx.next_id++;
            node.inputs.push_back(left);
            node.inputs.push_back(right);
            node.outputs.push_back(node.id);
            ctx.graph.nodes.push_back(node);
            return node.id;
        }

        // Unary expression
        if (auto* e = dynamic_cast<const sysp::ast::UnaryExpr*>(expr)) {
            uint32_t operand = process_expr(ctx, e->operand.get());
            return ctx.new_node_with_dep(operand);
        }

        // Call expression: fn(args)
        if (auto* e = dynamic_cast<const sysp::ast::CallExpr*>(expr)) {
            MetatronNode node;
            node.id = ctx.next_id++;
            // callee
            uint32_t callee = process_expr(ctx, e->callee.get());
            node.inputs.push_back(callee);
            // arguments
            for (auto& arg : e->arguments)
                node.inputs.push_back(process_expr(ctx, arg.get()));
            node.outputs.push_back(node.id);
            ctx.graph.nodes.push_back(node);
            return node.id;
        }

        // Method call: obj.method(args)
        if (auto* e = dynamic_cast<const sysp::ast::MethodCallExpr*>(expr)) {
            MetatronNode node;
            node.id = ctx.next_id++;
            node.inputs.push_back(process_expr(ctx, e->object.get()));
            for (auto& arg : e->arguments)
                node.inputs.push_back(process_expr(ctx, arg.get()));
            node.outputs.push_back(node.id);
            ctx.graph.nodes.push_back(node);
            return node.id;
        }

        // Member access: obj.field
        if (auto* e = dynamic_cast<const sysp::ast::MemberExpr*>(expr)) {
            uint32_t obj = process_expr(ctx, e->object.get());
            return ctx.new_node_with_dep(obj);
        }

        // Index: array[i]
        if (auto* e = dynamic_cast<const sysp::ast::IndexExpr*>(expr)) {
            uint32_t obj = process_expr(ctx, e->object.get());
            uint32_t idx = process_expr(ctx, e->index.get());
            MetatronNode node;
            node.id = ctx.next_id++;
            node.inputs.push_back(obj);
            node.inputs.push_back(idx);
            node.outputs.push_back(node.id);
            ctx.graph.nodes.push_back(node);
            return node.id;
        }

        // Tuple
        if (auto* e = dynamic_cast<const sysp::ast::TupleExpr*>(expr)) {
            MetatronNode node;
            node.id = ctx.next_id++;
            for (auto& el : e->elements)
                node.inputs.push_back(process_expr(ctx, el.get()));
            node.outputs.push_back(node.id);
            ctx.graph.nodes.push_back(node);
            return node.id;
        }

        // Move — mark source as consumed in Jarbes
        if (auto* e = dynamic_cast<const sysp::ast::MoveExpr*>(expr)) {
            // Resolve the identifier being moved
            uint32_t src = process_expr(ctx, e->expr.get());
            // If moving an identifier, get the actual variable node
            if (auto* id = dynamic_cast<const sysp::ast::IdentifierExpr*>(e->expr.get())) {
                int var_id = ctx.symbols.get(id->name);
                if (var_id >= 0) {
                    src = static_cast<uint32_t>(var_id);
                    // Copy name to consumed map for error messages
                    get_node_names()[src] = id->name;
                }
            }
            get_consumed_nodes()[src] = true;
            uint32_t move_result = ctx.new_node_with_dep(src);
            get_move_result_nodes().insert(move_result); // this node IS the move — not an error
            return move_result;
        }

        // Alloc: new Type(args) or new [n]T
        if (auto* e = dynamic_cast<const sysp::ast::AllocExpr*>(expr)) {
            MetatronNode node;
            node.id = ctx.next_id++;
            for (auto& arg : e->arguments)
                node.inputs.push_back(process_expr(ctx, arg.get()));
            node.outputs.push_back(node.id);
            ctx.graph.nodes.push_back(node);
            // Register type for type-mismatch checker
            get_node_types()[node.id] = e->alloc_type;
            // Register array bounds for buffer-overflow checker
            if (e->is_array && e->array_size) {
                if (auto* lit = dynamic_cast<const sysp::ast::LiteralExpr*>(e->array_size.get())) {
                    try {
                        int sz = std::stoi(lit->value);
                        get_array_bounds()[node.id] = sz;
                    } catch (...) {}
                }
            }
            return node.id;
        }

        // Ok / Err / Some / None
        if (auto* e = dynamic_cast<const sysp::ast::OkExpr*>(expr))
            return ctx.new_node_with_dep(process_expr(ctx, e->expr.get()));
        if (auto* e = dynamic_cast<const sysp::ast::ErrExpr*>(expr))
            return ctx.new_node_with_dep(process_expr(ctx, e->expr.get()));
        if (auto* e = dynamic_cast<const sysp::ast::SomeExpr*>(expr))
            return ctx.new_node_with_dep(process_expr(ctx, e->expr.get()));
        if (dynamic_cast<const sysp::ast::NoneExpr*>(expr))
            return ctx.new_node();

        // Cast
        if (auto* e = dynamic_cast<const sysp::ast::CastExpr*>(expr))
            return ctx.new_node_with_dep(process_expr(ctx, e->expr.get()));

        // Range
        if (auto* e = dynamic_cast<const sysp::ast::RangeExpr*>(expr)) {
            uint32_t s = process_expr(ctx, e->start.get());
            uint32_t en = process_expr(ctx, e->end.get());
            MetatronNode node;
            node.id = ctx.next_id++;
            node.inputs.push_back(s);
            node.inputs.push_back(en);
            node.outputs.push_back(node.id);
            ctx.graph.nodes.push_back(node);
            return node.id;
        }

        // Await / Recv / Spawn
        if (auto* e = dynamic_cast<const sysp::ast::AwaitExpr*>(expr))
            return ctx.new_node_with_dep(process_expr(ctx, e->task.get()));
        if (auto* e = dynamic_cast<const sysp::ast::RecvExpr*>(expr))
            return ctx.new_node_with_dep(process_expr(ctx, e->channel.get()));
        if (auto* e = dynamic_cast<const sysp::ast::SpawnExpr*>(expr)) {
            uint32_t spawn_node = ctx.new_node_with_dep(process_expr(ctx, e->expr.get()));
            get_spawn_nodes().insert(spawn_node);
            return spawn_node;
        }

        // Channel — register for data-race detection
        if (dynamic_cast<const sysp::ast::ChannelExpr*>(expr)) {
            uint32_t ch_node = ctx.new_node();
            get_channel_nodes().insert(ch_node);
            return ch_node;
        }

        // Lambda
        if (auto* e = dynamic_cast<const sysp::ast::LambdaExpr*>(expr)) {
            uint32_t lambda_node = ctx.new_node();
            if (e->body) process_block(ctx, e->body.get());
            return lambda_node;
        }

        // Interpolated string
        if (dynamic_cast<const sysp::ast::InterpolatedStringExpr*>(expr))
            return ctx.new_node();

        // Drop
        if (auto* e = dynamic_cast<const sysp::ast::DropExpr*>(expr))
            return ctx.new_node_with_dep(process_expr(ctx, e->expr.get()));

        // Tuple access
        if (auto* e = dynamic_cast<const sysp::ast::TupleAccessExpr*>(expr))
            return ctx.new_node_with_dep(process_expr(ctx, e->object.get()));

        // Fallback
        return ctx.new_node();
    }

    // ── Statement processing ──────────────────────────────────────────

    static void process_stmt(BuildContext& ctx, const sysp::ast::Stmt* stmt) {
        if (!stmt) return;

        // var decl: let x = expr
        if (auto* s = dynamic_cast<const sysp::ast::VarDeclStmt*>(stmt)) {
            uint32_t val_node = process_expr(ctx, s->initializer.get());
            for (auto& name : s->names) {
                ctx.symbols.set(name, static_cast<int>(val_node));
                get_node_names()[val_node] = name;
                // Register explicit type if provided
                if (!s->type.empty())
                    get_node_types()[val_node] = s->type;
            }
            return;
        }

        // const decl
        if (auto* s = dynamic_cast<const sysp::ast::ConstDeclStmt*>(stmt)) {
            uint32_t val_node = process_expr(ctx, s->value.get());
            ctx.symbols.set(s->name, static_cast<int>(val_node));
            return;
        }

        // assignment
        if (auto* s = dynamic_cast<const sysp::ast::AssignStmt*>(stmt)) {
            uint32_t rhs = process_expr(ctx, s->value.get());
            // Update symbol if target is identifier
            if (auto* id = dynamic_cast<const sysp::ast::IdentifierExpr*>(s->target.get()))
                ctx.symbols.set(id->name, static_cast<int>(rhs));
            return;
        }

        // expression statement (e.g. println(x))
        if (auto* s = dynamic_cast<const sysp::ast::ExpressionStmt*>(stmt)) {
            process_expr(ctx, s->expression.get());
            return;
        }

        // return
        if (auto* s = dynamic_cast<const sysp::ast::ReturnStmt*>(stmt)) {
            if (s->value) process_expr(ctx, s->value.get());
            ctx.new_node(); // return node
            return;
        }

        // if
        if (auto* s = dynamic_cast<const sysp::ast::IfStmt*>(stmt)) {
            process_expr(ctx, s->condition.get());
            if (s->then_block) process_block(ctx, s->then_block.get());
            if (s->else_stmt)  process_stmt(ctx, s->else_stmt.get());
            return;
        }

        // while
        if (auto* s = dynamic_cast<const sysp::ast::WhileStmt*>(stmt)) {
            process_expr(ctx, s->condition.get());
            if (s->body) process_block(ctx, s->body.get());
            return;
        }

        // loop
        if (auto* s = dynamic_cast<const sysp::ast::LoopStmt*>(stmt)) {
            if (s->body) process_block(ctx, s->body.get());
            return;
        }

        // for
        if (auto* s = dynamic_cast<const sysp::ast::ForStmt*>(stmt)) {
            uint32_t iter_node = process_expr(ctx, s->iterable.get());
            ctx.symbols.set(s->iterator, static_cast<int>(iter_node));
            if (s->body) process_block(ctx, s->body.get());
            return;
        }

        // match
        if (auto* s = dynamic_cast<const sysp::ast::MatchStmt*>(stmt)) {
            process_expr(ctx, s->value.get());
            for (auto& arm : s->arms) {
                if (arm.guard) process_expr(ctx, arm.guard.get());
                if (arm.body)  process_stmt(ctx, arm.body.get());
            }
            return;
        }

        // region — track all nodes created inside as belonging to this region
        if (auto* s = dynamic_cast<const sysp::ast::RegionStmt*>(stmt)) {
            uint32_t region_node = ctx.new_node();
            int region_id = static_cast<int>(region_node);
            ctx.symbols.set(s->name, region_id);
            get_node_region()[region_node] = region_id;
            // Track size before processing body
            size_t before = ctx.graph.nodes.size();
            if (s->body) process_block(ctx, s->body.get());
            // Mark all nodes created inside the region
            for (size_t i = before; i < ctx.graph.nodes.size(); i++) {
                get_node_region()[ctx.graph.nodes[i].id] = region_id;
                // Mark as freed when region closes — for dangling-pointer check
                get_freed_nodes().insert(ctx.graph.nodes[i].id);
            }
            return;
        }

        // unsafe — register all nodes inside as unsafe
        if (auto* s = dynamic_cast<const sysp::ast::UnsafeStmt*>(stmt)) {
            size_t before = ctx.graph.nodes.size();
            if (s->body) process_block(ctx, s->body.get());
            for (size_t i = before; i < ctx.graph.nodes.size(); i++)
                get_unsafe_nodes()[ctx.graph.nodes[i].id] = true;
            return;
        }

        // drop — mark node as freed
        if (auto* s = dynamic_cast<const sysp::ast::DropStmt*>(stmt)) {
            uint32_t drop_node = process_expr(ctx, s->expr.get());
            get_freed_nodes().insert(drop_node);
            // If dropping an identifier, mark the variable node too
            if (auto* id = dynamic_cast<const sysp::ast::IdentifierExpr*>(s->expr.get())) {
                int var_id = ctx.symbols.get(id->name);
                if (var_id >= 0) {
                    get_freed_nodes().insert(static_cast<uint32_t>(var_id));
                    get_node_names()[static_cast<uint32_t>(var_id)] = id->name;
                }
            }
            return;
        }

        // panic
        if (auto* s = dynamic_cast<const sysp::ast::PanicStmt*>(stmt)) {
            process_expr(ctx, s->message.get());
            ctx.new_node();
            return;
        }

        // spawn — register spawn node for data-race detection
        if (auto* s = dynamic_cast<const sysp::ast::SpawnStmt*>(stmt)) {
            uint32_t task_node = process_expr(ctx, s->expr.get());
            ctx.symbols.set(s->name, static_cast<int>(task_node));
            get_spawn_nodes().insert(task_node);
            get_node_names()[task_node] = s->name;
            return;
        }

        // send
        if (auto* s = dynamic_cast<const sysp::ast::SendStmt*>(stmt)) {
            process_expr(ctx, s->value.get());
            return;
        }

        // select
        if (auto* s = dynamic_cast<const sysp::ast::SelectStmt*>(stmt)) {
            for (auto& arm : s->arms) {
                if (arm.send_value) process_expr(ctx, arm.send_value.get());
                if (arm.body)       process_stmt(ctx, arm.body.get());
            }
            return;
        }

        // block
        if (auto* s = dynamic_cast<const sysp::ast::BlockStmt*>(stmt)) {
            process_block(ctx, s);
            return;
        }

        // break / continue — just a node
        ctx.new_node();
    }

    static void process_block(BuildContext& ctx, const sysp::ast::BlockStmt* block) {
        if (!block) return;
        for (auto& stmt : block->statements)
            process_stmt(ctx, stmt.get());
    }

    // ── Declaration processing ────────────────────────────────────────

    static void process_decl(BuildContext& ctx, const sysp::ast::Decl* decl) {
        if (!decl) return;

        // Function
        if (auto* d = dynamic_cast<const sysp::ast::FunctionDecl*>(decl)) {
            uint32_t fn_node = ctx.new_node();
            ctx.symbols.set(d->name, static_cast<int>(fn_node));
            if (d->body) process_block(ctx, d->body.get());
            return;
        }

        // Struct
        if (auto* d = dynamic_cast<const sysp::ast::StructDecl*>(decl)) {
            uint32_t struct_node = ctx.new_node();
            ctx.symbols.set(d->name, static_cast<int>(struct_node));
            return;
        }

        // Enum
        if (auto* d = dynamic_cast<const sysp::ast::EnumDecl*>(decl)) {
            uint32_t enum_node = ctx.new_node();
            ctx.symbols.set(d->name, static_cast<int>(enum_node));
            return;
        }

        // Impl
        if (auto* d = dynamic_cast<const sysp::ast::ImplDecl*>(decl)) {
            for (auto& method : d->methods)
                process_decl(ctx, method.get());
            return;
        }

        // ImplTrait
        if (auto* d = dynamic_cast<const sysp::ast::ImplTraitDecl*>(decl)) {
            for (auto& method : d->methods)
                process_decl(ctx, method.get());
            return;
        }

        // Const
        if (auto* d = dynamic_cast<const sysp::ast::ConstDecl*>(decl)) {
            uint32_t val_node = process_expr(ctx, d->value.get());
            ctx.symbols.set(d->name, static_cast<int>(val_node));
            return;
        }

        // TypeAlias
        if (auto* d = dynamic_cast<const sysp::ast::TypeAliasDecl*>(decl)) {
            ctx.symbols.set(d->name, static_cast<int>(ctx.new_node()));
            return;
        }

        // Trait
        if (auto* d = dynamic_cast<const sysp::ast::TraitDecl*>(decl)) {
            ctx.symbols.set(d->name, static_cast<int>(ctx.new_node()));
            return;
        }
    }

    // ── Main entry point ──────────────────────────────────────────────

    MetatronGraph MetatronBuilder::build_from_program(const sysp::ast::Program& program) {
        MetatronGraph graph;
        BuildContext ctx{ graph, SymbolTable{}, 0 };

        // ── Register built-in functions from modules ──────────────────
        // Any module declared with "module io.println" etc. gets a
        // built-in node registered BEFORE processing declarations.
        // This prevents "use before production" errors for built-ins.
        static const std::unordered_map<std::string, std::string> MODULE_BUILTINS = {
            { "io.println",  "println"  },
            { "io.print",    "print"    },
            { "io.read",     "read"     },
            { "io.fs",       "fs"       },
            { "io.string",   "str"      },
        };

        for (auto& mod : program.modules) {
            auto it = MODULE_BUILTINS.find(mod->path);
            if (it != MODULE_BUILTINS.end()) {
                uint32_t builtin_id = ctx.new_node();
                ctx.symbols.set(it->second, static_cast<int>(builtin_id));
                get_builtin_nodes().insert(builtin_id);
            }
        }

        // ── Process all declarations ──────────────────────────────────
        for (auto& decl : program.declarations)
            process_decl(ctx, decl.get());

        std::cout << "[Metatron] Graph built from AST — "
        << graph.nodes.size() << " nodes ("
        << builtin_nodes.size() << " built-ins)\n";

        return graph;
    }

} // namespace sysp
