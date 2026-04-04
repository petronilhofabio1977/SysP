
section .data
    str_0 db 112, 49, 46, 120, 58, 10
    str_0_null db 0
    str_0_len equ str_0_null - str_0
    str_1 db 112, 49, 46, 121, 58, 10
    str_1_null db 0
    str_1_len equ str_1_null - str_1
    str_2 db 112, 50, 46, 120, 58, 10
    str_2_null db 0
    str_2_len equ str_2_null - str_2
    str_3 db 112, 50, 46, 121, 58, 10
    str_3_null db 0
    str_3_len equ str_3_null - str_3
    str_4 db 100, 105, 115, 116, 97, 110, 99, 105, 97, 32, 97, 111, 32, 113, 117, 97, 100, 114, 97, 100, 111, 32, 40, 51, 44, 52, 41, 58, 10
    str_4_null db 0
    str_4_len equ str_4_null - str_4
    str_5 db 100, 105, 115, 116, 97, 110, 99, 105, 97, 32, 97, 111, 32, 113, 117, 97, 100, 114, 97, 100, 111, 32, 40, 54, 44, 56, 41, 58, 10
    str_5_null db 0
    str_5_len equ str_5_null - str_5

    sysp_str_true  db 't','r','u','e',10
    sysp_str_false db 'f','a','l','s','e',10
section .text
global _start

distancia_quad:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-8], rdi
    mov [rbp-16], rsi
    mov rax, [rbp-8]    ; load ptr to a
    mov rax, [rax]    ; a.x via ptr
    push rax
    mov rax, [rbp-16]    ; load ptr to b
    mov rax, [rax]    ; b.x via ptr
    mov rbx, rax
    pop rax
    sub rax, rbx
    mov [rbp-24], rax
    mov rax, [rbp-8]    ; load ptr to a
    mov rax, [rax-8]    ; a.y via ptr
    push rax
    mov rax, [rbp-16]    ; load ptr to b
    mov rax, [rax-8]    ; b.y via ptr
    mov rbx, rax
    pop rax
    sub rax, rbx
    mov [rbp-32], rax
    mov rax, [rbp-24]
    push rax
    mov rax, [rbp-24]
    mov rbx, rax
    pop rax
    imul rax, rbx
    push rax
    mov rax, [rbp-32]
    push rax
    mov rax, [rbp-32]
    mov rbx, rax
    pop rax
    imul rax, rbx
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
    ; struct Ponto p1 base=[rbp-8] fields=2
    mov rax, 0
    mov [rbp-8], rax    ; Ponto.x
    mov rax, 0
    mov [rbp-16], rax    ; Ponto.y
    ; struct Ponto p2 base=[rbp-24] fields=2
    mov rax, 3
    mov [rbp-24], rax    ; Ponto.x
    mov rax, 4
    mov [rbp-32], rax    ; Ponto.y
    mov rax, 1
    mov rdi, 1
    mov rsi, str_0
    mov rdx, str_0_len
    syscall
    mov rax, [rbp-8]    ; p1.x
    mov rdi, rax
    call sysp_println_int
    mov rax, 1
    mov rdi, 1
    mov rsi, str_1
    mov rdx, str_1_len
    syscall
    mov rax, [rbp-16]    ; p1.y
    mov rdi, rax
    call sysp_println_int
    mov rax, 1
    mov rdi, 1
    mov rsi, str_2
    mov rdx, str_2_len
    syscall
    mov rax, [rbp-24]    ; p2.x
    mov rdi, rax
    call sysp_println_int
    mov rax, 1
    mov rdi, 1
    mov rsi, str_3
    mov rdx, str_3_len
    syscall
    mov rax, [rbp-32]    ; p2.y
    mov rdi, rax
    call sysp_println_int
    lea rax, [rbp-24]
    push rax
    lea rax, [rbp-8]
    push rax
    pop rdi
    pop rsi
    mov rax, distancia_quad
    call rax
    mov [rbp-40], rax
    mov rax, 1
    mov rdi, 1
    mov rsi, str_4
    mov rdx, str_4_len
    syscall
    mov rax, [rbp-40]
    mov rdi, rax
    call sysp_println_int
    mov rax, 6
    mov [rbp-24], rax    ; p2.x =
    mov rax, 8
    mov [rbp-32], rax    ; p2.y =
    lea rax, [rbp-24]
    push rax
    lea rax, [rbp-8]
    push rax
    pop rdi
    pop rsi
    mov rax, distancia_quad
    call rax
    mov [rbp-48], rax
    mov rax, 1
    mov rdi, 1
    mov rsi, str_5
    mov rdx, str_5_len
    syscall
    mov rax, [rbp-48]
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
