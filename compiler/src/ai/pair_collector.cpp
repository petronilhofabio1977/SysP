#include "pair_collector.hpp"
#include <fstream>

namespace sysp::ai {

void PairCollector::registrar(const std::string& sysp, const std::string& cpp) {
    pares_.push_back({sysp, cpp});
}

void PairCollector::salvar(const std::string& caminho) {
    std::ofstream f(caminho);
    for (auto& p : pares_) {
        f << "{\"input\":\"" << p.first << "\",\"output\":\"" << p.second << "\"}\n";
    }
}

} // namespace sysp::ai
