#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "../../frontend/ast/decl.hpp"
#include "../../frontend/ast/expr.hpp"

namespace sysp::typesystem {

enum class TypeKind {
    Unknown, Void,
    I8, I16, I32, I64,
    U8, U16, U32, U64,
    F32, F64, Bool, String,
    Array, Slice, Tuple, Pointer, Reference,
    Result, Option, Task, Channel,
    Struct, Enum, Trait, Function, Lambda,
};

class TypeSystem {
public:
    TypeSystem();
    TypeKind infer_literal(const sysp::ast::LiteralExpr* expr) const;
    TypeKind infer_binary(TypeKind left, TypeKind right, const std::string& op) const;
    TypeKind infer_from_name(const std::string& type_name) const;
    bool is_numeric(TypeKind k)  const;
    bool is_integer(TypeKind k)  const;
    bool is_float(TypeKind k)    const;
    bool is_primitive(TypeKind k) const;
    bool types_compatible(TypeKind a, TypeKind b) const;
    void     set_type(const std::string& name, TypeKind kind);
    TypeKind get_type(const std::string& name) const;
    bool     has_type(const std::string& name) const;
    std::string to_string(TypeKind k) const;
private:
    std::unordered_map<std::string, TypeKind> type_table_;
    std::unordered_map<std::string, TypeKind> builtin_types_;
    void init_builtin_types();
};

} // namespace sysp::typesystem
