#include "symbol_table.hpp"

int SymbolTable::get(const std::string& name) const {
    auto it = table_.find(name);
    if (it == table_.end()) return -1;
    return it->second;
}
void SymbolTable::set(const std::string& name, int node_id) { table_[name] = node_id; }
bool SymbolTable::has(const std::string& name) const { return table_.count(name) > 0; }
void SymbolTable::remove(const std::string& name) { table_.erase(name); }
void SymbolTable::clear() { table_.clear(); }
int  SymbolTable::size() const { return (int)table_.size(); }
