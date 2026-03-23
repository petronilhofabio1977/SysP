#pragma once
#include <vector>
#include <algorithm>

namespace sysp::core::algorithm {

template<typename T>
void swap(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}

template<typename T>
int partition(std::vector<T>& arr, int low, int high) {
    T pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i+1], arr[high]);
    return i + 1;
}

template<typename T>
void quicksort(std::vector<T>& arr, int low, int high) {
    if (low < high) {
        int p = partition(arr, low, high);
        quicksort(arr, low, p - 1);
        quicksort(arr, p + 1, high);
    }
}

} // namespace sysp::core::algorithm
