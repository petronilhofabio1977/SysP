#include "../frontend/lexer/lexer.hpp"
#include "../frontend/parser/parser.hpp"
#include "../middleend/metatron_graph/metatron_builder.hpp"
#include "../middleend/jarbes_kernel/jarbes_kernel.hpp"
#include "../middleend/jarbes_kernel/analyzers/analyzers.hpp"
#include "../middleend/optimizer/optimizer.hpp"
#include "../middleend/typesystem/type_system.hpp"
#include "../backend/x86/x86_backend.hpp"
#include "../ai/hex_emitter.hpp"
#include "../ai/pattern_graph.hpp"
#include "../ai/book_index.hpp"
#include "../ai/cpp_extractor.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstdio>
#include <chrono>
#include <iomanip>

using namespace sysp;
namespace fs = std::filesystem;

static const std::string PATTERN_DB = "sysp_patterns.bin";
static const std::string BOOK_INDEX = "sysp_books.bin";

static sysp::ai::PatternGraph g_patterns;
static bool g_patterns_loaded = false;

static void carregar_patterns() {
    if (!g_patterns_loaded && fs::exists(PATTERN_DB)) {
        g_patterns.carregar(PATTERN_DB);
        std::cout << "[AI] PatternGraph carregado: "
                  << g_patterns.padroes_unicos() << " padroes\n";
    }
    g_patterns_loaded = true;
}

static std::string read_file(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) throw std::runtime_error("Cannot open file: " + path);
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

using Clock = std::chrono::steady_clock;
using Ms    = std::chrono::duration<double, std::milli>;

static double ms_since(Clock::time_point t0) {
    return std::chrono::duration_cast<Ms>(Clock::now() - t0).count();
}

static void run_pipeline(const std::string& file, bool bench_mode = false) {
    auto t_total = Clock::now();
    std::cout << "\n[SysP] Compiling: " << file << "\n";
    std::cout << std::string(52, '-') << "\n";

    std::string source = read_file(file);

    // ── Phase 1: Lexer ───────────────────────────────────────────────
    std::cout << "[1/6] Lexer\n";
    auto t1 = Clock::now();
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    double ms_lexer = ms_since(t1);

    // ── Phase 2: Parser ──────────────────────────────────────────────
    std::cout << "[2/6] Parser\n";
    auto t2 = Clock::now();
    sysp::parser::Parser parser(std::move(tokens));
    sysp::ast::Program program = parser.parse_program();
    double ms_parser = ms_since(t2);

    // ── Phase 3: Type System ─────────────────────────────────────────
    std::cout << "[3/6] Type System\n";
    auto t3 = Clock::now();
    sysp::typesystem::TypeSystem types;
    double ms_types = ms_since(t3);

    // ── Phase 4: Metatron IR ─────────────────────────────────────────
    std::cout << "[4/6] Metatron IR\n";
    auto t4 = Clock::now();
    reset_jarbes_state();
    MetatronBuilder builder;
    MetatronGraph graph = builder.build_from_program(program);
    double ms_metatron = ms_since(t4);

    // ── Phase 5: Jarbes ──────────────────────────────────────────────
    std::cout << "[5/6] Jarbes Security Analysis\n";
    auto t5 = Clock::now();
    JarbesKernel kernel;
    kernel.build_metatron(1.0);
    kernel.build_honeycomb(4);
    kernel.distribute_transistors();
    kernel.step();
    kernel.analyze(graph, program);
    sysp::optimizer::Optimizer optimizer;
    optimizer.run(graph);
    double ms_jarbes = ms_since(t5);

    // ── Phase 6: Backend ─────────────────────────────────────────────
    std::cout << "[6/6] Backend x86-64\n";
    auto t6 = Clock::now();
    std::string asm_path = file.substr(0, file.rfind('.')) + ".asm";
    std::ofstream asm_out(asm_path);
    sysp::backend::x86::Backend backend;
    backend.generate_program(program, asm_out);
    asm_out.close();
    double ms_backend = ms_since(t6);

    // ── AI / PatternGraph ────────────────────────────────────────────
    std::cout << "[AI] Emitindo hex semantico\n";
    auto t_ai = Clock::now();
    sysp::ai::HexEmitter emitter;
    auto hex = emitter.emitir_programa(program);
    carregar_patterns();
    g_patterns.registrar(hex);
    g_patterns.salvar(PATTERN_DB);
    double ms_ai = ms_since(t_ai);

    std::cout << "[AI] PatternGraph: " << g_patterns.padroes_unicos()
              << " padroes | " << g_patterns.total_registros() << " registros\n";

    double ms_total = ms_since(t_total);

    std::string base = file.substr(0, file.rfind('.'));
    std::cout << "    output: " << asm_path << "\n";
    std::cout << std::string(52, '-') << "\n[SysP] Done.\n";

    if (bench_mode) {
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "\n[BENCH] Phase times (ms):\n";
        std::cout << "  Lexer      : " << ms_lexer    << " ms\n";
        std::cout << "  Parser     : " << ms_parser   << " ms\n";
        std::cout << "  TypeSystem : " << ms_types    << " ms\n";
        std::cout << "  MetatronIR : " << ms_metatron << " ms\n";
        std::cout << "  Jarbes     : " << ms_jarbes   << " ms\n";
        std::cout << "  Backend    : " << ms_backend  << " ms\n";
        std::cout << "  PatternGraph: " << ms_ai      << " ms\n";
        std::cout << "  TOTAL      : " << ms_total    << " ms\n";
    }
}

static void print_usage() {
    std::cout << "SysP Compiler v2.0 — IA Integrada\n\n";
    std::cout << "Usage:\n";
    std::cout << "  sysp compile <file.sp>\n";
    std::cout << "  sysp stats\n";
    std::cout << "  sysp sugerir <descricao>\n";
    std::cout << "  sysp indexar <pasta>\n";
    std::cout << "  sysp buscar <query>\n";
    std::cout << "  sysp llm-sugerir <descricao>\n";
    std::cout << "  sysp version\n\n";
}

int main(int argc, char** argv) {
    if (argc < 2) { print_usage(); return 0; }

    std::string cmd = argv[1];

    if (cmd == "help" || cmd == "--help") { print_usage(); return 0; }
    if (cmd == "version") {
        std::cout << "SysP v2.0 — IA Integrada — C++23/GCC\n";
        return 0;
    }

    if (cmd == "compile") {
        if (argc < 3) { std::cerr << "Error: missing source file\n"; return 1; }
        bool bench = (argc >= 4 && std::string(argv[3]) == "--bench");
        try { run_pipeline(argv[2], bench); }
        catch (const std::exception& e) { std::cerr << "\n[Error] " << e.what() << "\n"; return 1; }
        return 0;
    }

    if (cmd == "stats") {
        carregar_patterns();
        std::cout << "\n[SysP AI Stats]\n";
        std::cout << "  PatternGraph padroes unicos: " << g_patterns.padroes_unicos() << "\n";
        std::cout << "  PatternGraph total registros: " << g_patterns.total_registros() << "\n";
        if (fs::exists(BOOK_INDEX)) {
            sysp::ai::BookIndex idx;
            idx.carregar(BOOK_INDEX);
            std::cout << "  BookIndex entradas: " << idx.total_entradas() << "\n";
        }
        return 0;
    }

    if (cmd == "sugerir") {
        if (argc < 3) { std::cerr << "Error: informe a descricao\n"; return 1; }
        carregar_patterns();
        std::string desc;
        for (int i = 2; i < argc; i++) desc += std::string(argv[i]) + " ";
        std::cout << "\n[AI] Sugestao para: " << desc << "\n\n";
        std::cout << g_patterns.sugerir_codigo(desc) << "\n";
        return 0;
    }

    if (cmd == "indexar") {
        std::string pasta = (argc >= 3) ? argv[2] : "../../language/livros";
        sysp::ai::BookIndex idx;
        idx.indexar(pasta);
        idx.salvar(BOOK_INDEX);
        return 0;
    }

    if (cmd == "buscar") {
        if (argc < 3) { std::cerr << "Error: informe a query\n"; return 1; }
        sysp::ai::BookIndex idx;
        if (fs::exists(BOOK_INDEX)) {
            idx.carregar_lazy(BOOK_INDEX); // mmap: only index section loaded
        } else {
            std::cout << "[AI] Indice nao encontrado. Rode: sysp indexar\n";
            return 1;
        }
        std::string query = argv[2];
        for (int i = 3; i < argc; i++) query += " " + std::string(argv[i]);
        std::string resultado = idx.buscar(query);
        std::cout << resultado << "\n";
        return 0;
    }

    // ── llm-sugerir ──────────────────────────────────────────────
    if (cmd == "llm-sugerir") {
        if (argc < 3) {
            std::cerr << "Error: informe a descricao\n";
            return 1;
        }
        std::string desc = argv[2];
        for (int i = 3; i < argc; i++) desc += " " + std::string(argv[i]);
        
        std::cout << "\n[LLM] Gerando codigo para: " << desc << "\n\n";
        
        std::string cmd_llm = "~/llama.cpp/build/bin/llama-cli -m ~/llama.cpp/phi-2.Q4_K_M.gguf -p \"Write SysP code for: " + desc + "\" -n 256 2>/dev/null";
        
        FILE* pipe = popen(cmd_llm.c_str(), "r");
        if (!pipe) {
            std::cerr << "Erro ao executar llama.cpp\n";
            return 1;
        }
        
        char buffer[4096];
        std::string resultado;
        while (fgets(buffer, sizeof(buffer), pipe)) {
            resultado += buffer;
        }
        pclose(pipe);
        
        std::cout << resultado << "\n";
        return 0;
    }

    std::cerr << "Error: unknown command '" << cmd << "'\n";
    print_usage();
    return 1;
}
