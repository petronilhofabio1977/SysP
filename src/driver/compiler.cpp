#include "../frontend/lexer/lexer.hpp"
#include "../frontend/parser/parser.hpp"

#include "../middleend/metatron/metatron_graph.hpp"
#include "../middleend/metatron/ir/ast_to_metatron.hpp"

#include <fstream>
#include <iostream>

std::string read_file(const std::string& path)
{
    std::ifstream file(path);

    return std::string(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );
}

int main(int argc,char** argv)
{

    if(argc < 2)
    {
        std::cout << "usage: sy file.syp\n";
        return 1;
    }

    std::cout << "stage 1: reading file\n";

    std::string source = read_file(argv[1]);

    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    std::cout << "stage 2: lexer done\n";

    Parser parser(tokens);
    auto ast = parser.parse();

    std::cout << "stage 3: parser done\n";

    MetatronGraph graph;

    std::cout << "stage 4: converting to metatron\n";

    ASTToMetatron::convert(ast,graph);

    std::cout << "stage 5: metatron done\n";

    std::cout << "Metatron nodes: " << graph.nodes().size() << std::endl;

    return 0;
}
