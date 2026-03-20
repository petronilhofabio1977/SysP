#!/bin/bash
COMPILER="$(dirname "$0")/../build/sysp"
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
run_test tests/valid/hello.sp        ok
run_test tests/valid/move_ok.sp      ok
run_test tests/valid/region_ok.sp    ok
run_test tests/valid/init_ok.sp      ok
run_test tests/valid/overflow_ok.sp  ok

echo ""
echo "── Ownership errors ─────────────────────────────"
run_test tests/ownership/use_after_move.sp  error
run_test tests/ownership/region_escape.sp   error

echo ""
echo "── Overflow errors ──────────────────────────────"
run_test tests/overflow/i32_overflow.sp     error

echo ""
echo "── Dataflow errors ──────────────────────────────"
run_test tests/dataflow/uninit.sp           error

echo ""
echo "── Match errors ─────────────────────────────────"
run_test tests/match/non_exhaustive.sp      error
run_test tests/match/exhaustive_ok.sp       ok

echo ""
echo "================================================"
echo " Results: $PASS passed, $FAIL failed"
echo "================================================"
[ $FAIL -eq 0 ] && exit 0 || exit 1
