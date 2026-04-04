#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
COMPILER="$SCRIPT_DIR/../../compiler/build/sysp"
PASS=0
FAIL=0

run_test() {
    local file="$1"
    local expect="$2"
    local name=$(basename "$file")
    local output
    output=$("$COMPILER" compile "$file" 2>&1)

    if [ "$expect" = "error" ]; then
        if echo "$output" | grep -q "\[Error\]"; then
            echo "  ✅ PASS: $name"
            PASS=$((PASS+1))
        else
            echo "  ❌ FAIL: $name (expected error but passed)"
            FAIL=$((FAIL+1))
        fi
    else
        if echo "$output" | grep -q "memory safe"; then
            echo "  ✅ PASS: $name"
            PASS=$((PASS+1))
        else
            echo "  ❌ FAIL: $name (expected success but failed)"
            echo "$output" | grep "\[Error\]"
            FAIL=$((FAIL+1))
        fi
    fi
}

echo "================================================"
echo " SysP Test Suite — Jarbes 14 Checkers"
echo "================================================"

echo ""
echo "── Valid programs ───────────────────────────────"
run_test "$SCRIPT_DIR/valid/hello.sp"        ok
run_test "$SCRIPT_DIR/valid/move_ok.sp"      ok
run_test "$SCRIPT_DIR/valid/region_ok.sp"    ok
run_test "$SCRIPT_DIR/valid/init_ok.sp"      ok
run_test "$SCRIPT_DIR/valid/overflow_ok.sp"  ok

echo ""
echo "── Ownership errors ─────────────────────────────"
run_test "$SCRIPT_DIR/ownership/use_after_move.sp"  error
run_test "$SCRIPT_DIR/ownership/region_escape.sp"   error

echo ""
echo "── Overflow errors ──────────────────────────────"
run_test "$SCRIPT_DIR/overflow/i32_overflow.sp"     error

echo ""
echo "── Dataflow errors ──────────────────────────────"
run_test "$SCRIPT_DIR/dataflow/uninit.sp"           error

echo ""
echo "── Match errors ─────────────────────────────────"
run_test "$SCRIPT_DIR/match/non_exhaustive.sp"      error
run_test "$SCRIPT_DIR/match/exhaustive_ok.sp"       ok
run_test "$SCRIPT_DIR/match/type_mismatch.sp"       error

echo ""
echo "================================================"
echo " Results: $PASS passed, $FAIL failed"
echo "================================================"
[ $FAIL -eq 0 ] && exit 0 || exit 1
