#pragma once
#include <string>
#include "pattern_graph.hpp"

namespace sysp::ai {

class CppExtractor {
public:
    void extrair_da_pasta(const std::string& pasta, PatternGraph& pg);
    void extrair_de_arquivo(const std::string& caminho, PatternGraph& pg);
};

} // namespace sysp::ai
