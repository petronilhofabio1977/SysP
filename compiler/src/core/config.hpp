#pragma once
#include <string>

// ================================================================
// SysP Compiler Configuration
// ================================================================

namespace sysp::core {

struct Config {
    // ── Version ───────────────────────────────────────────────────
    static constexpr const char* VERSION         = "1.0";
    static constexpr const char* GRAMMAR_VERSION = "7.0 Final";
    static constexpr const char* FILE_EXTENSION  = ".sp";

    // ── Compilation options ───────────────────────────────────────
    bool verbose         = false;
    bool optimize        = true;
    bool debug_info      = false;
    bool warnings_as_errors = false;

    // ── Target ────────────────────────────────────────────────────
    enum class Target { X86_64, ARM64 };
    Target target = Target::X86_64;

    // ── Jarbes security checks ────────────────────────────────────
    bool check_use_before_definition = true;
    bool check_use_after_move        = true;
    bool check_region_escape         = true;
    bool check_cycle                 = true;
    bool check_data_race             = true;

    // ── Output ────────────────────────────────────────────────────
    std::string output_dir  = "";
    std::string output_file = "";

    // ── Module search paths ───────────────────────────────────────
    std::string modules_dir = "modules";

    // ── Limits ───────────────────────────────────────────────────
    int max_errors    = 20;
    int max_warnings  = 100;
    int stack_size    = 8 * 1024 * 1024;  // 8MB default stack
};

// Global compiler config — set once at startup
inline Config& get_config() {
    static Config instance;
    return instance;
}

} // namespace sysp::core
