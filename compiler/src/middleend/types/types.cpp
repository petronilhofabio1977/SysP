#include "types.hpp"

namespace sysp::types {

std::shared_ptr<Type> make_primitive(TypeKind kind)
{
    return std::make_shared<Type>(kind);
}

std::shared_ptr<Type> make_pointer(std::shared_ptr<Type> base)
{
    auto t = std::make_shared<Type>(TypeKind::Pointer);
    t->element_type = base;
    return t;
}

std::shared_ptr<Type> make_reference(std::shared_ptr<Type> base)
{
    auto t = std::make_shared<Type>(TypeKind::Reference);
    t->element_type = base;
    return t;
}

std::shared_ptr<Type> make_array(std::shared_ptr<Type> base, size_t size)
{
    auto t = std::make_shared<Type>(TypeKind::Array);
    t->element_type = base;
    t->array_size = size;
    return t;
}

std::shared_ptr<Type> make_slice(std::shared_ptr<Type> base)
{
    auto t = std::make_shared<Type>(TypeKind::Slice);
    t->element_type = base;
    return t;
}

}
