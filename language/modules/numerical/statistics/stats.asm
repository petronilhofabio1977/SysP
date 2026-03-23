; SysP Compiler Output
; Source: ../../language/modules/numerical/statistics/stats.sp


    sysp_str_true  db 't','r','u','e',10
    sysp_str_false db 'f','a','l','s','e',10
section .text
global _start

media:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-8], rdi
    mov [rbp-16], rsi
    ; float literal 0.0 (TODO: xmm)
    xor rax, rax
    mov [rbp-24], rax
    mov rax, 0
    mov [rbp-32], rax
.for_loop_0:
    mov rax, [rbp-32]
    mov rax, [rbp-16]
    mov rbx, rax
    mov rax, [rbp-32]
    cmp rax, rbx
    jge .for_end_1
    mov rax, [rbp-24]
    push rax
    mov rax, [rbp-32]
    mov rbx, rax
    mov rax, [rbp-8]
    ; array[rbx] (TODO: bounds check)
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-24], rax
    mov rax, [rbp-32]
    inc rax
    mov [rbp-32], rax
    jmp .for_loop_0
.for_end_1:
    mov rax, [rbp-24]
    push rax
    xor rax, rax ; unhandled expr
    mov rbx, rax
    pop rax
    xor rdx, rdx
    div rbx
    mov rsp, rbp
    pop rbp
    ret
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

variancia:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-8], rdi
    mov [rbp-16], rsi
    mov rax, [rbp-16]
    push rax
    mov rax, [rbp-8]
    push rax
    pop rdi
    pop rsi
    mov rax, media
    call rax
    mov [rbp-24], rax
    ; float literal 0.0 (TODO: xmm)
    xor rax, rax
    mov [rbp-32], rax
    mov rax, 0
    mov [rbp-40], rax
.for_loop_2:
    mov rax, [rbp-40]
    mov rax, [rbp-16]
    mov rbx, rax
    mov rax, [rbp-40]
    cmp rax, rbx
    jge .for_end_3
    mov rax, [rbp-40]
    mov rbx, rax
    mov rax, [rbp-8]
    ; array[rbx] (TODO: bounds check)
    push rax
    mov rax, [rbp-24]
    mov rbx, rax
    pop rax
    sub rax, rbx
    mov [rbp-48], rax
    mov rax, [rbp-32]
    push rax
    mov rax, [rbp-48]
    push rax
    mov rax, [rbp-48]
    mov rbx, rax
    pop rax
    imul rax, rbx
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-32], rax
    mov rax, [rbp-40]
    inc rax
    mov [rbp-40], rax
    jmp .for_loop_2
.for_end_3:
    mov rax, [rbp-32]
    push rax
    xor rax, rax ; unhandled expr
    mov rbx, rax
    pop rax
    xor rdx, rdx
    div rbx
    mov rsp, rbp
    pop rbp
    ret
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

maximo:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-8], rdi
    mov [rbp-16], rsi
    mov rax, 0
    mov rbx, rax
    mov rax, [rbp-8]
    ; array[rbx] (TODO: bounds check)
    mov [rbp-24], rax
    mov rax, 1
    mov [rbp-32], rax
.for_loop_4:
    mov rax, [rbp-32]
    mov rax, [rbp-16]
    mov rbx, rax
    mov rax, [rbp-32]
    cmp rax, rbx
    jge .for_end_5
    mov rax, [rbp-32]
    mov rbx, rax
    mov rax, [rbp-8]
    ; array[rbx] (TODO: bounds check)
    push rax
    mov rax, [rbp-24]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    setg al
    movzx rax, al
    test rax, rax
    jz .if_else_6
    mov rax, [rbp-32]
    mov rbx, rax
    mov rax, [rbp-8]
    ; array[rbx] (TODO: bounds check)
    mov [rbp-24], rax
    jmp .if_end_7
.if_else_6:
.if_end_7:
    mov rax, [rbp-32]
    inc rax
    mov [rbp-32], rax
    jmp .for_loop_4
.for_end_5:
    mov rax, [rbp-24]
    mov rsp, rbp
    pop rbp
    ret
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

minimo:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-8], rdi
    mov [rbp-16], rsi
    mov rax, 0
    mov rbx, rax
    mov rax, [rbp-8]
    ; array[rbx] (TODO: bounds check)
    mov [rbp-24], rax
    mov rax, 1
    mov [rbp-32], rax
.for_loop_8:
    mov rax, [rbp-32]
    mov rax, [rbp-16]
    mov rbx, rax
    mov rax, [rbp-32]
    cmp rax, rbx
    jge .for_end_9
    mov rax, [rbp-32]
    mov rbx, rax
    mov rax, [rbp-8]
    ; array[rbx] (TODO: bounds check)
    push rax
    mov rax, [rbp-24]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    setl al
    movzx rax, al
    test rax, rax
    jz .if_else_10
    mov rax, [rbp-32]
    mov rbx, rax
    mov rax, [rbp-8]
    ; array[rbx] (TODO: bounds check)
    mov [rbp-24], rax
    jmp .if_end_11
.if_else_10:
.if_end_11:
    mov rax, [rbp-32]
    inc rax
    mov [rbp-32], rax
    jmp .for_loop_8
.for_end_9:
    mov rax, [rbp-24]
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
