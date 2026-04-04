
section .data
    str_0 db 109, 97, 105, 111, 114, 32, 113, 117, 101, 32, 53, 10
    str_0_len equ $ - str_0
    str_1 db 109, 101, 110, 111, 114, 32, 111, 117, 32, 105, 103, 117, 97, 108, 32, 97, 32, 53, 10
    str_1_len equ $ - str_1

    sysp_str_true  db 't','r','u','e',10
    sysp_str_false db 'f','a','l','s','e',10
section .text
global _start

maior:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-8], rdi
    mov [rbp-16], rsi
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    setg al
    movzx rax, al
    test rax, rax
    jz .if_else_0
    mov rax, [rbp-8]
    mov rsp, rbp
    pop rbp
    ret
    jmp .if_end_1
.if_else_0:
.if_end_1:
    mov rax, [rbp-16]
    mov rsp, rbp
    pop rbp
    ret
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

esta_entre:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-8], rdi
    mov [rbp-16], rsi
    mov [rbp-24], rdx
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    setge al
    movzx rax, al
    push rax
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-24]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    setle al
    movzx rax, al
    mov rbx, rax
    pop rax
    test rax, rax
    setnz al
    test rbx, rbx
    setnz bl
    and al, bl
    movzx rax, al
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
    mov rax, 20
    push rax
    mov rax, 10
    push rax
    pop rdi
    pop rsi
    mov rax, maior
    call rax
    mov rdi, rax
    call sysp_println_int
    mov rax, 5
    push rax
    mov rax, 30
    push rax
    pop rdi
    pop rsi
    mov rax, maior
    call rax
    mov rdi, rax
    call sysp_println_int
    mov rax, 10
    push rax
    mov rax, 1
    push rax
    mov rax, 5
    push rax
    pop rdi
    pop rsi
    pop rdx
    mov rax, esta_entre
    call rax
    mov rdi, rax
    call sysp_println_int
    mov rax, 10
    push rax
    mov rax, 1
    push rax
    mov rax, 15
    push rax
    pop rdi
    pop rsi
    pop rdx
    mov rax, esta_entre
    call rax
    mov rdi, rax
    call sysp_println_int
    mov rax, 0
    mov [rbp-8], rax
    mov rax, 1
    mov [rbp-16], rax
.for_loop_2:
    mov rax, [rbp-16]
    mov rax, 10
    mov rbx, rax
    mov rax, [rbp-16]
    cmp rax, rbx
    jg .for_end_3
    mov rax, [rbp-16]
    mov rbx, [rbp-8]
    add rbx, rax
    mov [rbp-8], rbx
    mov rax, [rbp-16]
    inc rax
    mov [rbp-16], rax
    jmp .for_loop_2
.for_end_3:
    mov rax, [rbp-8]
    mov rdi, rax
    call sysp_println_int
    mov rax, 7
    mov [rbp-24], rax
    mov rax, [rbp-24]
    push rax
    mov rax, 5
    mov rbx, rax
    pop rax
    cmp rax, rbx
    setg al
    movzx rax, al
    test rax, rax
    jz .if_else_4
    mov rax, 1
    mov rdi, 1
    mov rsi, str_0
    mov rdx, str_0_len
    syscall
    jmp .if_end_5
.if_else_4:
    mov rax, 1
    mov rdi, 1
    mov rsi, str_1
    mov rdx, str_1_len
    syscall
.if_end_5:
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
