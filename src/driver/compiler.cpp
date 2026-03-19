#include "../middleend/jarbes_kernel/jarbes_kernel.hpp"
#include "../middleend/metatron_graph/metatron_builder.hpp"

#include <iostream>
#include <stdexcept>

using namespace sysp;

void run_pipeline(const std::string &file)
{
    std::cout << "Lexer..." << std::endl;
    std::cout << "Parser..." << std::endl;
    std::cout << "AST..." << std::endl;
    std::cout << "Metatron IR..." << std::endl;

    MetatronBuilder builder;
    MetatronGraph graph = builder.build_from_ast(file);

    JarbesKernel kernel;

    kernel.build_metatron(1.0);
    kernel.build_honeycomb(4);
    kernel.distribute_transistors();
    kernel.step();

    std::cout << "Jarbes Security Analysis..." << std::endl;

    kernel.analyze(graph);

    std::cout << "Optimizer..." << std::endl;
    std::cout << "Backend..." << std::endl;
    std::cout << "Compilation finished" << std::endl;
}

int main(int argc, char** argv)
{
    if(argc < 3)
    {
        std::cout << "SysP Compiler\n";
        std::cout << "Usage:\n";
        std::cout << "  sysp compile <file>\n";
        return 1;
    }

    std::string command = argv[1];

    if(command == "compile")
    {
        std::string file = argv[2];

        std::cout << "Compiling: " << file << std::endl;

        try
        {
            run_pipeline(file);
        }
        catch(const std::exception &e)
        {
            std::cerr << "Compilation failed: " << e.what() << std::endl;
            return 1;
        }
    }

    return 0;
}
