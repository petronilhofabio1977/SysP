#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace sysp::ai {

class BookIndex {
public:
    void indexar(const std::string& pasta);
    std::string buscar(const std::string& query) const;
    void salvar(const std::string& caminho) const;
    void carregar(const std::string& caminho);
    size_t total_entradas() const { return indice_.size(); }

private:
    // palavra -> lista de (livro, frequencia)
    std::unordered_map<std::string, std::vector<std::pair<std::string, int>>> indice_;
};

} // namespace sysp::ai
