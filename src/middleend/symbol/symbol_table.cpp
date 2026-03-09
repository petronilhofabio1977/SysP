#include "symbol_table.hpp"

int SymbolTable::get(const std::string& name)
{
    if(table.count(name) == 0)
    {
        table[name] = next_slot++;
    }

    return table[name];
}
