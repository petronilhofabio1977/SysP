#include "middleend/jarbes_kernel/jarbes_kernel.hpp"
#include "cli.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include "../runtime/runtime.hpp"
#include "../frontend/lexer/lexer.hpp"

namespace sysp::cli {

CLI::CLI()
{}

void CLI::print_help()
{
    std::cout << "SysP Compiler\n";
    std::cout << "Usage:\n";
    std::cout << "  sysp compile <file>\n";
}

std::string read_file(const std::string& path)
{
    std::ifstream file(path);

    if (!file)
    {
        std::cerr << "Error: cannot open file " << path << std::endl;
        exit(1);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

int CLI::compile_file(const std::string& path)
{
    std::cout << "Compiling: " << path << std::endl;

    sysp::runtime::runtime_init();

    std::string source = read_file(path);

    std::cout << "Source size: " << source.size() << " bytes\n";

    std::cout << "Lexer...\n";

    Lexer lexer(source);

    auto tokens = lexer.tokenize();

    std::cout << "Tokens: " << tokens.size() << std::endl;

    std::cout << "Parser...\n";
    std::cout << "AST...\n";
    std::cout << "Metatron IR...\n";
    sysp::JarbesKernel kernel;
    kernel.build_metatron(1.0);
    kernel.build_honeycomb(4);
    kernel.distribute_transistors();
    kernel.step();
    std::cout << "Jarbes Security Analysis..." << std::endl;

    std::cout << "Optimizer...\n";
    std::cout << "Backend...\n";

    std::cout << "Compilation finished\n";

    return 0;
}

int CLI::run(int argc, char** argv)
{
    if (argc < 2)
    {
        print_help();
        return 1;
    }

    std::string command = argv[1];

    if (command == "compile")
    {
        if (argc < 3)
        {
            std::cout << "Missing file\n";
            return 1;
        }

        return compile_file(argv[2]);
    }

    print_help();
    return 1;
}

}
