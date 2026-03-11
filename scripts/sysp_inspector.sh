#!/usr/bin/env bash

echo "==============================================="
echo "SYS P COMPILER ARCHITECTURE INSPECTOR"
echo "==============================================="

ROOT=$(pwd)

echo
echo "Project root:"
echo "$ROOT"

echo
echo "-----------------------------------------------"
echo "PROJECT SIZE"
echo "-----------------------------------------------"

FILES=$(find src -type f | wc -l)
CPP=$(find src -name "*.cpp" | wc -l)
HPP=$(find src -name "*.hpp" | wc -l)
LINES=$(find src \( -name "*.cpp" -o -name "*.hpp" \) | xargs wc -l | tail -1)

echo "files: $FILES"
echo "cpp files: $CPP"
echo "hpp files: $HPP"
echo "lines: $LINES"

echo
echo "-----------------------------------------------"
echo "MODULE STRUCTURE"
echo "-----------------------------------------------"

for dir in src/* ; do
    if [ -d "$dir" ]; then
        COUNT=$(find "$dir" -type f | wc -l)
        echo "$(basename $dir) : $COUNT files"
    fi
done

echo
echo "-----------------------------------------------"
echo "FRONTEND DEPENDENCIES"
echo "-----------------------------------------------"

grep -R "#include" src/frontend | sed 's/.*#include //' | sort | uniq

echo
echo "-----------------------------------------------"
echo "MIDDLEEND DEPENDENCIES"
echo "-----------------------------------------------"

grep -R "#include" src/middleend | sed 's/.*#include //' | sort | uniq

echo
echo "-----------------------------------------------"
echo "BACKEND DEPENDENCIES"
echo "-----------------------------------------------"

grep -R "#include" src/backend | sed 's/.*#include //' | sort | uniq

echo
echo "-----------------------------------------------"
echo "EMPTY FILES"
echo "-----------------------------------------------"

find src -type f -empty

echo
echo "-----------------------------------------------"
echo "PIPELINE DETECTED"
echo "-----------------------------------------------"

echo "source"
echo "  ↓"
echo "lexer"
echo "  ↓"
echo "parser"
echo "  ↓"
echo "AST"

if [ -d "src/middleend/ast_to_metatron" ]; then
    echo "  ↓"
    echo "AST → METATRON"
fi

if [ -d "src/middleend/metatron_graph" ]; then
    echo "  ↓"
    echo "Metatron Graph IR"
fi

if [ -d "src/middleend/optimizer" ]; then
    echo "  ↓"
    echo "optimizer"
fi

if [ -d "src/backend" ]; then
    echo "  ↓"
    echo "backend"
fi

echo
echo "-----------------------------------------------"
echo "IMPLEMENTATION STATUS"
echo "-----------------------------------------------"

FRONT=$(find src/frontend -name "*.cpp" | wc -l)
MID=$(find src/middleend -name "*.cpp" | wc -l)
BACK=$(find src/backend -name "*.cpp" | wc -l)
RUN=$(find src/runtime -name "*.cpp" | wc -l)

echo "frontend files: $FRONT"
echo "middleend files: $MID"
echo "backend files: $BACK"
echo "runtime files: $RUN"

TOTAL=$((FRONT + MID + BACK + RUN))

if [ "$TOTAL" -gt 0 ]; then
    PERCENT=$(( (FRONT * 30 + MID * 40 + BACK * 20 + RUN * 10) / TOTAL ))
    echo
    echo "estimated completion: $PERCENT%"
fi

echo
echo "==============================================="
echo "INSPECTION COMPLETE"
echo "==============================================="
