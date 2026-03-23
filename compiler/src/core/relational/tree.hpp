#pragma once
#include <vector>

template<typename T>
struct TreeNode {

    T value;

    std::vector<TreeNode*> children;

};
