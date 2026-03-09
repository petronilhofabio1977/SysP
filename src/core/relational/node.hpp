#pragma once
#include <vector>

template<typename T>
struct Node {

    T value;

    std::vector<Node*> neighbors;

};
