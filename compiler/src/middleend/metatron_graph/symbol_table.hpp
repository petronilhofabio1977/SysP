#pragma once

#include <unordered_map>
#include <string>
#include <cstdint>

namespace sysp {

class SymbolTable {

public:

    std::unordered_map<std::string, uint32_t> producers;

    bool exists(const std::string &name)
    {
        return producers.find(name) != producers.end();
    }

    uint32_t get(const std::string &name)
    {
        return producers[name];
    }

    void set(const std::string &name, uint32_t node)
    {
        producers[name] = node;
    }

};

}
