; SysP Compiler Output
; Source: ../../language/modules/data_structures/tree/bst.sp


    sysp_str_true  db 't','r','u','e',10
    sysp_str_false db 'f','a','l','s','e',10
section .text
global _start

inserir:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-8], rdi
    mov [rbp-16], rsi
    mov rax, [rbp-8]
    push rax
    mov rax, null
    mov rbx, rax
    pop rax
    cmp rax, rbx
    sete al
    movzx rax, al
    test rax, rax
    jz .if_else_0
    ; region arvore_op begin
    xor rax, rax ; unhandled expr
    mov [rbp-24], rax
    mov rax, [rbp-24]
    mov rsp, rbp
    pop rbp
    ret
    ; region arvore_op end
    jmp .if_end_1
.if_else_0:
.if_end_1:
    mov rax, [rbp-16]
    push rax
    mov rax, [rbp-8]
    ; member .valor (TODO: struct offset)
    mov rbx, rax
    pop rax
    cmp rax, rbx
    setl al
    movzx rax, al
    test rax, rax
    jz .if_else_2
    mov rax, [rbp-16]
    push rax
    mov rax, [rbp-8]
    ; member .esquerda (TODO: struct offset)
    push rax
    pop rdi
    pop rsi
    mov rax, inserir
    call rax
    jmp .if_end_3
.if_else_2:
    mov rax, [rbp-16]
    push rax
    mov rax, [rbp-8]
    ; member .direita (TODO: struct offset)
    push rax
    pop rdi
    pop rsi
    mov rax, inserir
    call rax
.if_end_3:
    mov rax, [rbp-8]
    mov rsp, rbp
    pop rbp
    ret
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

buscar:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-8], rdi
    mov [rbp-16], rsi
    mov rax, [rbp-8]
    push rax
    mov rax, null
    mov rbx, rax
    pop rax
    cmp rax, rbx
    sete al
    movzx rax, al
    test rax, rax
    jz .if_else_4
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret
    jmp .if_end_5
.if_else_4:
.if_end_5:
    mov rax, [rbp-8]
    ; member .valor (TODO: struct offset)
    push rax
    mov rax, [rbp-16]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    sete al
    movzx rax, al
    test rax, rax
    jz .if_else_6
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
    jmp .if_end_7
.if_else_6:
.if_end_7:
    mov rax, [rbp-16]
    push rax
    mov rax, [rbp-8]
    ; member .valor (TODO: struct offset)
    mov rbx, rax
    pop rax
    cmp rax, rbx
    setl al
    movzx rax, al
    test rax, rax
    jz .if_else_8
    mov rax, [rbp-16]
    push rax
    mov rax, [rbp-8]
    ; member .esquerda (TODO: struct offset)
    push rax
    pop rdi
    pop rsi
    mov rax, buscar
    call rax
    mov rsp, rbp
    pop rbp
    ret
    jmp .if_end_9
.if_else_8:
.if_end_9:
    mov rax, [rbp-16]
    push rax
    mov rax, [rbp-8]
    ; member .direita (TODO: struct offset)
    push rax
    pop rdi
    pop rsi
    mov rax, buscar
    call rax
    mov rsp, rbp
    pop rbp
    ret
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
