#include "runtime.hpp"

#include <iostream>

namespace sysp::runtime {

void runtime_init()
{
    // runtime initialization placeholder
}

void print_int(int value)
{
    std::cout << value << std::endl;
}

void print_string(const std::string& str)
{
    std::cout << str << std::endl;
}

}
