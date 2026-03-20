#include "../frontend/lexer/lexer.hpp"
#include "../frontend/parser/parser.hpp"
#include "../middleend/metatron_graph/metatron_builder.hpp"
#include "../middleend/jarbes_kernel/jarbes_kernel.hpp"
#include "../middleend/optimizer/optimizer.hpp"
#include "../middleend/typesystem/type_system.hpp"
#include "../backend/x86/x86_backend.hpp"
#include "../runtime/runtime.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstring>

using namespace sysp;

static std::string read_file(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) throw std::runtime_error("Cannot open file: " + path);
    std::stringstream ss; ss << f.rdbuf(); return ss.str();
}

static std::string get_out(const std::string& in, const std::string& ext) {
    auto dot = in.rfind('.');
    return (dot != std::string::npos ? in.substr(0, dot) : in) + ext;
}

static void run_pipeline(const std::string& file, bool verbose) {
    const std::string SEP(52,'-');
    std::cout << "\n[SysP] Compiling: " << file << "\n" << SEP << "\n";

    std::string source = read_file(file);

    std::cout << "[1/6] Lexer\n";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    if (lexer.has_errors()) {
        for (auto& e : lexer.errors()) std::cerr << "  [Lexer Error] " << e << "\n";
        throw std::runtime_error("Lexer failed.");
    }

    std::cout << "[2/6] Parser\n";
    sysp::parser::Parser parser(std::move(tokens));
    sysp::ast::Program program = parser.parse_program();

    std::cout << "[3/6] Type System\n";
    sysp::typesystem::TypeSystem types;
    for (auto& decl : program.declarations) {
        if (auto* fn = dynamic_cast<const sysp::ast::FunctionDecl*>(decl.get()))
            if (!fn->return_type.empty())
                types.set_type(fn->name, types.infer_from_name(fn->return_type));
    }

    std::cout << "[4/6] Metatron IR\n";
    MetatronBuilder builder;
    MetatronGraph graph = builder.build_from_program(program);

    std::cout << "[5/6] Jarbes Security Analysis\n";
    JarbesKernel kernel;
    kernel.build_metatron(1.0);
    kernel.build_honeycomb(4);
    kernel.distribute_transistors();
    kernel.step();
    kernel.analyze(graph, program);  // new interface with full Program

    sysp::optimizer::Optimizer optimizer;
    optimizer.run(graph);

    std::cout << "[6/6] Backend x86-64\n";
    std::string asm_path = get_out(file, ".asm");
    std::ofstream asm_out(asm_path);
    if (!asm_out.is_open()) throw std::runtime_error("Cannot write: " + asm_path);
    asm_out << "; SysP Compiler Output — Grammar v7.0 Final\n";
    asm_out << "; Source: " << file << "\n\n";
    sysp::backend::x86::Backend backend;
    backend.generate_program(program, asm_out);
    asm_out.close();

    std::string base = get_out(file, "");
    std::cout << "    output: " << asm_path << "\n";
    std::cout << SEP << "\n[SysP] Done. To run:\n";
    std::cout << "    nasm -f elf64 " << asm_path << " -o " << base << ".o\n";
    std::cout << "    ld " << base << ".o -o " << base << "\n";
    std::cout << "    ./" << base << "\n\n";
}

static void print_usage() {
    std::cout << "SysP Compiler v1.0 — Grammar v7.0 Final\n\n";
    std::cout << "Usage:\n";
    std::cout << "  sysp compile <file.sp> [--verbose]\n";
    std::cout << "  sysp version\n\n";
}

int main(int argc, char** argv) {
    if (argc < 2) { print_usage(); return 0; }
    std::string cmd = argv[1];
    if (cmd=="help"||cmd=="--help") { print_usage(); return 0; }
    if (cmd=="version") { std::cout << "SysP v1.0 — Grammar v7.0 Final — C++23/GCC\n"; return 0; }
    if (cmd=="compile") {
        if (argc < 3) { std::cerr << "Error: missing source file\n"; return 1; }
        bool verbose = (argc>=4 && strcmp(argv[3],"--verbose")==0);
        (void)verbose;
        try { run_pipeline(argv[2], false); }
        catch (const std::exception& e) { std::cerr << "\n[Error] " << e.what() << "\n"; return 1; }
        return 0;
    }
    std::cerr << "Error: unknown command '" << cmd << "'\n";
    return 1;
}
