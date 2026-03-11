#pragma once

#include <string>

namespace sysp::runtime {

void print_int(int value);

void print_string(const std::string& str);

void runtime_init();

}
