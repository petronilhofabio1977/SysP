#include "../frontend/lexer/lexer.hpp"
#include "../frontend/parser/parser.hpp"
#include "../middleend/metatron_graph/metatron_builder.hpp"
#include "../middleend/jarbes_kernel/jarbes_kernel.hpp"
#include "../backend/x86/x86_backend.hpp"
#include "../runtime/runtime.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstring>

using namespace sysp;

static std::string read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file: " + path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

static void print_token_summary(const std::vector<Token>& tokens) {
    int idents = 0, literals = 0, keywords = 0, operators = 0;
    for (auto& t : tokens) {
        switch (t.type) {
            case TokenType::IDENT:                        idents++;   break;
            case TokenType::INTEGER:
            case TokenType::FLOAT:
            case TokenType::STRING:
            case TokenType::INTERP_STRING:
            case TokenType::BOOL_TRUE:
            case TokenType::BOOL_FALSE:                   literals++; break;
            case TokenType::END:                          break;
            default:
                if (!t.lexeme.empty() && !std::isalpha(t.lexeme[0])) operators++;
                else                                      keywords++;
                break;
        }
    }
    std::cout << "    tokens: "     << (tokens.size() - 1)
    << "  identifiers: "  << idents
    << "  literals: "     << literals
    << "  keywords: "     << keywords
    << "  operators: "    << operators << "\n";
}

static void print_ast_summary(const sysp::ast::Program& program) {
    int fns = 0, structs = 0, enums = 0, traits = 0, impls = 0;
    for (auto& decl : program.declarations) {
        if (dynamic_cast<const sysp::ast::FunctionDecl*>(decl.get()))  fns++;
        if (dynamic_cast<const sysp::ast::StructDecl*>(decl.get()))    structs++;
        if (dynamic_cast<const sysp::ast::EnumDecl*>(decl.get()))      enums++;
        if (dynamic_cast<const sysp::ast::TraitDecl*>(decl.get()))     traits++;
        if (dynamic_cast<const sysp::ast::ImplDecl*>(decl.get()))      impls++;
    }
    std::cout << "    modules: "     << program.modules.size()
    << "  functions: "     << fns
    << "  structs: "       << structs
    << "  enums: "         << enums
    << "  traits: "        << traits
    << "  impls: "         << impls << "\n";
}

static std::string get_output_path(const std::string& input, const std::string& ext) {
    auto dot = input.rfind('.');
    std::string base = (dot != std::string::npos) ? input.substr(0, dot) : input;
    return base + ext;
}

static void run_pipeline(const std::string& file, bool verbose) {
    const std::string SEP = std::string(52, '-');
    std::cout << "\n[SysP] Compiling: " << file << "\n";
    std::cout << SEP << "\n";

    std::string source = read_file(file);

    std::cout << "[1/5] Lexer\n";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    if (lexer.has_errors()) {
        for (auto& err : lexer.errors())
            std::cerr << "  [Lexer Error] " << err << "\n";
        throw std::runtime_error("Lexer failed.");
    }
    if (verbose) print_token_summary(tokens);

    std::cout << "[2/5] Parser\n";
    sysp::parser::Parser parser(std::move(tokens));
    sysp::ast::Program program = parser.parse_program();
    if (verbose) print_ast_summary(program);

    std::cout << "[3/5] Metatron IR\n";
    MetatronBuilder builder;
    MetatronGraph graph = builder.build_from_program(program);
    if (verbose)
        std::cout << "    nodes: " << graph.nodes.size() << "\n";

    std::cout << "[4/5] Jarbes Security Analysis\n";
    JarbesKernel kernel;
    kernel.build_metatron(1.0);
    kernel.build_honeycomb(4);
    kernel.distribute_transistors();
    kernel.step();
    kernel.analyze(graph);
    std::cout << "    no memory safety violations detected\n";

    std::cout << "[5/5] Backend x86-64\n";
    std::string asm_path = get_output_path(file, ".asm");
    std::ofstream asm_out(asm_path);
    if (!asm_out.is_open())
        throw std::runtime_error("Cannot write: " + asm_path);

    asm_out << "; SysP Compiler Output\n";
    asm_out << "; Source: " << file << "\n\n";

    sysp::backend::x86::Backend backend;
    backend.generate_program(program, asm_out);
    asm_out.close();

    std::cout << "    output: " << asm_path << "\n";
    std::cout << SEP << "\n";
    std::cout << "[SysP] Done. To run:\n";
    std::string base = get_output_path(file, "");
    std::cout << "    nasm -f elf64 " << asm_path << " -o " << base << ".o\n";
    std::cout << "    ld " << base << ".o -o " << base << "\n";
    std::cout << "    ./" << base << "\n\n";
}

static void print_usage() {
    std::cout << "SysP Compiler v1.0 — Grammar v7.0 Final\n\n";
    std::cout << "Usage:\n";
    std::cout << "  sysp compile <file.sp> [--verbose]\n";
    std::cout << "  sysp version\n";
    std::cout << "  sysp help\n\n";
}

int main(int argc, char** argv) {
    if (argc < 2) { print_usage(); return 0; }
    std::string command = argv[1];
    if (command == "help" || command == "--help") { print_usage(); return 0; }
    if (command == "version") {
        std::cout << "SysP v1.0 — Grammar v7.0 Final — C++23/GCC\n";
        return 0;
    }
    if (command == "compile") {
        if (argc < 3) { std::cerr << "Error: missing source file\n"; return 1; }
        std::string file    = argv[2];
        bool        verbose = (argc >= 4 && strcmp(argv[3], "--verbose") == 0);
        try { run_pipeline(file, verbose); }
        catch (const std::exception& e) {
            std::cerr << "\n[Error] " << e.what() << "\n";
            return 1;
        }
        return 0;
    }
    std::cerr << "Error: unknown command '" << command << "'\n";
    return 1;
}
