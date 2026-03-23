#pragma once

#include <string>

namespace sysp::cli {

class CLI {

public:

    CLI();

    int run(int argc, char** argv);

private:

    void print_help();

    int compile_file(const std::string& path);

};

}
