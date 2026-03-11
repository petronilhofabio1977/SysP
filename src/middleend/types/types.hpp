#pragma once

#include <string>
#include <memory>

namespace sysp::types {

enum class TypeKind {

    Void,

    I8,
    I16,
    I32,
    I64,

    U8,
    U16,
    U32,
    U64,

    F32,
    F64,

    Bool,
    String,

    Pointer,
    Reference,
    Array,
    Slice,

    Custom

};

struct Type {

    TypeKind kind;

    std::shared_ptr<Type> element_type;

    size_t array_size = 0;

    std::string name;

    Type(TypeKind k)
        : kind(k)
    {
    }

};

std::shared_ptr<Type> make_primitive(TypeKind kind);

std::shared_ptr<Type> make_pointer(std::shared_ptr<Type> base);

std::shared_ptr<Type> make_reference(std::shared_ptr<Type> base);

std::shared_ptr<Type> make_array(std::shared_ptr<Type> base, size_t size);

std::shared_ptr<Type> make_slice(std::shared_ptr<Type> base);

}
