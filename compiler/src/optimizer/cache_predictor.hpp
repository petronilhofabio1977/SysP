#pragma once
#include <unordered_map>
#include <string>
#include <iostream>

namespace sysp::optimizer {

class CachePredictor {
private:
    std::unordered_map<std::string, std::string> cache;
    std::unordered_map<std::string, int> frequencia;
    
public:
    bool ja_viu(const std::string& codigo) {
        return cache.find(codigo) != cache.end();
    }
    
    std::string pegar_cache(const std::string& codigo) {
        auto it = cache.find(codigo);
        if (it != cache.end()) {
            std::cout << "[IA] Cache hit! (1000x mais rápido)\n";
            return it->second;
        }
        return "";
    }
    
    void guardar_cache(const std::string& codigo, const std::string& assembly) {
        cache[codigo] = assembly;
        frequencia[codigo]++;
    }
    
    void aprender_padrao(const std::string& codigo) {
        frequencia[codigo]++;
    }
    
    std::string sugerir_otimizacao(const std::string& codigo) {
        for (const auto& [padrao, freq] : frequencia) {
            if (codigo.find(padrao) != std::string::npos && freq > 5) {
                return "[IA] Otimização sugerida (vista " + std::to_string(freq) + "x)";
            }
        }
        return "";
    }
};

} // namespace sysp::optimizer
