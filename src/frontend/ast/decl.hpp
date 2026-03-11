#pragma once

#include <string>
#include <vector>
#include <memory>

#include "stmt.hpp"

namespace sysp::ast {

struct Decl {
    virtual ~Decl() = default;
};

struct Parameter {

    std::string name;
    std::string type;

    bool is_ref = false;

};

struct StructField {

    std::string name;
    std::string type;

};

struct EnumVariant {

    std::string name;
    std::vector<std::string> fields;

};

struct TraitMethod {

    std::string name;

    std::vector<Parameter> parameters;

    std::string return_type;

};

struct FunctionDecl : Decl {

    std::string name;

    std::vector<std::string> generics;

    std::vector<Parameter> parameters;

    std::string return_type;

    std::unique_ptr<BlockStmt> body;

};

struct StructDecl : Decl {

    std::string name;

    std::vector<StructField> fields;

};

struct EnumDecl : Decl {

    std::string name;

    std::vector<EnumVariant> variants;

};

struct TraitDecl : Decl {

    std::string name;

    std::vector<TraitMethod> methods;

};

}
