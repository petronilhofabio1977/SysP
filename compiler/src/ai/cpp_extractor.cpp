#include "cpp_extractor.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <regex>

namespace sysp::ai {
namespace fs = std::filesystem;

static std::vector<std::string> extrair_funcoes_cpp(const std::string& arquivo) {
    std::ifstream f(arquivo);
    if (!f.is_open()) return {};
    std::stringstream ss;
    ss << f.rdbuf();
    std::string conteudo = ss.str();
    std::vector<std::string> funcoes;
    
    std::regex funcao_regex(R"((\w+(?:\:\:\w+)?)\s+(\w+)\s*\([^)]*\)\s*\{)");
    std::smatch match;
    std::string::const_iterator searchStart(conteudo.cbegin());
    
    while (std::regex_search(searchStart, conteudo.cend(), match, funcao_regex)) {
        funcoes.push_back(match[0]);
        searchStart = match[0].second;
    }
    return funcoes;
}

static std::vector<std::string> extrair_structs_cpp(const std::string& arquivo) {
    std::ifstream f(arquivo);
    if (!f.is_open()) return {};
    std::stringstream ss;
    ss << f.rdbuf();
    std::string conteudo = ss.str();
    std::vector<std::string> structs;
    
    std::regex struct_regex(R"(struct\s+(\w+)\s*\{)");
    std::smatch match;
    std::string::const_iterator searchStart(conteudo.cbegin());
    
    while (std::regex_search(searchStart, conteudo.cend(), match, struct_regex)) {
        structs.push_back(match[0]);
        searchStart = match[0].second;
    }
    return structs;
}

void CppExtractor::extrair_de_arquivo(const std::string& caminho, PatternGraph& pg) {
    auto funcoes = extrair_funcoes_cpp(caminho);
    for (const auto& f : funcoes) {
        std::vector<uint8_t> hex;
        hex.push_back(0x10);
        hex.push_back(0x01);
        for (char c : f) hex.push_back((uint8_t)c);
        pg.registrar(hex);
    }
    
    auto structs = extrair_structs_cpp(caminho);
    for (const auto& s : structs) {
        std::vector<uint8_t> hex;
        hex.push_back(0x10);
        hex.push_back(0x02);
        for (char c : s) hex.push_back((uint8_t)c);
        pg.registrar(hex);
    }
}

void CppExtractor::extrair_da_pasta(const std::string& pasta, PatternGraph& pg) {
    (void)pg; // evitar warning
    std::cout << "[CppExtractor] Extraindo de " << pasta << "\n";
    int total_funcoes = 0, total_structs = 0;
    int arquivos = 0;
    
    for (auto& entry : fs::recursive_directory_iterator(pasta)) {
        if (entry.path().extension() == ".cpp" || 
            entry.path().extension() == ".hpp") {
            arquivos++;
            auto f = extrair_funcoes_cpp(entry.path().string());
            auto s = extrair_structs_cpp(entry.path().string());
            total_funcoes += f.size();
            total_structs += s.size();
        }
    }
    std::cout << "  → " << arquivos << " arquivos\n";
    std::cout << "  → " << total_funcoes << " funcoes\n";
    std::cout << "  → " << total_structs << " structs\n";
}

} // namespace sysp::ai
