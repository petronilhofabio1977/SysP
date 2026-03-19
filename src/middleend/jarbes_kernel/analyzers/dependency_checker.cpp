#include "analyzers.hpp"
#include <iostream>

bool check_use_before_production(const MetatronGraph &graph)
{
    bool ok = true;

    for(const auto &node : graph.nodes)
    {
        for(auto input : node.inputs)
        {
            if(input >= node.id)
            {
                std::cerr << "[Jarbes] Error: use before production at node "
                          << node.id << std::endl;

                ok = false;
            }
        }
    }

    return ok;
}
