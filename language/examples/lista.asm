
section .data
    str_0 db 78, 111, 32, 49, 58, 10
    str_0_null db 0
    str_0_len equ str_0_null - str_0
    str_1 db 78, 111, 32, 50, 58, 10
    str_1_null db 0
    str_1_len equ str_1_null - str_1
    str_2 db 78, 111, 32, 51, 58, 10
    str_2_null db 0
    str_2_len equ str_2_null - str_2
    str_3 db 83, 111, 109, 97, 32, 116, 111, 116, 97, 108, 58, 10
    str_3_null db 0
    str_3_len equ str_3_null - str_3
    str_4 db 77, 101, 100, 105, 97, 58, 10
    str_4_null db 0
    str_4_len equ str_4_null - str_4
    str_5 db 78, 111, 32, 108, 111, 99, 97, 108, 32, 110, 111, 32, 114, 101, 103, 105, 111, 110, 58, 10
    str_5_null db 0
    str_5_len equ str_5_null - str_5
    str_6 db 110, 50, 32, 97, 112, 111, 115, 32, 109, 111, 100, 105, 102, 105, 99, 97, 99, 97, 111, 58, 10
    str_6_null db 0
    str_6_len equ str_6_null - str_6

    sysp_str_true  db 't','r','u','e',10
    sysp_str_false db 'f','a','l','s','e',10
section .text
global _start

soma_lista:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-8], rdi
    mov [rbp-16], rsi
    mov [rbp-24], rdx
    mov rax, [rbp-8]    ; load ptr to a
    mov rax, [rax]    ; a.valor via ptr
    push rax
    mov rax, [rbp-16]    ; load ptr to b
    mov rax, [rax]    ; b.valor via ptr
    mov rbx, rax
    pop rax
    add rax, rbx
    push rax
    mov rax, [rbp-24]    ; load ptr to c
    mov rax, [rax]    ; c.valor via ptr
    mov rbx, rax
    pop rax
    add rax, rbx
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
    ; struct No n1 base=[rbp-8] fields=2
    mov rax, 10
    mov [rbp-8], rax    ; No.valor
    mov rax, 0
    mov [rbp-16], rax    ; No.proximo
    ; struct No n2 base=[rbp-24] fields=2
    mov rax, 20
    mov [rbp-24], rax    ; No.valor
    mov rax, 0
    mov [rbp-32], rax    ; No.proximo
    ; struct No n3 base=[rbp-40] fields=2
    mov rax, 30
    mov [rbp-40], rax    ; No.valor
    mov rax, 0
    mov [rbp-48], rax    ; No.proximo
    mov rax, 1
    mov rdi, 1
    mov rsi, str_0
    mov rdx, str_0_len
    syscall
    mov rax, [rbp-8]    ; n1.valor
    mov rdi, rax
    call sysp_println_int
    mov rax, 1
    mov rdi, 1
    mov rsi, str_1
    mov rdx, str_1_len
    syscall
    mov rax, [rbp-24]    ; n2.valor
    mov rdi, rax
    call sysp_println_int
    mov rax, 1
    mov rdi, 1
    mov rsi, str_2
    mov rdx, str_2_len
    syscall
    mov rax, [rbp-40]    ; n3.valor
    mov rdi, rax
    call sysp_println_int
    lea rax, [rbp-40]
    push rax
    lea rax, [rbp-24]
    push rax
    lea rax, [rbp-8]
    push rax
    pop rdi
    pop rsi
    pop rdx
    mov rax, soma_lista
    call rax
    mov [rbp-56], rax
    mov rax, 1
    mov rdi, 1
    mov rsi, str_3
    mov rdx, str_3_len
    syscall
    mov rax, [rbp-56]
    mov rdi, rax
    call sysp_println_int
    mov rax, 1
    mov rdi, 1
    mov rsi, str_4
    mov rdx, str_4_len
    syscall
    mov rax, [rbp-56]
    push rax
    mov rax, 3
    mov rbx, rax
    pop rax
    xor rdx, rdx
    div rbx
    mov rdi, rax
    call sysp_println_int
    ; region temp begin
    ; struct No local base=[rbp-64] fields=2
    mov rax, 99
    mov [rbp-64], rax    ; No.valor
    mov rax, 0
    mov [rbp-72], rax    ; No.proximo
    mov rax, 1
    mov rdi, 1
    mov rsi, str_5
    mov rdx, str_5_len
    syscall
    mov rax, [rbp-64]    ; local.valor
    mov rdi, rax
    call sysp_println_int
    ; region temp end
    mov rax, 50
    mov [rbp-24], rax    ; n2.valor =
    mov rax, 1
    mov rdi, 1
    mov rsi, str_6
    mov rdx, str_6_len
    syscall
    mov rax, [rbp-24]    ; n2.valor
    mov rdi, rax
    call sysp_println_int
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
