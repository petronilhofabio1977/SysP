#!/usr/bin/env bash

echo "============================================="
echo "SYSP EBNF COVERAGE CHECK"
echo "============================================="

EBNF="docs/spec/sysp_grammar.ebnf"

echo
echo "Extracting grammar rules..."

RULES=$(grep -E '^[a-z_]+[[:space:]]*=' $EBNF | sed 's/=.*//' | tr -d ' ')

TOTAL=0
IMPLEMENTED=0
MISSING=0

echo
echo "Checking parser coverage..."
echo "--------------------------------"

for rule in $RULES
do
    TOTAL=$((TOTAL+1))

    FOUND=$(grep -R "$rule" src/frontend/parser 2>/dev/null)

    if [ -n "$FOUND" ]; then
        echo "OK   -> $rule"
        IMPLEMENTED=$((IMPLEMENTED+1))
    else
        echo "MISS -> $rule"
        MISSING=$((MISSING+1))
    fi
done

echo
echo "--------------------------------"
echo "EBNF STATS"
echo "--------------------------------"

echo "Rules total: $TOTAL"
echo "Implemented: $IMPLEMENTED"
echo "Missing: $MISSING"

PERCENT=$((IMPLEMENTED * 100 / TOTAL))

echo
echo "Coverage: $PERCENT %"

echo "============================================="
