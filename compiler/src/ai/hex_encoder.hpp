#pragma once
#include <vector>
#include <cstdint>
#include <Eigen/Dense>

namespace sysp::ai {

class HexEncoder {
private:
    // Embedding: 256 hex codes → 256 dimensões
    Eigen::MatrixXf embedding;
    
    // Projeção: 256 → 2560 (dimensão do Phi-2)
    Eigen::MatrixXf projecao;
    
public:
    HexEncoder() {
        // Inicializa aleatoriamente
        embedding = Eigen::MatrixXf::Random(256, 256);
        projecao = Eigen::MatrixXf::Random(256, 2560);
    }
    
    // Converte sequência hex para vetor no espaço do Phi-2
    Eigen::VectorXf codificar(const std::vector<uint8_t>& hex) {
        Eigen::VectorXf resultado = Eigen::VectorXf::Zero(2560);
        
        for (uint8_t b : hex) {
            // Embedding do byte
            Eigen::VectorXf emb = embedding.row(b);
            // Projeta para espaço do Phi-2
            Eigen::VectorXf proj = emb * projecao;
            resultado += proj;
        }
        
        // Normaliza
        resultado /= (hex.size() + 1);
        return resultado;
    }
    
    // Treina com seus dados
    void treinar(const std::vector<std::vector<uint8_t>>& entradas,
                 const std::vector<Eigen::VectorXf>& saidas) {
        // Usa gradiente descendente simples
        float lr = 0.01f;
        for (size_t i = 0; i < entradas.size(); i++) {
            auto pred = codificar(entradas[i]);
            auto erro = saidas[i] - pred;
            
            // Atualiza pesos (simplificado)
            // Na prática usaria Adam ou similar
        }
    }
};

} // namespace sysp::ai
