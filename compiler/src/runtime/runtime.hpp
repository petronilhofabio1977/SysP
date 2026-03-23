#pragma once

#include <string>
#include <cstdint>

namespace sysp::runtime {

    // ── Initialization ────────────────────────────────────────────────
    void runtime_init();

    // ── IO — println (with newline) ───────────────────────────────────
    void println_string(const char* s, int64_t len);
    void println_cstr(const char* s);
    void println_int(int64_t value);
    void println_float(double value);
    void println_bool(bool value);

    // ── IO — print (without newline) ─────────────────────────────────
    void print_string(const char* s, int64_t len);
    void print_cstr(const char* s);
    void print_int(int64_t value);
    void print_float(double value);
    void print_bool(bool value);

    // ── String helpers ────────────────────────────────────────────────
    int64_t str_len(const char* s);
    void    int_to_str(int64_t value, char* buf, int* out_len);
    void    float_to_str(double value, char* buf, int* out_len);

} // namespace sysp::runtime
