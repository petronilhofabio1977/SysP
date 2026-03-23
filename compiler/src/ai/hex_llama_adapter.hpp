#pragma once
#include "text_to_hex.hpp"
#include <string>
#include <cstdio>
#include <memory>

namespace sysp::ai {

class HexLlamaAdapter {
private:
    TextToHex converter;
    std::string modelo_path;
    
public:
    HexLlamaAdapter(const std::string& path) : modelo_path(path) {}
    
    std::string gerar(const std::string& prompt_texto, int max_tokens = 128) {
        // 1. Converte prompt para hex (seus bytes semânticos)
        auto hex_prompt = converter.converter(prompt_texto);
        
        // 2. Converte hex para string para o llama.cpp
        std::string hex_str;
        for (uint8_t b : hex_prompt) {
            char buf[4];
            snprintf(buf, sizeof(buf), "%02X", b);
            hex_str += buf;
        }
        
        // 3. Prompt com instrução especial
        std::string prompt_llm = "Convert this hex code to SysP:\n" + hex_str + "\nSysP code:\n";
        
        // 4. Chama llama.cpp
        std::string cmd = "~/llama.cpp/build/bin/llama-cli " \
            "-m " + modelo_path + " " \
            "-p \"" + prompt_llm + "\" " \
            "-n " + std::to_string(max_tokens) + " " \
            "-t 4 -c 256 --no-mmap 2>/dev/null";
        
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
    
    // Versão que já recebe hex diretamente
    std::string gerar_hex(const std::vector<uint8_t>& hex_prompt, int max_tokens = 128) {
        std::string hex_str;
        for (uint8_t b : hex_prompt) {
            char buf[4];
            snprintf(buf, sizeof(buf), "%02X", b);
            hex_str += buf;
        }
        
        std::string prompt_llm = "Convert this hex to SysP:\n" + hex_str + "\nSysP:\n";
        
        std::string cmd = "~/llama.cpp/build/bin/llama-cli " \
            "-m " + modelo_path + " " \
            "-p \"" + prompt_llm + "\" " \
            "-n " + std::to_string(max_tokens) + " " \
            "-t 4 -c 256 --no-mmap 2>/dev/null";
        
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
};

} // namespace sysp::ai
