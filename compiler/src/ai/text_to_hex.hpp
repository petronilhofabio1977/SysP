#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace sysp::ai {

class TextToHex {
private:
    // Mapeamento de palavras-chave SysP para bytes hex
    std::unordered_map<std::string, std::vector<uint8_t>> keyword_map;
    
public:
    TextToHex() {
        // Inicializa mapeamento
        keyword_map["fn"] = {0x01, 0x01};
        keyword_map["struct"] = {0x01, 0x02};
        keyword_map["enum"] = {0x01, 0x03};
        keyword_map["trait"] = {0x01, 0x04};
        keyword_map["impl"] = {0x01, 0x05};
        keyword_map["let"] = {0x01, 0x06};
        keyword_map["const"] = {0x01, 0x07};
        
        // Tipos
        keyword_map["i32"] = {0x02, 0x12};
        keyword_map["i64"] = {0x02, 0x13};
        keyword_map["f32"] = {0x02, 0x18};
        keyword_map["f64"] = {0x02, 0x19};
        keyword_map["bool"] = {0x02, 0x1A};
        keyword_map["string"] = {0x02, 0x1B};
        
        // Controle
        keyword_map["if"] = {0x07, 0x01};
        keyword_map["else"] = {0x07, 0x02};
        keyword_map["while"] = {0x07, 0x03};
        keyword_map["for"] = {0x07, 0x04};
        keyword_map["return"] = {0x07, 0x07};
        
        // Memória
        keyword_map["region"] = {0x08, 0x01};
        keyword_map["new"] = {0x08, 0x03};
        keyword_map["move"] = {0x08, 0x04};
    }
    
    std::vector<uint8_t> converter(const std::string& texto) {
        std::vector<uint8_t> hex;
        std::string palavra;
        
        for (size_t i = 0; i < texto.size(); i++) {
            char c = texto[i];
            
            if (std::isalnum(c) || c == '_') {
                palavra += c;
            } else {
                if (!palavra.empty()) {
                    auto it = keyword_map.find(palavra);
                    if (it != keyword_map.end()) {
                        hex.insert(hex.end(), it->second.begin(), it->second.end());
                    } else {
                        // Identificador: hash
                        uint16_t h = 0;
                        for (char ch : palavra) h = h * 31 + (uint8_t)ch;
                        hex.push_back(0xA0);
                        hex.push_back((h >> 8) & 0xFF);
                        hex.push_back(h & 0xFF);
                    }
                    palavra.clear();
                }
                
                // Caractere especial
                switch (c) {
                    case '(': hex.push_back(0x30); hex.push_back(0x01); break;
                    case ')': hex.push_back(0x30); hex.push_back(0x02); break;
                    case '{': hex.push_back(0x30); hex.push_back(0x03); break;
                    case '}': hex.push_back(0x30); hex.push_back(0x04); break;
                    case '=': hex.push_back(0x50); hex.push_back(0x01); break;
                    case '+': hex.push_back(0x50); hex.push_back(0x02); break;
                    case '-': hex.push_back(0x50); hex.push_back(0x03); break;
                    case '*': hex.push_back(0x50); hex.push_back(0x04); break;
                    case '/': hex.push_back(0x50); hex.push_back(0x05); break;
                    case ';': hex.push_back(0x40); hex.push_back(0x01); break;
                    case ',': hex.push_back(0x40); hex.push_back(0x02); break;
                    case ':': hex.push_back(0x40); hex.push_back(0x03); break;
                }
            }
        }
        
        return hex;
    }
    
    std::string hex_para_texto(const std::vector<uint8_t>& hex) {
        std::string texto;
        // Mapeamento inverso
        return texto;
    }
};

} // namespace sysp::ai
