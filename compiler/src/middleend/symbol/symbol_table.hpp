#pragma once
#include <string>
#include <unordered_map>

class SymbolTable {
public:
    int  get(const std::string& name) const;
    void set(const std::string& name, int node_id);
    bool has(const std::string& name) const;
    void remove(const std::string& name);
    void clear();
    int  size() const;
private:
    std::unordered_map<std::string, int> table_;
};
