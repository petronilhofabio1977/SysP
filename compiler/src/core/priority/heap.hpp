#pragma once
#include <vector>
#include <algorithm>

template<typename T>
class Heap {

public:

    void push(T value) {
        data.push_back(value);
        std::push_heap(data.begin(),data.end());
    }

    void pop() {
        std::pop_heap(data.begin(),data.end());
        data.pop_back();
    }

    T top() {
        return data.front();
    }

private:

    std::vector<T> data;

};
