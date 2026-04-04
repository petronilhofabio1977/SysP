#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace sysp::ai {

// ================================================================
// BookIndex — indexed full-text search over the book library
//
// File format v3 ("SYSPBX3"):
//
//   [8]  magic  "SYSPBX3\0"
//   [8]  n_words
//   [8]  string_table_offset   (from file start)
//   [8]  data_section_offset   (from file start)
//   --- Hash table (right after 32-byte header) ---
//   n_words × HashEntry { uint64 hash; uint64 str_off; uint64 data_off }
//   sorted by hash ascending — binary-searchable, zero-parse (cast pointer)
//   --- String table (at string_table_offset) ---
//   n_words × { uint16 len; char[len] word }  (same order as hash table)
//   used only for hash-collision verification (rare for FNV-64 + 384k words)
//   --- Data section (at data_section_offset) ---
//   at data_off:
//     uint32  n_books
//     n_books × { uint16 title_len; char[title_len]; int32 freq }
//
// carregar_lazy() flow:
//   1. mmap the whole file  (virtual pages, no physical I/O yet)
//   2. madvise WILLNEED for hash-table region (~6 MB)
//   3. cast hash-table pointer — zero parsing
//   4. store string_table and data_section pointers
//   Total cold time: < 30 ms (dominated by the 6 MB prefetch)
//
// buscar() per query word:
//   1. FNV-64 hash of word
//   2. lower_bound on the in-memory fixed array
//   3. verify word string (pointer dereference)
//   4. read book list from data section
//   Total: < 5 ms
// ================================================================

class BookIndex {
public:
    BookIndex()  = default;
    ~BookIndex();

    void    indexar(const std::string& pasta);
    std::string buscar(const std::string& query) const;
    void    salvar(const std::string& caminho) const;
    void    carregar(const std::string& caminho);       // auto-detects format
    void    carregar_lazy(const std::string& caminho);  // mmap v3 (fast)
    size_t  total_entradas() const;

private:
    // ── v3 hash-table layout (fixed 24 bytes/entry) ──────────────────
    struct HashEntry {
        uint64_t hash;     // FNV-1a 64-bit of lowercase word
        uint64_t str_off;  // byte offset in string table (for collision check)
        uint64_t data_off; // byte offset in data section
    };
    static_assert(sizeof(HashEntry) == 24, "HashEntry must be 24 bytes");

    static uint64_t fnv64(std::string_view s);

    // ── mmap state ───────────────────────────────────────────────────
    void*         mmap_ptr_     = nullptr;
    size_t        mmap_size_    = 0;
    int           mmap_fd_      = -1;

    const HashEntry* hash_table_    = nullptr;  // points into mmap
    uint64_t         n_mmap_words_  = 0;
    const char*      str_table_     = nullptr;  // points into mmap
    const char*      data_section_  = nullptr;  // points into mmap

    // ── legacy in-memory index (v1 / fallback) ───────────────────────
    std::unordered_map<std::string,
                       std::vector<std::pair<std::string,int>>> indice_;

    // ── helpers ─────────────────────────────────────────────────────
    std::vector<std::pair<std::string,int>> read_book_list(const char* ptr) const;
    std::string_view string_table_word(uint64_t str_off) const;
    void close_mmap();
};

} // namespace sysp::ai
