#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>

#include "stmt.hpp"

// ================================================================
// SysP Declaration AST — Grammar v7.0 Final
// ================================================================

namespace sysp::ast {

    // ── Base ─────────────────────────────────────────────────────────

    struct Decl {
        virtual ~Decl() = default;
    };

    using DeclPtr = std::unique_ptr<Decl>;

    // ── Shared building blocks ────────────────────────────────────────

    // Generic parameter with optional trait bounds
    // e.g.: T   T: Ord   T: Ord + Display
    struct GenericParam {
        std::string              name;
        std::vector<std::string> bounds;    // trait names
    };

    // Where clause item: T: Trait1 + Trait2
    struct WhereItem {
        std::string              type_name;
        std::vector<std::string> bounds;
    };

    // Function / method parameter
    // e.g.: self   x i32   p ref i32
    struct Parameter {
        std::string name;
        std::string type;
        bool        is_self = false;
        bool        is_ref  = false;
    };

    // ── Type alias ───────────────────────────────────────────────────

    // [pub] type Name = Type
    struct TypeAliasDecl : Decl {
        bool        is_pub = false;
        std::string name;
        std::string type;
    };

    // ── Constant ─────────────────────────────────────────────────────

    // [pub] const NAME [: Type] = expr
    struct ConstDecl : Decl {
        bool        is_pub = false;
        std::string name;
        std::string type;       // optional
        ExprPtr     value;
    };

    // ── Struct ───────────────────────────────────────────────────────

    struct StructField {
        bool        is_pub = false;
        std::string name;
        std::string type;
    };

    // [pub] struct Name [<T, U: Bound>] [where ...] { fields }
    struct StructDecl : Decl {
        bool                     is_pub = false;
        std::string              name;
        std::vector<GenericParam> generics;
        std::vector<WhereItem>   where_clause;
        std::vector<StructField> fields;
    };

    // ── Enum ─────────────────────────────────────────────────────────

    struct EnumVariant {
        std::string              name;
        std::vector<std::string> fields;    // types for tuple-style variants
    };

    // [pub] enum Name [<T>] { Variant, Variant(T), ... }
    struct EnumDecl : Decl {
        bool                     is_pub = false;
        std::string              name;
        std::vector<GenericParam> generics;
        std::vector<EnumVariant> variants;
    };

    // ── Trait ────────────────────────────────────────────────────────

    // Associated type declaration inside trait
    struct AssocType {
        std::string name;           // e.g.: Item
    };

    // Method signature or method with default implementation
    struct TraitMethod {
        std::string                name;
        std::vector<GenericParam>  generics;
        std::vector<Parameter>     parameters;
        std::string                return_type;
        std::vector<WhereItem>     where_clause;
        bool                       has_default = false;
        std::unique_ptr<BlockStmt> default_body;  // only when has_default
    };

    // trait Name [<T: Bound>] [where ...] { assoc_types methods }
    struct TraitDecl : Decl {
        std::string                name;
        std::vector<GenericParam>  generics;
        std::vector<WhereItem>     where_clause;
        std::vector<AssocType>     assoc_types;
        std::vector<TraitMethod>   methods;
    };

    // ── Function ─────────────────────────────────────────────────────

    // [pub] fn name [<T: Bound>] (params) [-> Type] [where ...] { body }
    struct FunctionDecl : Decl {
        bool                       is_pub = false;
        std::string                name;
        std::vector<GenericParam>  generics;
        std::vector<Parameter>     parameters;
        std::string                return_type;
        std::vector<WhereItem>     where_clause;
        std::unique_ptr<BlockStmt> body;
    };

    // ── Impl ─────────────────────────────────────────────────────────

    // impl Name [<T>] [where ...] { fn methods }
    struct ImplDecl : Decl {
        std::string                        type_name;
        std::vector<GenericParam>          generics;
        std::vector<WhereItem>             where_clause;
        std::vector<std::unique_ptr<FunctionDecl>> methods;
    };

    // impl Trait [<T>] for Type [<T>] [where ...] { fn methods }
    struct ImplTraitDecl : Decl {
        std::string                        trait_name;
        std::vector<GenericParam>          trait_generics;
        std::string                        type_name;
        std::vector<GenericParam>          type_generics;
        std::vector<WhereItem>             where_clause;
        std::vector<std::unique_ptr<FunctionDecl>> methods;
    };

    // ── Module ───────────────────────────────────────────────────────

    // module io.println
    struct ModuleDecl : Decl {
        std::string path;   // full dot-separated path
    };

    // ── Program root ─────────────────────────────────────────────────

    // Top-level: list of module calls + declarations
    struct Program {
        std::vector<std::unique_ptr<ModuleDecl>> modules;
        std::vector<DeclPtr>                     declarations;
    };

} // namespace sysp::ast
