#!/usr/bin/env bash
# ================================================================
# sp — SysP language toolchain wrapper
# ================================================================

set -euo pipefail

SP_COMPILER=""
for candidate in \
    "/home/techmaster/Projetos/linguagens/SysP/compiler/build/sysp" \
    "$(command -v sysp 2>/dev/null || true)"
do
    if [[ -x "$candidate" ]]; then
        SP_COMPILER="$candidate"
        break
    fi
done

if [[ -z "$SP_COMPILER" ]]; then
    echo -e "\033[1;31m[sp] erro: compilador SysP não encontrado\033[0m"
    exit 1
fi

RED='\033[1;31m'; GRN='\033[1;32m'; YLW='\033[1;33m'
CYN='\033[1;36m'; BLD='\033[1m';    DIM='\033[2m';   RST='\033[0m'

ok()   { echo -e "${GRN}✓${RST} $*"; }
info() { echo -e "${CYN}●${RST} $*"; }
fail() { echo -e "${RED}✗${RST} $*" >&2; }
step() { echo -e "${DIM}  $*${RST}"; }
banner(){ echo -e "${BLD}${CYN}══════════════════════════════════════${RST}"; }

filter_compiler_output() {
    while IFS= read -r line; do
        if   [[ "$line" =~ \[Error\]           ]]; then echo -e "${RED}${line}${RST}"
        elif [[ "$line" =~ \[Jarbes\]\ Error   ]]; then echo -e "${RED}  ${line}${RST}"
        elif [[ "$line" =~ memory\ safe        ]]; then echo -e "${GRN}${line}${RST}"
        elif [[ "$line" =~ \[Jarbes\].*OK      ]]; then echo -e "${DIM}${line}${RST}"
        elif [[ "$line" =~ \[JarbesKernel\]    ]]; then echo -e "${DIM}${line}${RST}"
        elif [[ "$line" =~ \[AI\]|\[PatternGraph\]|\[Optimizer\] ]]; then echo -e "${DIM}${line}${RST}"
        elif [[ "$line" =~ output:             ]]; then echo -e "${GRN}${line}${RST}"
        elif [[ "$line" =~ ^\[SysP\]           ]]; then echo -e "${BLD}${line}${RST}"
        else echo "$line"
        fi
    done
}

resolve_sp() {
    local file="$1"
    if [[ ! -f "$file" ]]; then fail "arquivo não encontrado: $file"; exit 1; fi
    if [[ "${file: -3}" != ".sp" ]]; then fail "esperado arquivo .sp: $file"; exit 1; fi
    realpath "$file"
}

compile_sp() {
    local sp_file="$1"
    local out
    out=$("$SP_COMPILER" compile "$sp_file" 2>&1)
    echo "$out" | filter_compiler_output
    if echo "$out" | grep -q "\[Error\]"; then return 1; fi
    return 0
}

assemble() {
    local asm="$1"; local obj="${asm%.asm}.o"
    echo -e "${DIM}  nasm  $(basename "$asm") → $(basename "$obj")${RST}" >&2
    local nasm_err
    if ! nasm_err=$(nasm -f elf64 "$asm" -o "$obj" 2>&1); then
        echo -e "${RED}${nasm_err}${RST}" >&2
        echo -e "${RED}✗${RST} erro de montagem" >&2
        return 1
    fi
    echo "$obj"
}

link_obj() {
    local obj="$1"; local bin="${obj%.o}"
    echo -e "${DIM}  ld    $(basename "$obj") → $(basename "$bin")${RST}" >&2
    local ld_err
    if ! ld_err=$(ld "$obj" -o "$bin" 2>&1); then
        echo -e "${RED}${ld_err}${RST}" >&2
        echo -e "${RED}✗${RST} erro de ligação" >&2
        return 1
    fi
    echo "$bin"
}

cleanup() {
    local base="$1"
    for ext in asm o; do
        local f="${base}.${ext}"
        [[ -f "$f" ]] && { rm -f "$f"; echo -e "${DIM}  removido $(basename "$f")${RST}" >&2; }
    done
}

cmd_run() {
    [[ -z "${1:-}" ]] && { fail "uso: sp run <arquivo.sp>"; exit 1; }
    local sp_file; sp_file=$(resolve_sp "$1")
    local base="${sp_file%.sp}"

    banner; info "sp run  $(basename "$sp_file")"; banner

    compile_sp "$sp_file" || { fail "compilação falhou — corrija os erros acima"; exit 1; }

    local obj; obj=$(assemble "${base}.asm") || exit 1
    local bin; bin=$(link_obj "$obj")        || { cleanup "$base"; exit 1; }
    cleanup "$base"

    ok "binário: $(basename "$bin")"
    banner
    echo -e "${BLD}▶ executando $(basename "$bin")${RST}"
    banner
    "$bin"
    local code=$?
    banner
    [[ $code -eq 0 ]] \
        && ok "saiu com código $code" \
        || echo -e "${YLW}⚠${RST} saiu com código $code"
    exit $code
}

cmd_build() {
    [[ -z "${1:-}" ]] && { fail "uso: sp build <arquivo.sp>"; exit 1; }
    local sp_file; sp_file=$(resolve_sp "$1")
    local base="${sp_file%.sp}"

    banner; info "sp build  $(basename "$sp_file")"; banner

    compile_sp "$sp_file" || { fail "compilação falhou — corrija os erros acima"; exit 1; }

    local obj; obj=$(assemble "${base}.asm") || exit 1
    local bin; bin=$(link_obj "$obj")        || { cleanup "$base"; exit 1; }
    cleanup "$base"

    banner
    ok "binário gerado: ${BLD}$(basename "$bin")${RST}"
    ok "caminho:        $bin"
    ok "tamanho:        $(du -h "$bin" | cut -f1)"
}

cmd_check() {
    [[ -z "${1:-}" ]] && { fail "uso: sp check <arquivo.sp>"; exit 1; }
    local sp_file; sp_file=$(resolve_sp "$1")
    local base="${sp_file%.sp}"

    banner; info "sp check  $(basename "$sp_file")"; banner

    local ok_check=0
    compile_sp "$sp_file" || ok_check=1
    [[ -f "${base}.asm" ]] && rm -f "${base}.asm"

    if [[ $ok_check -eq 0 ]]; then
        ok "programa seguro — sem violações Jarbes"
    else
        fail "verificação encontrou problemas de segurança"
        exit 1
    fi
}

cmd_buscar() {
    [[ -z "${1:-}" ]] && { fail "uso: sp buscar \"tema\""; exit 1; }
    banner; info "sp buscar  \"$1\""; banner
    "$SP_COMPILER" buscar "$1"
}

cmd_sugerir() {
    [[ -z "${1:-}" ]] && { fail "uso: sp sugerir \"descrição\""; exit 1; }
    banner; info "sp sugerir  \"$1\""; banner
    "$SP_COMPILER" sugerir "$1"
}

cmd_help() {
    echo -e "
${BLD}${CYN}sp${RST} — SysP Language Toolchain

${BLD}Uso:${RST}
  ${GRN}sp run${RST}     <arquivo.sp>    compila, monta, linka e executa
  ${GRN}sp build${RST}   <arquivo.sp>    gera binário (sem executar)
  ${GRN}sp check${RST}   <arquivo.sp>    análise Jarbes sem gerar binário
  ${GRN}sp buscar${RST}  <\"tema\">        busca nos livros indexados
  ${GRN}sp sugerir${RST} <\"descrição\">   sugere código via IA

${BLD}Exemplos:${RST}
  sp run    hello.sp
  sp build  calculadora.sp
  sp check  programa.sp
  sp buscar  \"alocação de memória\"
  sp sugerir \"função que soma lista\"

${DIM}Compilador: $SP_COMPILER${RST}
"
}

cmd="${1:-help}"; shift || true
case "$cmd" in
    run)            cmd_run     "${1:-}" ;;
    build)          cmd_build   "${1:-}" ;;
    check)          cmd_check   "${1:-}" ;;
    buscar)         cmd_buscar  "${1:-}" ;;
    sugerir)        cmd_sugerir "${1:-}" ;;
    help|--help|-h) cmd_help ;;
    *)
        fail "comando desconhecido: '$cmd'"
        echo -e "  use ${CYN}sp help${RST} para ver os comandos disponíveis"
        exit 1
        ;;
esac
