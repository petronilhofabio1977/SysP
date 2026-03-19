#include "../core/metatron_graph.hpp"
#include <vector>

bool dfs(uint32_t node,
         std::vector<int> &visited,
         std::vector<int> &stack,
         MetatronGraph &graph)
{
    if (!visited[node])
    {
        visited[node] = 1;
        stack[node] = 1;

        for (auto input : graph.nodes[node].inputs)
        {
            if (!visited[input] && dfs(input, visited, stack, graph))
                return true;

            else if (stack[input])
                return true;
        }
    }

    stack[node] = 0;
    return false;
}

bool detect_cycle(MetatronGraph &graph)
{
    size_t n = graph.nodes.size();

    std::vector<int> visited(n, 0);
    std::vector<int> stack(n, 0);

    for (size_t i = 0; i < n; i++)
        if (dfs(i, visited, stack, graph))
            return true;

    return false;
}
