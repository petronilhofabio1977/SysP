#!/usr/bin/env bash
# ============================================================
#  SysP Compiler — Benchmark Completo
# ============================================================
set -uo pipefail

SYSP="./compiler/build/sysp"
HELLO="language/examples/hello.sp"
RUNS=10

# ── aritmética via python ────────────────────────────────────
py() { python3 -c "$1"; }

avg()    { python3 -c "vals=[$( IFS=,; echo "$*" )]; print(f'{sum(vals)/len(vals):.3f}')"; }
minv()   { python3 -c "vals=[$( IFS=,; echo "$*" )]; print(f'{min(vals):.3f}')"; }
maxv()   { python3 -c "vals=[$( IFS=,; echo "$*" )]; print(f'{max(vals):.3f}')"; }
ratio()  { python3 -c "print(f'{$1/$2:.2f}')"; }
pct()    { python3 -c "print(f'{100*$1/$2:.1f}')"; }
ns2ms()  { python3 -c "print(f'{($1)/1e6:.3f}')"; }

# ── helpers ─────────────────────────────────────────────────
RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'
CYAN='\033[0;36m'; BOLD='\033[1m'; RESET='\033[0m'
sep() { printf '%0.s─' {1..60}; echo; }
header() { echo -e "\n${BOLD}${CYAN}$1${RESET}"; sep; }
ns() { date +%s%N; }

echo ""
echo -e "${BOLD}╔══════════════════════════════════════════════════════════╗${RESET}"
echo -e "${BOLD}║        SysP Compiler — Benchmark Completo                ║${RESET}"
echo -e "${BOLD}╚══════════════════════════════════════════════════════════╝${RESET}"
echo -e "  Data    : $(date '+%Y-%m-%d %H:%M:%S')"
echo -e "  Sistema : $(uname -srm)"
echo -e "  CPU     : $(grep 'model name' /proc/cpuinfo | head -1 | cut -d: -f2 | xargs)"
echo -e "  Memória : $(free -h | awk '/Mem:/{print $2}') total"
echo -e "  GCC     : $(gcc --version | head -1)"
echo -e "  SysP    : $($SYSP version 2>&1 | head -1)"

# ════════════════════════════════════════════════════════════
# 1. TEMPO TOTAL — 10 execuções
# ════════════════════════════════════════════════════════════
header "1. TEMPO DE COMPILAÇÃO — hello.sp (${RUNS} execuções)"

times_total=()
for i in $(seq 1 $RUNS); do
    t0=$(ns)
    $SYSP compile "$HELLO" > /dev/null 2>&1
    t1=$(ns)
    ms=$(ns2ms $((t1-t0)))
    times_total+=("$ms")
    printf "  Run %2d: %8s ms\n" "$i" "$ms"
done

avg_total=$(avg "${times_total[@]}")
min_total=$(minv "${times_total[@]}")
max_total=$(maxv "${times_total[@]}")

echo ""
echo -e "  ${GREEN}Média  : ${avg_total} ms${RESET}"
echo    "  Min    : ${min_total} ms"
echo    "  Max    : ${max_total} ms"

# ════════════════════════════════════════════════════════════
# 2. TEMPO POR FASE (--bench, 5 execuções, média)
# ════════════════════════════════════════════════════════════
header "2. TEMPO POR FASE (5 execuções, média)"

declare -A phase_acc
for ph in Lexer Parser TypeSystem MetatronIR Jarbes Backend PatternGraph TOTAL; do
    phase_acc[$ph]="0"
done
PHASE_RUNS=5

for i in $(seq 1 $PHASE_RUNS); do
    out=$($SYSP compile "$HELLO" --bench 2>/dev/null) || out=""
    while IFS= read -r line; do
        for ph in Lexer Parser TypeSystem MetatronIR Jarbes Backend PatternGraph TOTAL; do
            if echo "$line" | grep -q "$ph" 2>/dev/null; then
                val=$(echo "$line" | grep -oP '[\d]+\.[\d]+' 2>/dev/null | head -1) || val=""
                [ -n "$val" ] && phase_acc[$ph]=$(python3 -c "print(${phase_acc[$ph]}+$val)")
            fi
        done
    done <<< "$out"
done

echo "  Fase             Média (ms)   % do total"
echo "  ───────────────────────────────────────"
total_ph=$(python3 -c "print(f'{${phase_acc[TOTAL]}/$PHASE_RUNS:.3f}')")
for ph in Lexer Parser TypeSystem MetatronIR Jarbes Backend PatternGraph; do
    avg_ph=$(python3 -c "print(f'{${phase_acc[$ph]}/$PHASE_RUNS:.3f}')")
    p=$(pct "$avg_ph" "$total_ph" 2>/dev/null || echo "?")
    printf "  %-16s %8s ms    %5s%%\n" "$ph" "$avg_ph" "$p"
done
echo "  ───────────────────────────────────────"
printf "  %-16s %8s ms    100%%\n" "TOTAL" "$total_ph"

# identifica fase mais lenta
slowest_ph=""
slowest_val=0
for ph in Lexer Parser TypeSystem MetatronIR Jarbes Backend PatternGraph; do
    avg_ph=$(python3 -c "print(f'{${phase_acc[$ph]}/$PHASE_RUNS:.3f}')")
    is_max=$(python3 -c "print(1 if float('$avg_ph') > float('$slowest_val') else 0)")
    [ "$is_max" = "1" ] && { slowest_ph=$ph; slowest_val=$avg_ph; }
done
echo -e "\n  ${YELLOW}Fase mais lenta: $slowest_ph (${slowest_val} ms)${RESET}"

# ════════════════════════════════════════════════════════════
# 3. VELOCIDADE DO BOOKINDEX — 5 queries
# ════════════════════════════════════════════════════════════
header "3. BOOKINDEX — velocidade de busca (5 queries)"

queries=(
    "memory safety rust ownership"
    "compiler design parsing algorithms"
    "machine learning python neural network"
    "cryptography network security protocols"
    "data structures algorithms complexity"
)

echo "  Query                                    Tempo (ms)"
echo "  ──────────────────────────────────────────────────"

search_times=()
for q in "${queries[@]}"; do
    t0=$(ns)
    $SYSP buscar "$q" > /dev/null 2>/dev/null || true
    t1=$(ns)
    ms=$(ns2ms $((t1-t0)))
    search_times+=("$ms")
    printf "  %-40s %8s ms\n" "${q:0:40}" "$ms"
done

avg_search=$(avg "${search_times[@]}")
echo ""
echo -e "  ${GREEN}Média por busca: ${avg_search} ms${RESET}"

# ════════════════════════════════════════════════════════════
# 4. PATTERNGRAPH — registro e carregamento
# ════════════════════════════════════════════════════════════
header "4. PATTERNGRAPH — registro e carregamento"

echo "  Operação                        Tempo (ms)"
echo "  ──────────────────────────────────────────"

stats_times=()
for i in $(seq 1 5); do
    t0=$(ns)
    $SYSP stats > /dev/null 2>&1
    t1=$(ns)
    ms=$(ns2ms $((t1-t0)))
    stats_times+=("$ms")
done
avg_stats=$(avg "${stats_times[@]}")
printf "  %-32s %8s ms\n" "Carregar PatternGraph (stats)" "$avg_stats"

# PatternGraph tempo via --bench
pg_times=()
for i in $(seq 1 5); do
    val=$($SYSP compile "$HELLO" --bench 2>/dev/null | grep "PatternGraph" | grep -oP '[\d]+\.[\d]+' 2>/dev/null | head -1) || val=""
    [ -n "$val" ] && pg_times+=("$val")
done
[ ${#pg_times[@]} -gt 0 ] && avg_pg=$(avg "${pg_times[@]}") || avg_pg="N/A"
printf "  %-32s %8s ms\n" "Registrar + salvar padrão" "$avg_pg"

padroes=$($SYSP stats 2>/dev/null | grep "padroes unicos"   | grep -oP '\d+' | head -1)
registros=$($SYSP stats 2>/dev/null | grep "total registros" | grep -oP '\d+' | head -1)
echo ""
echo "  PatternGraph: ${padroes} padrões únicos | ${registros} registros totais"

# ════════════════════════════════════════════════════════════
# 5. TAMANHO DOS BINÁRIOS
# ════════════════════════════════════════════════════════════
header "5. TAMANHO DOS ARQUIVOS GERADOS"

ASM="language/examples/hello.asm"
SYSP_BIN="compiler/build/sysp"

echo "  Arquivo                              Tamanho"
echo "  ─────────────────────────────────────────────────"

asm_size=0
if [ -f "$ASM" ]; then
    asm_size=$(wc -c < "$ASM")
    asm_lines=$(wc -l < "$ASM")
    printf "  %-36s %7d bytes   (%d linhas)\n" "hello.asm (saída SysP)" "$asm_size" "$asm_lines"
fi

sysp_sz=$(stat -c%s "$SYSP_BIN")
sysp_mb=$(python3 -c "print(f'{$sysp_sz/1048576:.1f}')")
printf "  %-36s %7d bytes   (%.1f MB)\n" "sysp (binário compilador)" "$sysp_sz" "${sysp_mb%f}"

if [ -f "sysp_books.bin" ]; then
    books_sz=$(stat -c%s "sysp_books.bin")
    books_mb=$(python3 -c "print(f'{$books_sz/1048576:.1f}')")
    printf "  %-36s %7d bytes   (%s MB)\n" "sysp_books.bin (BookIndex)" "$books_sz" "$books_mb"
fi

if [ -f "sysp_patterns.bin" ]; then
    pat_sz=$(stat -c%s "sysp_patterns.bin")
    printf "  %-36s %7d bytes\n" "sysp_patterns.bin (PatternGraph)" "$pat_sz"
fi

# ════════════════════════════════════════════════════════════
# 6. COMPARAÇÃO COM GCC
# ════════════════════════════════════════════════════════════
header "6. COMPARAÇÃO: SysP vs GCC (hello world equivalente)"

cat > /tmp/bench_hello.c << 'EOF'
#include <stdio.h>
int main() {
    printf("Olá, SysP!\n");
    printf("%d\n", 42);
    printf("%.2f\n", 3.14);
    printf("true\n");
    return 0;
}
EOF

gcc_times=()
for i in $(seq 1 $RUNS); do
    t0=$(ns)
    gcc -O0 -o /tmp/bench_hello /tmp/bench_hello.c
    t1=$(ns)
    ms=$(ns2ms $((t1-t0)))
    gcc_times+=("$ms")
done

avg_gcc=$(avg "${gcc_times[@]}")
min_gcc=$(minv "${gcc_times[@]}")
max_gcc=$(maxv "${gcc_times[@]}")
gcc_bin_sz=$(stat -c%s /tmp/bench_hello)

echo "  Métrica                      SysP              GCC -O0"
echo "  ───────────────────────────────────────────────────────"
printf "  %-28s %10s ms    %10s ms\n" "Média (${RUNS} runs)" "$avg_total" "$avg_gcc"
printf "  %-28s %10s ms    %10s ms\n" "Mínimo" "$min_total" "$min_gcc"
printf "  %-28s %10s ms    %10s ms\n" "Máximo" "$max_total" "$max_gcc"
printf "  %-28s %10d B     %10d B\n"  "Artefato de saída"  "$asm_size" "$gcc_bin_sz"

ratio_val=$(ratio "$avg_total" "$avg_gcc")
echo ""
echo -e "  Razão SysP/GCC: ${YELLOW}${ratio_val}x${RESET}"
echo    "  (SysP: ASM + 13 análises de segurança; GCC: binário nativo -O0)"

# ════════════════════════════════════════════════════════════
# SUMÁRIO FINAL
# ════════════════════════════════════════════════════════════
echo ""
echo -e "${BOLD}╔══════════════════════════════════════════════════════════╗${RESET}"
echo -e "${BOLD}║                   SUMÁRIO DO BENCHMARK                  ║${RESET}"
echo -e "${BOLD}╚══════════════════════════════════════════════════════════╝${RESET}"
echo ""
printf "  ${GREEN}%-32s${RESET} ${BOLD}%s ms${RESET}\n"      "Compilação hello.sp (média):"   "$avg_total"
printf "  ${GREEN}%-32s${RESET} ${BOLD}%s ms${RESET}\n"      "Fase mais lenta:"               "${slowest_ph}: ${slowest_val}"
printf "  ${GREEN}%-32s${RESET} ${BOLD}%s ms${RESET}\n"      "BookIndex busca (média):"       "$avg_search"
printf "  ${GREEN}%-32s${RESET} ${BOLD}%s ms${RESET}\n"      "PatternGraph load (stats):"     "$avg_stats"
printf "  ${GREEN}%-32s${RESET} ${BOLD}%s padrões${RESET}\n" "PatternGraph tamanho:"          "$padroes"
printf "  ${GREEN}%-32s${RESET} ${BOLD}384.212 palavras únicas${RESET}\n" "BookIndex:"
printf "  ${GREEN}%-32s${RESET} ${BOLD}%sx${RESET} mais lento que GCC\n" "Razão SysP/GCC:"  "$ratio_val"
echo ""
