#pragma once
#include <vector>

namespace sysp::core::algorithm {

template<typename T>
int binary_search(const std::vector<T>& arr, const T& target) {
    int low = 0, high = (int)arr.size() - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (arr[mid] == target) return mid;
        if (arr[mid] < target)  low  = mid + 1;
        else                    high = mid - 1;
    }
    return -1;
}

template<typename T>
int linear_search(const std::vector<T>& arr, const T& target) {
    for (int i = 0; i < (int)arr.size(); i++)
        if (arr[i] == target) return i;
    return -1;
}

} // namespace sysp::core::algorithm
