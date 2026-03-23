#pragma once
#include <string>
#include <vector>
#include <iostream>

// ================================================================
// SysP Error System
// Used by all compiler stages to report errors and warnings
// ================================================================

namespace sysp::core {

enum class ErrorKind {
    // Lexer
    UnexpectedChar,
    UnterminatedString,
    UnterminatedComment,
    // Parser
    UnexpectedToken,
    ExpectedToken,
    // Type System
    TypeMismatch,
    UndefinedType,
    // Jarbes / Memory Safety
    UseBeforeDefinition,
    UseAfterMove,
    RegionEscape,
    DanglingPointer,
    DoubleFree,
    BufferOverflow,
    DataRace,
    CycleDetected,
    // General
    InternalError,
};

struct Error {
    ErrorKind   kind;
    std::string message;
    std::string file;
    int         line   = 0;
    int         column = 0;

    std::string to_string() const {
        std::string loc;
        if (!file.empty()) loc = file + ":" + std::to_string(line) + ":" + std::to_string(column) + ": ";
        return loc + "[Error] " + message;
    }
};

struct Warning {
    std::string message;
    std::string file;
    int         line   = 0;
    int         column = 0;

    std::string to_string() const {
        std::string loc;
        if (!file.empty()) loc = file + ":" + std::to_string(line) + ":" + std::to_string(column) + ": ";
        return loc + "[Warning] " + message;
    }
};

class ErrorReporter {
public:
    void error(ErrorKind kind, const std::string& msg,
               const std::string& file = "", int line = 0, int col = 0) {
        errors_.push_back({ kind, msg, file, line, col });
    }

    void warning(const std::string& msg,
                 const std::string& file = "", int line = 0, int col = 0) {
        warnings_.push_back({ msg, file, line, col });
    }

    bool has_errors()   const { return !errors_.empty(); }
    bool has_warnings() const { return !warnings_.empty(); }
    int  error_count()  const { return (int)errors_.size(); }
    int  warning_count()const { return (int)warnings_.size(); }

    void print_all() const {
        for (auto& w : warnings_) std::cout << w.to_string() << "\n";
        for (auto& e : errors_)   std::cerr << e.to_string() << "\n";
    }

    void clear() { errors_.clear(); warnings_.clear(); }

    const std::vector<Error>&   errors()   const { return errors_; }
    const std::vector<Warning>& warnings() const { return warnings_; }

private:
    std::vector<Error>   errors_;
    std::vector<Warning> warnings_;
};

} // namespace sysp::core
