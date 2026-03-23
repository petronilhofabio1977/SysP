#include "type_system.hpp"

namespace sysp::typesystem {

using enum TypeKind;

TypeSystem::TypeSystem() { init_builtin_types(); }

void TypeSystem::init_builtin_types() {
    builtin_types_["i8"]     = TypeKind::I8;
    builtin_types_["i16"]    = TypeKind::I16;
    builtin_types_["i32"]    = TypeKind::I32;
    builtin_types_["i64"]    = TypeKind::I64;
    builtin_types_["u8"]     = TypeKind::U8;
    builtin_types_["u16"]    = TypeKind::U16;
    builtin_types_["u32"]    = TypeKind::U32;
    builtin_types_["u64"]    = TypeKind::U64;
    builtin_types_["f32"]    = TypeKind::F32;
    builtin_types_["f64"]    = TypeKind::F64;
    builtin_types_["bool"]   = TypeKind::Bool;
    builtin_types_["string"] = TypeKind::String;
    builtin_types_["Result"] = TypeKind::Result;
    builtin_types_["Option"] = TypeKind::Option;
    builtin_types_["Task"]   = TypeKind::Task;
    builtin_types_["Channel"]= TypeKind::Channel;
}

TypeKind TypeSystem::infer_literal(const sysp::ast::LiteralExpr* expr) const {
    switch (expr->kind) {
    case sysp::ast::LiteralKind::Int:    return TypeKind::I32;
    case sysp::ast::LiteralKind::Float:  return TypeKind::F64;
    case sysp::ast::LiteralKind::String: return TypeKind::String;
    case sysp::ast::LiteralKind::Bool:   return TypeKind::Bool;
    }
    return TypeKind::Unknown;
}

TypeKind TypeSystem::infer_binary(TypeKind left, TypeKind right, const std::string& op) const {
    if (op=="=="||op=="!="||op=="<"||op==">"||op=="<="||op==">="||op=="&&"||op=="||")
        return TypeKind::Bool;
    if (left==TypeKind::F64||right==TypeKind::F64) return TypeKind::F64;
    if (left==TypeKind::F32||right==TypeKind::F32) return TypeKind::F32;
    if (left==TypeKind::I64||right==TypeKind::I64) return TypeKind::I64;
    if (left==TypeKind::U64||right==TypeKind::U64) return TypeKind::U64;
    return left;
}

TypeKind TypeSystem::infer_from_name(const std::string& n) const {
    auto it = builtin_types_.find(n);
    if (it != builtin_types_.end()) return it->second;
    auto it2 = type_table_.find(n);
    if (it2 != type_table_.end()) return it2->second;
    return TypeKind::Struct;
}

bool TypeSystem::is_numeric(TypeKind k)   const { return is_integer(k)||is_float(k); }
bool TypeSystem::is_integer(TypeKind k)   const {
    return k==TypeKind::I8 ||k==TypeKind::I16||k==TypeKind::I32||k==TypeKind::I64||
           k==TypeKind::U8 ||k==TypeKind::U16||k==TypeKind::U32||k==TypeKind::U64;
}
bool TypeSystem::is_float(TypeKind k)     const { return k==TypeKind::F32||k==TypeKind::F64; }
bool TypeSystem::is_primitive(TypeKind k) const { return is_numeric(k)||k==TypeKind::Bool||k==TypeKind::String; }
bool TypeSystem::types_compatible(TypeKind a, TypeKind b) const {
    if (a==b) return true;
    if (is_integer(a)&&is_integer(b)) return true;
    if (is_float(a)  &&is_float(b))   return true;
    if (a==TypeKind::Unknown||b==TypeKind::Unknown) return true;
    return false;
}
void     TypeSystem::set_type(const std::string& n, TypeKind k) { type_table_[n]=k; }
TypeKind TypeSystem::get_type(const std::string& n) const {
    auto it = type_table_.find(n);
    return it!=type_table_.end() ? it->second : TypeKind::Unknown;
}
bool TypeSystem::has_type(const std::string& n) const { return type_table_.count(n)>0; }

std::string TypeSystem::to_string(TypeKind k) const {
    switch(k){
    case TypeKind::I8:      return "i8";
    case TypeKind::I16:     return "i16";
    case TypeKind::I32:     return "i32";
    case TypeKind::I64:     return "i64";
    case TypeKind::U8:      return "u8";
    case TypeKind::U16:     return "u16";
    case TypeKind::U32:     return "u32";
    case TypeKind::U64:     return "u64";
    case TypeKind::F32:     return "f32";
    case TypeKind::F64:     return "f64";
    case TypeKind::Bool:    return "bool";
    case TypeKind::String:  return "string";
    case TypeKind::Result:  return "Result";
    case TypeKind::Option:  return "Option";
    case TypeKind::Task:    return "Task";
    case TypeKind::Channel: return "Channel";
    case TypeKind::Array:   return "array";
    case TypeKind::Slice:   return "slice";
    case TypeKind::Tuple:   return "tuple";
    case TypeKind::Struct:  return "struct";
    case TypeKind::Enum:    return "enum";
    case TypeKind::Function:return "fn";
    case TypeKind::Void:    return "void";
    default:                return "unknown";
    }
}

} // namespace sysp::typesystem
