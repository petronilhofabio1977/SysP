
section .data
    str_0 db 61, 61, 61, 32, 67, 97, 108, 99, 117, 108, 97, 100, 111, 114, 97, 32, 83, 121, 115, 80, 32, 61, 61, 61, 10
    str_0_null db 0
    str_0_len equ str_0_null - str_0
    str_1 db 97, 32, 61, 32, 49, 48, 44, 32, 98, 32, 61, 32, 45, 51, 44, 32, 99, 32, 61, 32, 55, 10
    str_1_null db 0
    str_1_len equ str_1_null - str_1
    str_2 db 97, 32, 43, 32, 99, 58, 10
    str_2_null db 0
    str_2_len equ str_2_null - str_2
    str_3 db 97, 32, 42, 32, 99, 58, 10
    str_3_null db 0
    str_3_len equ str_3_null - str_3
    str_4 db 97, 32, 47, 32, 99, 58, 10
    str_4_null db 0
    str_4_len equ str_4_null - str_4
    str_5 db 97, 32, 37, 32, 99, 58, 10
    str_5_null db 0
    str_5_len equ str_5_null - str_5
    str_6 db 97, 98, 115, 40, 98, 41, 32, 61, 32, 97, 98, 115, 40, 45, 51, 41, 58, 10
    str_6_null db 0
    str_6_len equ str_6_null - str_6
    str_7 db 97, 98, 115, 40, 97, 41, 32, 61, 32, 97, 98, 115, 40, 49, 48, 41, 58, 10
    str_7_null db 0
    str_7_len equ str_7_null - str_7
    str_8 db 109, 105, 110, 40, 97, 44, 32, 99, 41, 32, 61, 32, 109, 105, 110, 40, 49, 48, 44, 32, 55, 41, 58, 10
    str_8_null db 0
    str_8_len equ str_8_null - str_8
    str_9 db 109, 97, 120, 40, 97, 44, 32, 99, 41, 32, 61, 32, 109, 97, 120, 40, 49, 48, 44, 32, 55, 41, 58, 10
    str_9_null db 0
    str_9_len equ str_9_null - str_9
    str_10 db 109, 105, 110, 40, 98, 44, 32, 99, 41, 32, 61, 32, 109, 105, 110, 40, 45, 51, 44, 32, 55, 41, 58, 10
    str_10_null db 0
    str_10_len equ str_10_null - str_10
    str_11 db 52, 33, 32, 61, 10
    str_11_null db 0
    str_11_len equ str_11_null - str_11
    str_12 db 54, 33, 32, 61, 10
    str_12_null db 0
    str_12_len equ str_12_null - str_12
    str_13 db 50, 94, 49, 48, 32, 61, 10
    str_13_null db 0
    str_13_len equ str_13_null - str_13
    str_14 db 51, 94, 53, 32, 61, 10
    str_14_null db 0
    str_14_len equ str_14_null - str_14
    str_15 db 83, 121, 115, 80, 10
    str_15_null db 0
    str_15_len equ str_15_null - str_15
    str_16 db 108, 101, 110, 32, 100, 101, 32, 83, 121, 115, 80, 58, 10
    str_16_null db 0
    str_16_len equ str_16_null - str_16
    str_17 db 120, 32, 61, 32, 52, 50, 44, 32, 99, 108, 97, 115, 115, 105, 102, 105, 99, 97, 99, 97, 111, 58, 10
    str_17_null db 0
    str_17_len equ str_17_null - str_17
    str_18 db 114, 101, 115, 112, 111, 115, 116, 97, 33, 10
    str_18_null db 0
    str_18_len equ str_18_null - str_18
    str_19 db 111, 117, 116, 114, 111, 10
    str_19_null db 0
    str_19_len equ str_19_null - str_19
    str_20 db 61, 61, 61, 32, 70, 105, 109, 32, 61, 61, 61, 10
    str_20_null db 0
    str_20_len equ str_20_null - str_20

    sysp_str_true  db 't','r','u','e',10
    sysp_str_false db 'f','a','l','s','e',10
section .text
global _start

fatorial:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-8], rdi
    mov rax, [rbp-8]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    setle al
    movzx rax, al
    test rax, rax
    jz .if_else_0
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
    jmp .if_end_1
.if_else_0:
.if_end_1:
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-8]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    sub rax, rbx
    push rax
    pop rdi
    mov rax, fatorial
    call rax
    mov rbx, rax
    pop rax
    imul rax, rbx
    mov rsp, rbp
    pop rbp
    ret
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

potencia:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-8], rdi
    mov [rbp-16], rsi
    mov rax, 1
    mov [rbp-24], rax
    mov rax, 0
    mov [rbp-32], rax
.while_loop_2:
    mov rax, [rbp-32]
    push rax
    mov rax, [rbp-16]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    setl al
    movzx rax, al
    test rax, rax
    jz .while_end_3
    mov rax, [rbp-24]
    push rax
    mov rax, [rbp-8]
    mov rbx, rax
    pop rax
    imul rax, rbx
    mov [rbp-24], rax
    mov rax, [rbp-32]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-32], rax
    jmp .while_loop_2
.while_end_3:
    mov rax, [rbp-24]
    mov rsp, rbp
    pop rbp
    ret
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

main:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov rax, 1
    mov rdi, 1
    mov rsi, str_0
    mov rdx, str_0_len
    syscall
    mov rax, 10
    mov [rbp-8], rax
    mov rax, 3
    neg rax
    mov [rbp-16], rax
    mov rax, 7
    mov [rbp-24], rax
    mov rax, 1
    mov rdi, 1
    mov rsi, str_1
    mov rdx, str_1_len
    syscall
    mov rax, 1
    mov rdi, 1
    mov rsi, str_2
    mov rdx, str_2_len
    syscall
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-24]
    mov rbx, rax
    pop rax
    add rax, rbx
    mov rdi, rax
    call sysp_println_int
    mov rax, 1
    mov rdi, 1
    mov rsi, str_3
    mov rdx, str_3_len
    syscall
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-24]
    mov rbx, rax
    pop rax
    imul rax, rbx
    mov rdi, rax
    call sysp_println_int
    mov rax, 1
    mov rdi, 1
    mov rsi, str_4
    mov rdx, str_4_len
    syscall
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-24]
    mov rbx, rax
    pop rax
    xor rdx, rdx
    div rbx
    mov rdi, rax
    call sysp_println_int
    mov rax, 1
    mov rdi, 1
    mov rsi, str_5
    mov rdx, str_5_len
    syscall
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-24]
    mov rbx, rax
    pop rax
    xor rdx, rdx
    div rbx
    mov rax, rdx
    mov rdi, rax
    call sysp_println_int
    mov rax, 1
    mov rdi, 1
    mov rsi, str_6
    mov rdx, str_6_len
    syscall
    mov rax, [rbp-16]
    mov rdi, rax
    call sysp_math_abs
    mov rdi, rax
    call sysp_println_int
    mov rax, 1
    mov rdi, 1
    mov rsi, str_7
    mov rdx, str_7_len
    syscall
    mov rax, [rbp-8]
    mov rdi, rax
    call sysp_math_abs
    mov rdi, rax
    call sysp_println_int
    mov rax, 1
    mov rdi, 1
    mov rsi, str_8
    mov rdx, str_8_len
    syscall
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-24]
    mov rsi, rax
    pop rdi
    call sysp_math_min
    mov rdi, rax
    call sysp_println_int
    mov rax, 1
    mov rdi, 1
    mov rsi, str_9
    mov rdx, str_9_len
    syscall
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-24]
    mov rsi, rax
    pop rdi
    call sysp_math_max
    mov rdi, rax
    call sysp_println_int
    mov rax, 1
    mov rdi, 1
    mov rsi, str_10
    mov rdx, str_10_len
    syscall
    mov rax, [rbp-16]
    push rax
    mov rax, [rbp-24]
    mov rsi, rax
    pop rdi
    call sysp_math_min
    mov rdi, rax
    call sysp_println_int
    mov rax, 1
    mov rdi, 1
    mov rsi, str_11
    mov rdx, str_11_len
    syscall
    mov rax, 4
    push rax
    pop rdi
    mov rax, fatorial
    call rax
    mov rdi, rax
    call sysp_println_int
    mov rax, 1
    mov rdi, 1
    mov rsi, str_12
    mov rdx, str_12_len
    syscall
    mov rax, 6
    push rax
    pop rdi
    mov rax, fatorial
    call rax
    mov rdi, rax
    call sysp_println_int
    mov rax, 1
    mov rdi, 1
    mov rsi, str_13
    mov rdx, str_13_len
    syscall
    mov rax, 10
    push rax
    mov rax, 2
    push rax
    pop rdi
    pop rsi
    mov rax, potencia
    call rax
    mov rdi, rax
    call sysp_println_int
    mov rax, 1
    mov rdi, 1
    mov rsi, str_14
    mov rdx, str_14_len
    syscall
    mov rax, 5
    push rax
    mov rax, 3
    push rax
    pop rdi
    pop rsi
    mov rax, potencia
    call rax
    mov rdi, rax
    call sysp_println_int
    mov rax, str_15
    mov [rbp-32], rax
    mov rax, 1
    mov rdi, 1
    mov rsi, str_16
    mov rdx, str_16_len
    syscall
    mov rax, [rbp-32]
    mov rdi, rax
    call sysp_string_length
    mov rdi, rax
    call sysp_println_int
    mov rax, 42
    mov [rbp-40], rax
    mov rax, 1
    mov rdi, 1
    mov rsi, str_17
    mov rdx, str_17_len
    syscall
    mov rax, [rbp-40]
    mov [rbp-48], rax    ; match subject
    ; match begin
    mov rax, [rbp-48]
    cmp rax, 42
    jne .match_arm_skip_5
    mov rax, 1
    mov rdi, 1
    mov rsi, str_18
    mov rdx, str_18_len
    syscall
    jmp .match_end_4
.match_arm_skip_5:
    mov rax, [rbp-48]
    mov [rbp-56], rax    ; bind _
    mov rax, 1
    mov rdi, 1
    mov rsi, str_19
    mov rdx, str_19_len
    syscall
    jmp .match_end_4
.match_arm_skip_6:
.match_end_4:
    ; match end
    mov rax, 1
    mov rdi, 1
    mov rsi, str_20
    mov rdx, str_20_len
    syscall
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

_start:
    call main
    mov rax, 60
    xor rdi, rdi
    syscall

; println_int(rdi = value)
sysp_println_int:
    push rbp
    mov rbp, rsp
    sub rsp, 32

    ; handle zero
    cmp rdi, 0
    jne .pi_nonzero
    mov byte [rbp-1], 10    ; newline
    mov byte [rbp-2], '0'
    lea rsi, [rbp-2]
    mov rdx, 2
    mov rax, 1
    mov rdi, 1
    syscall
    jmp .pi_done

.pi_nonzero:
    ; handle negative
    xor r8, r8
    cmp rdi, 0
    jge .pi_positive
    inc r8
    neg rdi

    .pi_positive:
    ; convert to string in buffer (backwards)
    lea rcx, [rbp-1]
    mov byte [rcx], 10      ; newline first
    dec rcx
    mov rax, rdi

    .pi_loop:
    xor rdx, rdx
    mov rbx, 10
    div rbx
    add dl, '0'
    mov [rcx], dl
    dec rcx
    test rax, rax
    jnz .pi_loop

    ; add minus if negative
    test r8, r8
    jz .pi_emit
    mov byte [rcx], '-'
    dec rcx

    .pi_emit:
    inc rcx
    lea rdx, [rbp-1]
    inc rdx
    sub rdx, rcx
    mov rsi, rcx
    mov rax, 1
    mov rdi, 1
    syscall

    .pi_done:
    mov rsp, rbp
    pop rbp
    ret
    
; println_bool(rdi = 0 or 1)
sysp_println_bool:
    test rdi, rdi
    jz .pb_false
    mov rax, 1
    mov rdi, 1
    mov rsi, sysp_str_true
    mov rdx, 5
    syscall
    ret
.pb_false:
    mov rax, 1
    mov rdi, 1
    mov rsi, sysp_str_false
    mov rdx, 6
    syscall
    ret

; sysp_string_length(rdi=str) → rax=length (stops at null or newline)
sysp_string_length:
    xor rax, rax
    test rdi, rdi
    jz .sl_done
.sl_loop:
    movzx rcx, byte [rdi+rax]
    test rcx, rcx
    jz .sl_done
    cmp rcx, 10
    je .sl_done
    inc rax
    jmp .sl_loop
.sl_done:
    ret

; sysp_math_abs(rdi=x) → rax=|x|
sysp_math_abs:
    mov rax, rdi
    test rax, rax
    jge .ma_done
    neg rax
.ma_done:
    ret

; sysp_math_min(rdi=a, rsi=b) → rax=min(a,b)
sysp_math_min:
    mov rax, rdi
    cmp rdi, rsi
    jle .mm_done
    mov rax, rsi
.mm_done:
    ret

; sysp_math_max(rdi=a, rsi=b) → rax=max(a,b)
sysp_math_max:
    mov rax, rdi
    cmp rdi, rsi
    jge .mx_done
    mov rax, rsi
.mx_done:
    ret
