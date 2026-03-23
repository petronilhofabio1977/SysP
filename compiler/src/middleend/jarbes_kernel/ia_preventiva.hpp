#pragma once
#include "../../ai/pattern_graph.hpp"
#include <vector>
#include <string>
#include <iostream>

namespace sysp::jarbes {

struct ErroConhecido {
    std::string padrao;
    std::string tipo;
    std::string mensagem;
    std::string correcao;
    int frequencia;
};

class IAPreventiva {
private:
    sysp::ai::PatternGraph& pattern_graph;
    std::vector<ErroConhecido> erros_conhecidos;
    
public:
    IAPreventiva(sysp::ai::PatternGraph& pg) : pattern_graph(pg) {
        carregar_erros_dos_livros();
    }
    
    void carregar_erros_dos_livros() {
        erros_conhecidos.push_back({"move", "use-after-move",
            "Você moveu a variável mas tentou usar depois", 
            "Use clone() antes de mover, ou reordene as operações", 0});
            
        erros_conhecidos.push_back({"free", "double-free",
            "A mesma memória foi liberada duas vezes",
            "Use region para liberação automática", 0});
            
        erros_conhecidos.push_back({"[]", "buffer-overflow",
            "Acesso fora dos limites do array",
            "Use slice com verificação de bounds", 0});
    }
    
    std::string detectar_antes(const std::string& codigo) {
        for (auto& erro : erros_conhecidos) {
            if (codigo.find(erro.padrao) != std::string::npos) {
                erro.frequencia++;
                return "[IA PREVENTIVA] " + erro.tipo + "\n  " + 
                       erro.mensagem + "\n  Correção: " + erro.correcao;
            }
        }
        return "";
    }
    
    void aprender_erro(const std::string& codigo, const std::string& tipo, 
                       const std::string& msg) {
        for (auto& e : erros_conhecidos) {
            if (e.tipo == tipo) {
                e.frequencia++;
                return;
            }
        }
        ErroConhecido novo;
        novo.padrao = codigo;
        novo.tipo = tipo;
        novo.mensagem = msg;
        novo.correcao = buscar_correcao(tipo);
        novo.frequencia = 1;
        erros_conhecidos.push_back(novo);
        std::cout << "[IA] Aprendi novo erro: " << tipo << "\n";
    }
    
    std::string sugerir_correcao(const std::string& tipo) {
        for (auto& erro : erros_conhecidos) {
            if (erro.tipo == tipo) {
                return "[IA] " + erro.correcao;
            }
        }
        return "[IA] Nenhuma sugestão disponível";
    }
    
private:
    std::string buscar_correcao(const std::string& tipo) {
        if (tipo == "use-after-move") return "Use clone() antes de mover";
        if (tipo == "double-free") return "Use region para liberação automática";
        if (tipo == "buffer-overflow") return "Use slice com verificação de bounds";
        return "Consulte a documentação";
    }
};

} // namespace sysp::jarbes
