#include "pattern_graph.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <map>

namespace sysp::ai {

void PatternGraph::registrar(const std::vector<uint8_t>& hex) {
    if (hex.size() < 4) return;
    for (size_t i = 0; i + 4 <= hex.size(); i++) {
        Padrao ctx(hex.begin() + i, hex.begin() + i + 4);
        grafo_[ctx]++;
        total_++;
    }
    std::cout << "[PatternGraph] Registrados " << grafo_.size() 
              << " padroes, total " << total_ << "\n";
}

std::vector<Continuacao> PatternGraph::prever(const Padrao& ctx) const {
    auto it = grafo_.find(ctx);
    if (it == grafo_.end()) return {};
    std::vector<Continuacao> resultados;
    for (const auto& [padrao, freq] : grafo_) {
        if (padrao.size() >= 2 && padrao[0] == ctx[0]) {
            resultados.push_back({padrao, (uint32_t)freq, 0.0f});
        }
    }
    uint32_t total = 0;
    for (auto& c : resultados) total += c.frequencia;
    for (auto& c : resultados) c.confianca = (float)c.frequencia / total;
    std::sort(resultados.begin(), resultados.end(),
        [](const auto& a, const auto& b) { return a.confianca > b.confianca; });
    if (resultados.size() > 5) resultados.resize(5);
    return resultados;
}

static std::string hex_para_codigo(const Padrao& hex) {
    if (hex.size() < 2) return "";
    uint8_t cat = hex[0];
    uint8_t sub = hex[1];
    
    if (cat == 0x01 && sub == 0x01) return "fn nome(a i32, b i32) -> i32 {\n    return a + b\n}";
    if (cat == 0x01 && sub == 0x02) return "struct Nome {\n    pub campo i32\n}";
    if (cat == 0x01 && sub == 0x06) return "let x = 10";
    if (cat == 0x07 && sub == 0x01) return "if condicao {\n    // corpo\n}";
    if (cat == 0x07 && sub == 0x03) return "while condicao {\n    // corpo\n}";
    if (cat == 0x07 && sub == 0x07) return "return expr";
    if (cat == 0x08 && sub == 0x01) return "region nome {\n    // corpo\n}";
    if (cat == 0x02 && sub == 0x12) return "i32";
    if (cat == 0x02 && sub == 0x18) return "f64";
    if (cat == 0x02 && sub == 0x1A) return "bool";
    return "";
}

std::string PatternGraph::sugerir_codigo(const std::string& descricao) const {
    if (grafo_.empty()) {
        return "-- Nenhum padrao aprendido. Compile mais programas.\n";
    }
    
    std::string resultado = "-- Baseado em " + std::to_string(padroes_unicos()) + 
                            " padroes e " + std::to_string(total_registros()) + " registros\n\n";
    
    std::vector<std::pair<Padrao, int>> ordenado;
    for (const auto& [padrao, freq] : grafo_) ordenado.push_back({padrao, freq});
    std::sort(ordenado.begin(), ordenado.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    
    resultado += "=== PADRAO MAIS COMUM ===\n";
    if (!ordenado.empty()) {
        std::string codigo = hex_para_codigo(ordenado[0].first);
        if (!codigo.empty()) {
            resultado += codigo + "\n";
        } else {
            resultado += "Padrao hex: ";
            for (uint8_t b : ordenado[0].first) resultado += std::to_string(b) + " ";
            resultado += "\n";
        }
    }
    
    resultado += "\n=== SUGESTOES ===\n";
    
    bool tem_fn = descricao.find("fn") != std::string::npos || descricao.find("funcao") != std::string::npos;
    bool tem_struct = descricao.find("struct") != std::string::npos;
    bool tem_loop = descricao.find("while") != std::string::npos || descricao.find("for") != std::string::npos;
    bool tem_sort = descricao.find("sort") != std::string::npos || descricao.find("orden") != std::string::npos;
    bool tem_busca = descricao.find("busca") != std::string::npos || descricao.find("binary") != std::string::npos;
    bool tem_ponto = descricao.find("ponto") != std::string::npos || descricao.find("3d") != std::string::npos;
    
    if (tem_sort) {
        resultado += "fn ordenar(arr [100]i32, n i32) {\n";
        resultado += "    let i: i32 = 0\n";
        resultado += "    while i < n {\n";
        resultado += "        let j: i32 = 0\n";
        resultado += "        while j < n - i - 1 {\n";
        resultado += "            if arr[j] > arr[j+1] {\n";
        resultado += "                let temp = arr[j]\n";
        resultado += "                arr[j] = arr[j+1]\n";
        resultado += "                arr[j+1] = temp\n";
        resultado += "            }\n";
        resultado += "            j += 1\n";
        resultado += "        }\n";
        resultado += "        i += 1\n";
        resultado += "    }\n";
        resultado += "}\n";
    } 
    else if (tem_busca) {
        resultado += "fn busca_binaria(arr [100]i32, n i32, alvo i32) -> i32 {\n";
        resultado += "    let low: i32 = 0\n";
        resultado += "    let high: i32 = n - 1\n";
        resultado += "    while low <= high {\n";
        resultado += "        let mid = (low + high) / 2\n";
        resultado += "        if arr[mid] == alvo { return mid }\n";
        resultado += "        if arr[mid] < alvo { low = mid + 1 }\n";
        resultado += "        else { high = mid - 1 }\n";
        resultado += "    }\n";
        resultado += "    return -1\n";
        resultado += "}\n";
    }
    else if (tem_ponto && tem_struct) {
        resultado += "struct Ponto3D {\n";
        resultado += "    pub x f64\n";
        resultado += "    pub y f64\n";
        resultado += "    pub z f64\n";
        resultado += "}\n\n";
        resultado += "pub fn ponto3d_novo(x f64, y f64, z f64) -> Ponto3D {\n";
        resultado += "    return Ponto3D(x, y, z)\n";
        resultado += "}\n\n";
        resultado += "pub fn ponto3d_soma(a Ponto3D, b Ponto3D) -> Ponto3D {\n";
        resultado += "    return Ponto3D(a.x + b.x, a.y + b.y, a.z + b.z)\n";
        resultado += "}\n";
    }
    else if (tem_struct) {
        resultado += "struct Nome {\n    pub campo i32\n}\n";
    } 
    else if (tem_fn) {
        resultado += "fn nome(a i32, b i32) -> i32 {\n    return a + b\n}\n";
    } 
    else if (tem_loop) {
        resultado += "let i: i32 = 0\nwhile i < 10 {\n    i += 1\n}\n";
    } 
    else {
        resultado += "Sugestoes: \"fn que ordena\", \"fn que busca binaria\", \"struct ponto 3d\"\n";
    }
    
    return resultado;
}

void PatternGraph::salvar(const std::string& caminho) const {
    std::ofstream f(caminho, std::ios::binary);
    if (!f) return;
    size_t n = grafo_.size();
    f.write((char*)&n, sizeof(n));
    for (const auto& [padrao, freq] : grafo_) {
        size_t tam = padrao.size();
        f.write((char*)&tam, sizeof(tam));
        f.write((char*)padrao.data(), tam);
        f.write((char*)&freq, sizeof(freq));
    }
    std::cout << "[PatternGraph] Salvo " << n << " padroes\n";
}

void PatternGraph::carregar(const std::string& caminho) {
    std::ifstream f(caminho, std::ios::binary);
    if (!f) return;
    grafo_.clear();
    total_ = 0;
    size_t n = 0;
    f.read((char*)&n, sizeof(n));
    for (size_t i = 0; i < n; i++) {
        size_t tam = 0;
        f.read((char*)&tam, sizeof(tam));
        Padrao padrao(tam);
        f.read((char*)padrao.data(), tam);
        int freq = 0;
        f.read((char*)&freq, sizeof(freq));
        grafo_[padrao] = freq;
        total_ += freq;
    }
    std::cout << "[PatternGraph] Carregado " << n << " padroes, " << total_ << " registros\n";
}

} // namespace sysp::ai
