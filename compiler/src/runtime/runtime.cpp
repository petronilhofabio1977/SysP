#include "runtime.hpp"
#include <cstdint>

// ================================================================
// SysP Runtime — IO via direct Linux syscalls
// No libc dependency — pure syscall interface
// ================================================================

namespace sysp::runtime {

    // ── Linux x86-64 syscall numbers ─────────────────────────────────
    static const int64_t SYS_WRITE = 1;
    static const int64_t SYS_EXIT  = 60;
    static const int64_t STDOUT    = 1;

    // ── Raw syscall write ─────────────────────────────────────────────
    static inline int64_t sys_write(int64_t fd, const char* buf, int64_t len) {
        int64_t ret;
        __asm__ volatile (
            "syscall"
            : "=a"(ret)
            : "0"(SYS_WRITE), "D"(fd), "S"(buf), "d"(len)
            : "rcx", "r11", "memory"
        );
        return ret;
    }

    // ── String length ─────────────────────────────────────────────────
    int64_t str_len(const char* s) {
        int64_t len = 0;
        while (s[len]) len++;
        return len;
    }

    // ── Integer to string ─────────────────────────────────────────────
    void int_to_str(int64_t value, char* buf, int* out_len) {
        if (value == 0) {
            buf[0] = '0';
            *out_len = 1;
            return;
        }

        bool negative = value < 0;
        if (negative) value = -value;

        char tmp[32];
        int  i = 0;

        while (value > 0) {
            tmp[i++] = '0' + (value % 10);
            value /= 10;
        }

        int len = 0;
        if (negative) buf[len++] = '-';
        for (int j = i - 1; j >= 0; j--)
            buf[len++] = tmp[j];

        *out_len = len;
    }

    // ── Float to string ───────────────────────────────────────────────
    void float_to_str(double value, char* buf, int* out_len) {
        int len = 0;

        if (value < 0) {
            buf[len++] = '-';
            value = -value;
        }

        // Integer part
        int64_t int_part = (int64_t)value;
        double  frac     = value - (double)int_part;

        char int_buf[32];
        int  int_len = 0;
        int_to_str(int_part, int_buf, &int_len);
        for (int i = 0; i < int_len; i++)
            buf[len++] = int_buf[i];

        buf[len++] = '.';

        // Fractional part — 6 digits
        for (int d = 0; d < 6; d++) {
            frac *= 10;
            int digit = (int)frac;
            buf[len++] = '0' + digit;
            frac -= digit;
        }

        // Trim trailing zeros
        while (len > 1 && buf[len-1] == '0') len--;
        if (buf[len-1] == '.') {
            buf[len++] = '0'; // keep at least one decimal: 3.0
        }

        *out_len = len;
    }

    // ── println — with newline ────────────────────────────────────────

    void println_string(const char* s, int64_t len) {
        sys_write(STDOUT, s, len);
        sys_write(STDOUT, "\n", 1);
    }

    void println_cstr(const char* s) {
        println_string(s, str_len(s));
    }

    void println_int(int64_t value) {
        char buf[32];
        int  len = 0;
        int_to_str(value, buf, &len);
        sys_write(STDOUT, buf, len);
        sys_write(STDOUT, "\n", 1);
    }

    void println_float(double value) {
        char buf[64];
        int  len = 0;
        float_to_str(value, buf, &len);
        sys_write(STDOUT, buf, len);
        sys_write(STDOUT, "\n", 1);
    }

    void println_bool(bool value) {
        if (value) {
            sys_write(STDOUT, "true\n",  5);
        } else {
            sys_write(STDOUT, "false\n", 6);
        }
    }

    // ── print — without newline ───────────────────────────────────────

    void print_string(const char* s, int64_t len) {
        sys_write(STDOUT, s, len);
    }

    void print_cstr(const char* s) {
        print_string(s, str_len(s));
    }

    void print_int(int64_t value) {
        char buf[32];
        int  len = 0;
        int_to_str(value, buf, &len);
        sys_write(STDOUT, buf, len);
    }

    void print_float(double value) {
        char buf[64];
        int  len = 0;
        float_to_str(value, buf, &len);
        sys_write(STDOUT, buf, len);
    }

    void print_bool(bool value) {
        if (value) sys_write(STDOUT, "true",  4);
        else       sys_write(STDOUT, "false", 5);
    }

    // ── Runtime init ─────────────────────────────────────────────────

    void runtime_init() {
        // Nothing to initialize yet
    }

} // namespace sysp::runtime
