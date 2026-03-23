#!/usr/bin/env bash

echo "================================"
echo "SYSP ARCHITECTURE AUDIT"
echo "================================"

DOC="docs/compiler_architecture.md"

if [ ! -f "$DOC" ]; then
    echo "Documentation not found"
    exit 1
fi

echo
echo "Reading architecture documentation..."

MODULES=$(grep -i "module" "$DOC" | awk '{print $2}')

for mod in $MODULES
do
    echo
    echo "Checking module: $mod"

    FILE=$(find src -iname "*$mod*")

    if [ -z "$FILE" ]; then
        echo "ERROR: module $mod not implemented"
        continue
    fi

    for f in $FILE
    do
        if [ ! -s "$f" ]; then
            echo "ERROR: file empty -> $f"
        else
            echo "OK: $f"
        fi
    done
done
