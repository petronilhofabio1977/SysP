#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <algorithm>

namespace sysp::ai {

using Padrao = std::vector<uint8_t>;

struct HashPadrao {
    size_t operator()(const Padrao& p) const {
        size_t h = 0;
        for (uint8_t b : p) h = h * 31 + b;
        return h;
    }
};

struct Continuacao {
    Padrao   proximo;
    uint32_t frequencia = 0;
    float    confianca  = 0.0f;
};

class PatternGraph {
public:
    void registrar(const std::vector<uint8_t>& hex);
    std::vector<Continuacao> prever(const Padrao& ctx) const;
    std::string sugerir_codigo(const std::string& descricao) const;
    void salvar(const std::string& caminho) const;
    void carregar(const std::string& caminho);
    size_t padroes_unicos() const { return grafo_.size(); }
    size_t total_registros() const { return total_; }

private:
    std::unordered_map<Padrao, int, HashPadrao> grafo_;
    size_t total_ = 0;
};

} // namespace sysp::ai
