#!/usr/bin/env bash

echo "========================================"
echo "SYSP GRAMMAR COVERAGE AUDIT"
echo "========================================"
echo

GRAMMAR="docs/spec/sysp_grammar.ebnf"
PARSER="src/frontend/parser"

echo "Extracting grammar rules..."
echo

RULES=$(grep -E "^[a-zA-Z_]+" "$GRAMMAR" | grep "=" | sed 's/=.*//' | tr -d ' ')

TOTAL=0
FOUND=0
MISSING=0

for rule in $RULES
do
    TOTAL=$((TOTAL+1))

    if grep -R "$rule" "$PARSER" > /dev/null 2>&1
    then
        echo "OK   -> $rule"
        FOUND=$((FOUND+1))
    else
        echo "MISS -> $rule"
        MISSING=$((MISSING+1))
    fi
done

echo
echo "----------------------------------------"
echo "GRAMMAR STATS"
echo "----------------------------------------"

echo "Rules total: $TOTAL"
echo "Implemented: $FOUND"
echo "Missing: $MISSING"

if [ $TOTAL -gt 0 ]; then
    PERCENT=$((FOUND * 100 / TOTAL))
else
    PERCENT=0
fi

echo
echo "Coverage: $PERCENT %"
echo "========================================"
