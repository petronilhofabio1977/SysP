#pragma once

#include <string>
#include <unordered_map>

class SymbolTable {

public:

    int get(const std::string& name);

private:

    std::unordered_map<std::string,int> table;
    int next_slot = 0;

};
