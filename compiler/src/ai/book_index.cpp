#include "book_index.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <cstdio>
#include <vector>
#include <map>
#include <numeric>

// POSIX mmap
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace sysp::ai {
namespace fs = std::filesystem;

// ── Format constants ─────────────────────────────────────────────────
static constexpr char     MAGIC_V3[8] = {'S','Y','S','P','B','X','3','\0'};
static constexpr uint32_t HDR_SIZE    = 32; // 4 × uint64

// ── FNV-1a 64-bit ────────────────────────────────────────────────────
uint64_t BookIndex::fnv64(std::string_view s) {
    uint64_t h = 14695981039346656037ULL;
    for (unsigned char c : s) {
        h ^= c;
        h *= 1099511628211ULL;
    }
    return h;
}

// ── String utilities ─────────────────────────────────────────────────
static std::string to_lower(const std::string& s) {
    std::string r = s;
    for (char& c : r)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return r;
}

static std::vector<std::string> tokenizar(const std::string& texto) {
    std::vector<std::string> palavras;
    std::string atual;
    for (unsigned char c : texto) {
        if (std::isalnum(c)) {
            atual += static_cast<char>(std::tolower(c));
        } else {
            if (atual.size() >= 3) palavras.push_back(atual);
            atual.clear();
        }
    }
    if (atual.size() >= 3) palavras.push_back(atual);
    return palavras;
}

// ── PDF → text (prefer cached .txt) ─────────────────────────────────
static std::string extrair_texto(const std::string& pdf_path) {
    std::string txt_path = pdf_path.substr(0, pdf_path.rfind('.')) + ".txt";
    if (fs::exists(txt_path)) {
        std::ifstream f(txt_path);
        if (f) { std::ostringstream ss; ss << f.rdbuf(); return ss.str(); }
    }
    std::string cmd = "pdftotext -layout \"" + pdf_path + "\" - 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";
    std::string resultado;
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), pipe)) resultado += buffer;
    pclose(pipe);
    return resultado;
}

// ════════════════════════════════════════════════════════════════════
// indexar
// ════════════════════════════════════════════════════════════════════
void BookIndex::indexar(const std::string& pasta) {
    if (!fs::exists(pasta)) {
        std::cout << "[BookIndex] Pasta nao encontrada: " << pasta << "\n";
        return;
    }
    std::cout << "[BookIndex] Indexando livros em " << pasta << "\n";
    indice_.clear();

    int    total_pdfs    = 0;
    size_t total_palavras = 0;

    for (auto& entry : fs::directory_iterator(pasta)) {
        if (entry.path().extension() != ".pdf") continue;
        total_pdfs++;
        std::string nome = entry.path().filename().string();
        std::cout << "  → " << nome << " ... ";
        std::cout.flush();

        std::string texto = extrair_texto(entry.path().string());
        if (texto.empty()) { std::cout << "vazio\n"; continue; }

        auto palavras = tokenizar(texto);
        total_palavras += palavras.size();
        std::cout << palavras.size() << " palavras\n";

        std::map<std::string,int> freq;
        for (const auto& p : palavras) freq[p]++;
        for (const auto& [p, count] : freq)
            indice_[p].push_back({nome, count});
    }

    for (auto& [palavra, livros] : indice_) {
        std::sort(livros.begin(), livros.end(),
            [](const auto& a, const auto& b){ return a.second > b.second; });
        if (livros.size() > 20) livros.resize(20);
    }

    std::cout << "[BookIndex] Indexadas " << indice_.size()
              << " palavras unicas em " << total_pdfs << " PDFs\n";
    std::cout << "[BookIndex] Total de palavras: " << total_palavras << "\n";
}

// ════════════════════════════════════════════════════════════════════
// salvar — v3 format
// ════════════════════════════════════════════════════════════════════
void BookIndex::salvar(const std::string& caminho) const {
    // ── 1. sort words, compute hashes ────────────────────────────────
    std::vector<std::string> words;
    words.reserve(indice_.size());
    for (const auto& [w, _] : indice_) words.push_back(w);

    // sort by hash for the hash table
    std::vector<size_t> order(words.size());
    std::iota(order.begin(), order.end(), 0);
    std::vector<uint64_t> hashes(words.size());
    for (size_t i = 0; i < words.size(); i++) hashes[i] = fnv64(words[i]);
    std::sort(order.begin(), order.end(),
        [&](size_t a, size_t b){ return hashes[a] < hashes[b]; });

    // ── 2. build data section ────────────────────────────────────────
    std::string data_buf;
    data_buf.reserve(52 * 1024 * 1024);

    std::vector<uint64_t> data_offsets(words.size());
    for (size_t idx : order) {
        data_offsets[idx] = data_buf.size();
        const auto& livros = indice_.at(words[idx]);
        uint32_t n = static_cast<uint32_t>(livros.size());
        data_buf.append(reinterpret_cast<const char*>(&n), 4);
        for (const auto& [titulo, freq] : livros) {
            uint16_t tlen = static_cast<uint16_t>(titulo.size());
            data_buf.append(reinterpret_cast<const char*>(&tlen), 2);
            data_buf.append(titulo);
            int32_t f = freq;
            data_buf.append(reinterpret_cast<const char*>(&f), 4);
        }
    }

    // ── 3. build string table (same order as hash table) ────────────
    std::string str_buf;
    str_buf.reserve(words.size() * 10);
    std::vector<uint64_t> str_offsets(words.size());
    for (size_t rank = 0; rank < order.size(); rank++) {
        size_t idx = order[rank];
        str_offsets[rank] = str_buf.size();
        uint16_t wlen = static_cast<uint16_t>(words[idx].size());
        str_buf.append(reinterpret_cast<const char*>(&wlen), 2);
        str_buf.append(words[idx]);
    }

    // ── 4. build hash table (fixed 24 bytes/entry) ───────────────────
    std::string ht_buf;
    ht_buf.reserve(order.size() * 24);
    for (size_t rank = 0; rank < order.size(); rank++) {
        size_t idx = order[rank];
        uint64_t h  = hashes[idx];
        uint64_t so = str_offsets[rank];
        uint64_t do_ = data_offsets[idx];
        ht_buf.append(reinterpret_cast<const char*>(&h),  8);
        ht_buf.append(reinterpret_cast<const char*>(&so), 8);
        ht_buf.append(reinterpret_cast<const char*>(&do_),8);
    }

    // ── 5. compute section offsets ───────────────────────────────────
    uint64_t n_words         = words.size();
    uint64_t str_tbl_offset  = HDR_SIZE + ht_buf.size();
    uint64_t data_offset     = str_tbl_offset + str_buf.size();

    // ── 6. write file ────────────────────────────────────────────────
    std::ofstream f(caminho, std::ios::binary);
    if (!f) { std::cerr << "[BookIndex] Erro ao escrever: " << caminho << "\n"; return; }

    f.write(MAGIC_V3, 8);
    f.write(reinterpret_cast<const char*>(&n_words),        8);
    f.write(reinterpret_cast<const char*>(&str_tbl_offset), 8);
    f.write(reinterpret_cast<const char*>(&data_offset),    8);
    f.write(ht_buf.data(),  ht_buf.size());
    f.write(str_buf.data(), str_buf.size());
    f.write(data_buf.data(),data_buf.size());

    std::cout << "[BookIndex] Salvo em " << caminho << " (v3)"
              << "  hash-table " << ht_buf.size()/1024 << " KB"
              << "  +  string-table " << str_buf.size()/1024 << " KB"
              << "  +  data " << data_buf.size()/(1024*1024) << " MB\n";
}

// ════════════════════════════════════════════════════════════════════
// mmap helpers
// ════════════════════════════════════════════════════════════════════
void BookIndex::close_mmap() {
    if (mmap_ptr_ && mmap_ptr_ != MAP_FAILED) {
        munmap(mmap_ptr_, mmap_size_);
        mmap_ptr_ = nullptr;
    }
    if (mmap_fd_ >= 0) { ::close(mmap_fd_); mmap_fd_ = -1; }
    hash_table_   = nullptr;
    str_table_    = nullptr;
    data_section_ = nullptr;
    n_mmap_words_ = 0;
}

BookIndex::~BookIndex() { close_mmap(); }

std::string_view BookIndex::string_table_word(uint64_t str_off) const {
    const char* p = str_table_ + str_off;
    uint16_t len = 0;
    std::memcpy(&len, p, 2);
    return {p + 2, len};
}

// ════════════════════════════════════════════════════════════════════
// carregar_lazy — mmap v3, zero-parse hash table
// ════════════════════════════════════════════════════════════════════
void BookIndex::carregar_lazy(const std::string& caminho) {
    close_mmap();

    mmap_fd_ = ::open(caminho.c_str(), O_RDONLY);
    if (mmap_fd_ < 0) {
        std::cerr << "[BookIndex] Erro ao abrir: " << caminho << "\n"; return;
    }
    struct stat st{};
    ::fstat(mmap_fd_, &st);
    mmap_size_ = static_cast<size_t>(st.st_size);

    mmap_ptr_ = ::mmap(nullptr, mmap_size_, PROT_READ, MAP_PRIVATE, mmap_fd_, 0);
    if (mmap_ptr_ == MAP_FAILED) {
        ::close(mmap_fd_); mmap_fd_ = -1; mmap_ptr_ = nullptr;
        std::cerr << "[BookIndex] mmap falhou\n"; return;
    }

    const char* p = static_cast<const char*>(mmap_ptr_);

    // validate magic
    if (mmap_size_ < HDR_SIZE || std::memcmp(p, MAGIC_V3, 8) != 0) {
        // fall through to legacy load
        close_mmap();
        carregar(caminho);
        return;
    }

    uint64_t n_words       = 0;
    uint64_t str_tbl_off   = 0;
    uint64_t data_off      = 0;
    std::memcpy(&n_words,     p + 8,  8);
    std::memcpy(&str_tbl_off, p + 16, 8);
    std::memcpy(&data_off,    p + 24, 8);

    // set pointers directly into mmap — zero parsing
    hash_table_   = reinterpret_cast<const HashEntry*>(p + HDR_SIZE);
    str_table_    = p + str_tbl_off;
    data_section_ = p + data_off;
    n_mmap_words_ = n_words;

    // advise: prefetch hash table (small, ~9 MB) sequentially; rest on-demand
    size_t ht_size = n_words * sizeof(HashEntry);
    ::madvise(const_cast<HashEntry*>(hash_table_), ht_size, MADV_WILLNEED);
    ::madvise(const_cast<char*>(data_section_),
              mmap_size_ - data_off, MADV_RANDOM);

    std::cout << "[BookIndex] Carregado lazy v3: " << n_words
              << " palavras (mmap " << mmap_size_/(1024*1024) << " MB)\n";
}

// ════════════════════════════════════════════════════════════════════
// carregar — auto-detect v3 / v1 legacy
// ════════════════════════════════════════════════════════════════════
void BookIndex::carregar(const std::string& caminho) {
    // peek magic
    {
        std::ifstream f(caminho, std::ios::binary);
        if (!f) return;
        char magic[8] = {};
        f.read(magic, 8);
        if (std::memcmp(magic, MAGIC_V3, 8) == 0) {
            f.close();
            carregar_lazy(caminho);
            return;
        }
    }
    // v1 legacy full-load
    std::ifstream f(caminho, std::ios::binary);
    if (!f) return;
    indice_.clear();
    size_t n = 0;
    f.read(reinterpret_cast<char*>(&n), sizeof(n));
    for (size_t i = 0; i < n; i++) {
        size_t tam = 0;
        f.read(reinterpret_cast<char*>(&tam), sizeof(tam));
        std::string palavra(tam, '\0');
        f.read(palavra.data(), tam);
        size_t l = 0;
        f.read(reinterpret_cast<char*>(&l), sizeof(l));
        std::vector<std::pair<std::string,int>> livros(l);
        for (size_t j = 0; j < l; j++) {
            size_t lt = 0;
            f.read(reinterpret_cast<char*>(&lt), sizeof(lt));
            livros[j].first.resize(lt);
            f.read(livros[j].first.data(), lt);
            f.read(reinterpret_cast<char*>(&livros[j].second), sizeof(int));
        }
        indice_[palavra] = std::move(livros);
    }
    std::cout << "[BookIndex] Carregado (v1): " << indice_.size() << " palavras\n";
}

// ════════════════════════════════════════════════════════════════════
// read_book_list
// ════════════════════════════════════════════════════════════════════
std::vector<std::pair<std::string,int>>
BookIndex::read_book_list(const char* ptr) const {
    uint32_t n = 0;
    std::memcpy(&n, ptr, 4); ptr += 4;
    std::vector<std::pair<std::string,int>> result;
    result.reserve(n);
    for (uint32_t i = 0; i < n; i++) {
        uint16_t tlen = 0;
        std::memcpy(&tlen, ptr, 2); ptr += 2;
        std::string title(ptr, tlen); ptr += tlen;
        int32_t freq = 0;
        std::memcpy(&freq, ptr, 4); ptr += 4;
        result.push_back({std::move(title), freq});
    }
    return result;
}

// ════════════════════════════════════════════════════════════════════
// buscar
// ════════════════════════════════════════════════════════════════════
std::string BookIndex::buscar(const std::string& query) const {
    auto palavras = tokenizar(to_lower(query));
    if (palavras.empty()) return "[AI] Query vazia.\n";

    std::map<std::string,int> scores;

    if (hash_table_ && n_mmap_words_ > 0) {
        // ── Fast mmap v3 path ─────────────────────────────────────────
        for (const auto& word : palavras) {
            uint64_t h = fnv64(word);

            // binary search on fixed-size array — all in L3 cache after WILLNEED
            size_t lo = 0, hi = n_mmap_words_;
            while (lo < hi) {
                size_t mid = (lo + hi) / 2;
                if (hash_table_[mid].hash < h)       lo = mid + 1;
                else if (hash_table_[mid].hash > h)  hi = mid;
                else {
                    // found — verify string (collision guard)
                    if (string_table_word(hash_table_[mid].str_off) == word) {
                        auto livros = read_book_list(
                            data_section_ + hash_table_[mid].data_off);
                        for (const auto& [livro, freq] : livros)
                            scores[livro] += freq;
                    }
                    break;
                }
            }
        }
    } else if (!indice_.empty()) {
        // ── Legacy path ───────────────────────────────────────────────
        for (const auto& p : palavras) {
            auto it = indice_.find(p);
            if (it == indice_.end()) continue;
            for (const auto& [livro, freq] : it->second)
                scores[livro] += freq;
        }
    } else {
        return "[AI] Nenhum livro indexado. Rode: sysp indexar\n";
    }

    if (scores.empty()) return "[AI] Nenhum resultado para: " + query + "\n";

    std::vector<std::pair<std::string,int>> ordenado(scores.begin(), scores.end());
    std::sort(ordenado.begin(), ordenado.end(),
        [](const auto& a, const auto& b){ return a.second > b.second; });

    std::string resultado = "[AI] Livros encontrados para \"" + query + "\":\n";
    size_t lim = std::min(ordenado.size(), (size_t)10);
    for (size_t i = 0; i < lim; i++) {
        resultado += "  " + std::to_string(i+1) + ". " + ordenado[i].first
                  + " (relevância: " + std::to_string(ordenado[i].second) + ")\n";
    }
    return resultado;
}

// ════════════════════════════════════════════════════════════════════
// total_entradas
// ════════════════════════════════════════════════════════════════════
size_t BookIndex::total_entradas() const {
    if (n_mmap_words_ > 0) return n_mmap_words_;
    return indice_.size();
}

} // namespace sysp::ai
