#!/usr/bin/env bash

# ==========================================
# SysP Project Integrity Checker
# Verifica integridade e qualidade do código
# ==========================================

PROJECT_ROOT="$(pwd)"

echo "======================================="
echo "SYS P - PROJECT CHECK"
echo "Directory: $PROJECT_ROOT"
echo "======================================="

ERRORS=0
WARNINGS=0

# ------------------------------
# Verificar arquivos vazios
# ------------------------------
echo
echo "Checking empty files..."

EMPTY_FILES=$(find . -type f -empty)

if [ -n "$EMPTY_FILES" ]; then
    echo "WARNING: Empty files detected:"
    echo "$EMPTY_FILES"
    WARNINGS=$((WARNINGS+1))
fi


# ------------------------------
# Procurar TODO esquecidos
# ------------------------------
echo
echo "Checking TODO / FIXME..."

TODO=$(grep -R --line-number "TODO\|FIXME" .)

if [ -n "$TODO" ]; then
    echo "WARNING: TODO found:"
    echo "$TODO"
    WARNINGS=$((WARNINGS+1))
fi


# ------------------------------
# Verificar includes inválidos
# ------------------------------
echo
echo "Checking C includes..."

for file in $(find . -name "*.c"); do

    gcc -fsyntax-only "$file" 2>/tmp/sysp_check.log

    if [ $? -ne 0 ]; then
        echo "ERROR in $file"
        cat /tmp/sysp_check.log
        ERRORS=$((ERRORS+1))
    fi

done


# ------------------------------
# Verificar Rust
# ------------------------------
if [ -f "Cargo.toml" ]; then
    echo
    echo "Checking Rust project..."

    cargo check

    if [ $? -ne 0 ]; then
        echo "ERROR: Rust compilation failed"
        ERRORS=$((ERRORS+1))
    fi
fi


# ------------------------------
# Verificar scripts bash
# ------------------------------
echo
echo "Checking Bash scripts..."

for file in $(find . -name "*.sh"); do

    bash -n "$file"

    if [ $? -ne 0 ]; then
        echo "ERROR in bash script: $file"
        ERRORS=$((ERRORS+1))
    fi

done


# ------------------------------
# Verificar CMake
# ------------------------------
if [ -f "CMakeLists.txt" ]; then

    echo
    echo "Checking CMake..."

    cmake -S . -B build_check >/dev/null 2>&1

    if [ $? -ne 0 ]; then
        echo "ERROR in CMake configuration"
        ERRORS=$((ERRORS+1))
    fi

fi


# ------------------------------
# Procurar arquivos duplicados
# ------------------------------
echo
echo "Checking duplicate filenames..."

find . -type f | sed 's#.*/##' | sort | uniq -d > /tmp/duplicates.txt

if [ -s /tmp/duplicates.txt ]; then
    echo "WARNING duplicate filenames:"
    cat /tmp/duplicates.txt
    WARNINGS=$((WARNINGS+1))
fi


# ------------------------------
# Estatísticas do projeto
# ------------------------------
echo
echo "Project statistics"

FILES=$(find . -type f | wc -l)
LINES=$(wc -l $(find . -name "*.c" -o -name "*.h" -o -name "*.rs" 2>/dev/null) | tail -1)

echo "Total files: $FILES"
echo "Lines of code: $LINES"


# ------------------------------
# Resultado final
# ------------------------------
echo
echo "======================================="
echo "RESULT"
echo "Errors: $ERRORS"
echo "Warnings: $WARNINGS"
echo "======================================="

if [ $ERRORS -eq 0 ]; then
    echo "Project check PASSED"
else
    echo "Project check FAILED"
fi
