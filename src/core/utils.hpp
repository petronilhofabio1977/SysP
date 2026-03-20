#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <fstream>

// ================================================================
// SysP Utilities
// General-purpose helpers used across compiler stages
// ================================================================

namespace sysp::core {

// ── String utilities ──────────────────────────────────────────────

inline std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

inline std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string token;
    while (std::getline(ss, token, delim))
        result.push_back(token);
    return result;
}

inline std::string join(const std::vector<std::string>& v, const std::string& sep) {
    std::string result;
    for (size_t i = 0; i < v.size(); i++) {
        if (i > 0) result += sep;
        result += v[i];
    }
    return result;
}

inline bool starts_with(const std::string& s, const std::string& prefix) {
    return s.size() >= prefix.size() && s.substr(0, prefix.size()) == prefix;
}

inline bool ends_with(const std::string& s, const std::string& suffix) {
    return s.size() >= suffix.size() &&
           s.substr(s.size() - suffix.size()) == suffix;
}

inline std::string to_upper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

inline std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

inline std::string repeat(const std::string& s, int n) {
    std::string result;
    for (int i = 0; i < n; i++) result += s;
    return result;
}

// ── File utilities ────────────────────────────────────────────────

inline bool file_exists(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

inline std::string read_file(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return "";
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

inline std::string file_extension(const std::string& path) {
    auto dot = path.rfind('.');
    if (dot == std::string::npos) return "";
    return path.substr(dot);
}

inline std::string file_stem(const std::string& path) {
    auto slash = path.find_last_of("/\\");
    std::string name = (slash == std::string::npos) ? path : path.substr(slash + 1);
    auto dot = name.rfind('.');
    if (dot == std::string::npos) return name;
    return name.substr(0, dot);
}

// ── Number utilities ──────────────────────────────────────────────

inline bool is_integer(const std::string& s) {
    if (s.empty()) return false;
    size_t start = (s[0] == '-') ? 1 : 0;
    if (start == s.size()) return false;
    for (size_t i = start; i < s.size(); i++)
        if (!std::isdigit(s[i])) return false;
    return true;
}

inline bool is_float(const std::string& s) {
    if (s.empty()) return false;
    bool dot = false;
    size_t start = (s[0] == '-') ? 1 : 0;
    for (size_t i = start; i < s.size(); i++) {
        if (s[i] == '.') { if (dot) return false; dot = true; }
        else if (!std::isdigit(s[i])) return false;
    }
    return dot;
}

} // namespace sysp::core
