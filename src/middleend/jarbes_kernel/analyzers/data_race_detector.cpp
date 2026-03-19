#include "../core/metatron_graph.hpp"
#include <iostream>

bool detect_data_race(MetatronGraph &graph)
{
    for (size_t i = 0; i < graph.nodes.size(); i++)
    {
        for (size_t j = i + 1; j < graph.nodes.size(); j++)
        {
            for (auto out1 : graph.nodes[i].outputs)
            {
                for (auto out2 : graph.nodes[j].outputs)
                {
                    if (out1 == out2)
                    {
                        std::cout << "[Jarbes] Possible data race between nodes "
                                  << i << " and " << j << std::endl;
                    }
                }
            }
        }
    }

    return true;
}
