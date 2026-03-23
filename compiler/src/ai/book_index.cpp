#include "book_index.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <vector>
#include <map>
#include <set>

namespace sysp::ai {
namespace fs = std::filesystem;

static std::string to_lower(const std::string& s) {
    std::string r = s;
    for (char& c : r) c = std::tolower(c);
    return r;
}

static std::vector<std::string> tokenizar(const std::string& texto) {
    std::vector<std::string> palavras;
    std::string atual;
    for (char c : texto) {
        if (std::isalnum(c)) {
            atual += std::tolower(c);
        } else {
            if (atual.size() >= 3) {
                palavras.push_back(atual);
            }
            atual.clear();
        }
    }
    if (atual.size() >= 3) palavras.push_back(atual);
    return palavras;
}

static std::string extrair_texto_pdf(const std::string& caminho) {
    std::string cmd = "pdftotext -layout \"" + caminho + "\" - 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";
    std::string resultado;
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        resultado += buffer;
    }
    pclose(pipe);
    return resultado;
}

void BookIndex::indexar(const std::string& pasta) {
    if (!fs::exists(pasta)) {
        std::cout << "[BookIndex] Pasta nao encontrada: " << pasta << "\n";
        return;
    }
    
    std::cout << "[BookIndex] Indexando livros em " << pasta << "\n";
    indice_.clear();
    
    int total_pdfs = 0;
    int total_palavras = 0;
    
    for (auto& entry : fs::directory_iterator(pasta)) {
        if (entry.path().extension() == ".pdf") {
            total_pdfs++;
            std::string nome = entry.path().filename().string();
            std::cout << "  → " << nome << " ... ";
            std::cout.flush();
            
            std::string texto = extrair_texto_pdf(entry.path().string());
            if (texto.empty()) {
                std::cout << "vazio\n";
                continue;
            }
            
            auto palavras = tokenizar(texto);
            total_palavras += palavras.size();
            std::cout << palavras.size() << " palavras\n";
            
            std::map<std::string, int> freq;
            for (const auto& p : palavras) freq[p]++;
            
            for (const auto& [p, count] : freq) {
                indice_[p].push_back({nome, count});
            }
        }
    }
    
    for (auto& [palavra, livros] : indice_) {
        std::sort(livros.begin(), livros.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });
        if (livros.size() > 20) livros.resize(20);
    }
    
    std::cout << "[BookIndex] Indexadas " << indice_.size() 
              << " palavras unicas em " << total_pdfs << " PDFs\n";
    std::cout << "[BookIndex] Total de palavras: " << total_palavras << "\n";
}

std::string BookIndex::buscar(const std::string& query) const {
    if (indice_.empty()) return "[AI] Nenhum livro indexado. Rode: sysp indexar\n";
    
    std::string q = to_lower(query);
    auto palavras = tokenizar(q);
    
    std::map<std::string, int> scores;
    
    for (const auto& p : palavras) {
        auto it = indice_.find(p);
        if (it != indice_.end()) {
            for (const auto& [livro, freq] : it->second) {
                scores[livro] += freq;
            }
        }
    }
    
    if (scores.empty()) return "[AI] Nenhum resultado para: " + query + "\n";
    
    std::vector<std::pair<std::string, int>> ordenado;
    for (const auto& [livro, score] : scores) ordenado.push_back({livro, score});
    std::sort(ordenado.begin(), ordenado.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::string resultado = "[AI] Livros encontrados para \"" + query + "\":\n";
    for (size_t i = 0; i < std::min(ordenado.size(), (size_t)10); i++) {
        resultado += "  " + std::to_string(i+1) + ". " + ordenado[i].first;
        resultado += " (relevância: " + std::to_string(ordenado[i].second) + ")\n";
    }
    return resultado;
}

void BookIndex::salvar(const std::string& caminho) const {
    std::ofstream f(caminho, std::ios::binary);
    if (!f) return;
    size_t n = indice_.size();
    f.write((char*)&n, sizeof(n));
    for (const auto& [palavra, livros] : indice_) {
        size_t tam = palavra.size();
        f.write((char*)&tam, sizeof(tam));
        f.write(palavra.data(), tam);
        size_t l = livros.size();
        f.write((char*)&l, sizeof(l));
        for (const auto& [livro, freq] : livros) {
            size_t lt = livro.size();
            f.write((char*)&lt, sizeof(lt));
            f.write(livro.data(), lt);
            f.write((char*)&freq, sizeof(freq));
        }
    }
    std::cout << "[BookIndex] Salvo em " << caminho << "\n";
}

void BookIndex::carregar(const std::string& caminho) {
    std::ifstream f(caminho, std::ios::binary);
    if (!f) return;
    indice_.clear();
    size_t n = 0;
    f.read((char*)&n, sizeof(n));
    for (size_t i = 0; i < n; i++) {
        size_t tam = 0;
        f.read((char*)&tam, sizeof(tam));
        std::string palavra(tam, '\0');
        f.read(palavra.data(), tam);
        size_t l = 0;
        f.read((char*)&l, sizeof(l));
        std::vector<std::pair<std::string, int>> livros(l);
        for (size_t j = 0; j < l; j++) {
            size_t lt = 0;
            f.read((char*)&lt, sizeof(lt));
            livros[j].first.resize(lt);
            f.read(livros[j].first.data(), lt);
            f.read((char*)&livros[j].second, sizeof(livros[j].second));
        }
        indice_[palavra] = livros;
    }
    std::cout << "[BookIndex] Carregado: " << indice_.size() << " palavras\n";
}

} // namespace sysp::ai
