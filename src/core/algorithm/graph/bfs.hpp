#pragma once
#include <queue>
#include <unordered_set>

template<typename Graph, typename Node>
void bfs(Graph& g, Node start) {

    std::queue<Node> q;
    std::unordered_set<Node> visited;

    q.push(start);
    visited.insert(start);

    while(!q.empty()) {

        Node n = q.front();
        q.pop();

        for(auto& neighbor : g.neighbors(n)) {

            if(!visited.count(neighbor)) {

                visited.insert(neighbor);
                q.push(neighbor);

            }

        }

    }

}
