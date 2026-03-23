#pragma once

#include <vector>
#include <unordered_map>

template<typename T>
class Graph {

public:

    void add_node(T id) {
        adjacency[id];
    }

    void add_edge(T a, T b) {
        adjacency[a].push_back(b);
    }

    const std::vector<T>& neighbors(T node) {
        return adjacency[node];
    }

private:

    std::unordered_map<T,std::vector<T>> adjacency;

};
