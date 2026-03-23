#pragma once
#include <string>
#include <vector>
#include <cstdio>
#include <array>

namespace sysp::ai {

class LlamaInterface {
private:
    std::string modelo_path;
    FILE* pipe = nullptr;
    
public:
    LlamaInterface(const std::string& path) : modelo_path(path) {}
    
    ~LlamaInterface() {
        if (pipe) pclose(pipe);
    }
    
    // Gera texto usando llama.cpp
    std::string gerar(const std::string& prompt, int max_tokens = 256) {
        std::string cmd = "cd ~/llama.cpp && ./main -m " + modelo_path +
                          " -p \"" + prompt + "\" -n " + std::to_string(max_tokens) +
                          " --temp 0.7 -t 4 2>/dev/null";
        
        pipe = popen(cmd.c_str(), "r");
        if (!pipe) return "";
        
        std::string resultado;
        char buffer[4096];
        while (fgets(buffer, sizeof(buffer), pipe)) {
            resultado += buffer;
        }
        pclose(pipe);
        pipe = nullptr;
        
        return resultado;
    }
};

} // namespace sysp::ai
