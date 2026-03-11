#!/usr/bin/env bash

echo "=================================================="
echo "SYSP COMPILER AUDIT SYSTEM"
echo "=================================================="

PROJECT_ROOT="$(pwd)"

ERRORS=0
WARNINGS=0

echo
echo "Project root: $PROJECT_ROOT"

####################################################
# Função para verificar módulo
####################################################

check_module() {

    MODULE="$1"
    MODULE_PATH="$2"

    echo
    echo "Checking module: $MODULE"

    if [[ ! -d "$MODULE_PATH" ]]; then
        echo "ERROR: directory missing -> $MODULE_PATH"
        ((ERRORS++))
        return
    fi

    mapfile -t FILES < <(find "$MODULE_PATH" -type f)

    if [[ ${#FILES[@]} -eq 0 ]]; then
        echo "ERROR: module empty"
        ((ERRORS++))
        return
    fi

    for file in "${FILES[@]}"
    do
        if [[ ! -s "$file" ]]; then
            echo "WARNING: empty file -> $file"
            ((WARNINGS++))
        else
            echo "OK -> $file"
        fi
    done
}

####################################################
# 1 Arquitetura do compilador
####################################################

echo
echo "----------------------------------------------"
echo "COMPILER ARCHITECTURE"
echo "----------------------------------------------"

check_module "lexer" "src/frontend/lexer"
check_module "parser" "src/frontend/parser"
check_module "ast" "src/frontend/ast"

check_module "IR" "src/middleend/ir"
check_module "CFG" "src/middleend/cfg"
check_module "types" "src/middleend/types"
check_module "optimizer" "src/middleend/optimizer"

check_module "backend_x86" "src/backend/x86"
check_module "backend_arm" "src/backend/arm"

check_module "runtime" "src/runtime"

check_module "cli" "src/cli"

####################################################
# 2 Verificação de sintaxe
####################################################

echo
echo "----------------------------------------------"
echo "SYNTAX CHECK"
echo "----------------------------------------------"

TMP_LOG="/tmp/sysp_error.log"
> "$TMP_LOG"

while IFS= read -r file
do

    g++ -std=c++17 -fsyntax-only "$file" 2>> "$TMP_LOG"

    if [[ $? -ne 0 ]]; then
        echo
        echo "ERROR in file: $file"
        tail -n 10 "$TMP_LOG"
        ((ERRORS++))
    fi

done < <(find src -name "*.cpp")

####################################################
# 3 Verificar documentação
####################################################

echo
echo "----------------------------------------------"
echo "DOCUMENTATION CHECK"
echo "----------------------------------------------"

if [[ ! -d docs ]]; then
    echo "WARNING: docs directory missing"
    ((WARNINGS++))
else

    while IFS= read -r doc
    do

        if [[ ! -s "$doc" ]]; then
            echo "WARNING empty documentation -> $doc"
            ((WARNINGS++))
        else
            echo "OK documentation -> $doc"
        fi

    done < <(find docs -name "*.md")

fi

####################################################
# 4 Verificar testes
####################################################

echo
echo "----------------------------------------------"
echo "TESTS"
echo "----------------------------------------------"

if [[ ! -d tests ]]; then

    echo "WARNING: tests directory missing"
    ((WARNINGS++))

else

    while IFS= read -r test
    do

        if [[ ! -s "$test" ]]; then
            echo "WARNING empty test -> $test"
            ((WARNINGS++))
        else
            echo "OK test -> $test"
        fi

    done < <(find tests -name "*.cpp")

fi

####################################################
# 5 Estatísticas do projeto
####################################################

echo
echo "----------------------------------------------"
echo "PROJECT STATS"
echo "----------------------------------------------"

FILES=$(find . -type f | wc -l)

CPP=$(find src -name "*.cpp" 2>/dev/null | wc -l)
HPP=$(find src -name "*.hpp" 2>/dev/null | wc -l)

LINES=$(find src \( -name "*.cpp" -o -name "*.hpp" \) -print0 | xargs -0 wc -l 2>/dev/null | tail -n 1)

echo "Total files: $FILES"
echo "CPP files: $CPP"
echo "HPP files: $HPP"
echo "Lines of code: $LINES"

####################################################
# Resultado final
####################################################

echo
echo "=================================================="
echo "SYSP AUDIT RESULT"
echo "=================================================="

echo "Errors: $ERRORS"
echo "Warnings: $WARNINGS"

if [[ $ERRORS -eq 0 ]]; then
    echo
    echo "STATUS: BUILDABLE"
else
    echo
    echo "STATUS: BROKEN"
fi

echo "=================================================="
