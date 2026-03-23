#pragma once
#include <string>
#include <vector>
#include <fstream>

namespace sysp::ai {

class PairCollector {
public:
    void registrar(const std::string& sysp, const std::string& cpp);
    void salvar(const std::string& caminho);

private:
    std::vector<std::pair<std::string, std::string>> pares_;
};

} // namespace sysp::ai
