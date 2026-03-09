#pragma once

#include "stmt.hpp"
#include <string>
#include <vector>

struct FunctionDecl {

    std::string name;

    std::vector<std::string> params;

    BlockStmt* body;
};

struct ModuleDecl {

    std::string name;

    std::vector<FunctionDecl*> functions;
};
