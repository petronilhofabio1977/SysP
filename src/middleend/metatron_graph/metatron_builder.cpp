#include "metatron_builder.hpp"
#include "symbol_table.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>

namespace sysp {

static bool is_identifier(const std::string &s)
{
    if(s.empty()) return false;

    if(!std::isalpha(s[0]) && s[0] != '_')
        return false;

    for(char c : s)
        if(!std::isalnum(c) && c != '_')
            return false;

    return true;
}

MetatronGraph MetatronBuilder::build_from_ast(const std::string &file)
{
    MetatronGraph graph;
    SymbolTable symbols;

    std::ifstream input(file);

    std::string line;
    uint32_t node_id = 0;

    while(std::getline(input, line))
    {
        if(line.find("=") == std::string::npos)
            continue;

        std::stringstream ss(line);

        std::string lhs;
        std::string eq;

        ss >> lhs >> eq;

        MetatronNode node;
        node.id = node_id;

        std::string token;

        while(ss >> token)
        {
            if(is_identifier(token))
            {
                node.inputs.push_back(node_id + 1); 
            }
        }

        node.outputs.push_back(node_id);

        graph.nodes.push_back(node);

        symbols.set(lhs, node_id);

        node_id++;
    }

    return graph;
}

}
