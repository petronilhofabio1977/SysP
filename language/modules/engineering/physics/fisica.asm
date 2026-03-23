; SysP Compiler Output
; Source: ../../language/modules/engineering/physics/fisica.sp


    sysp_str_true  db 't','r','u','e',10
    sysp_str_false db 'f','a','l','s','e',10
section .text
global _start

energia_cinetica:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-8], rdi
    mov [rbp-16], rsi
    ; float literal 0.5 (TODO: xmm)
    xor rax, rax
    push rax
    mov rax, [rbp-8]
    mov rbx, rax
    pop rax
    imul rax, rbx
    push rax
    mov rax, [rbp-16]
    mov rbx, rax
    pop rax
    imul rax, rbx
    push rax
    mov rax, [rbp-16]
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

energia_potencial:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-8], rdi
    mov [rbp-16], rsi
    mov rax, [rbp-8]
    push rax
    mov rax, G
    mov rbx, rax
    pop rax
    imul rax, rbx
    push rax
    mov rax, [rbp-16]
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

forca:
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
    imul rax, rbx
    mov rsp, rbp
    pop rbp
    ret
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

velocidade_final:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-8], rdi
    mov [rbp-16], rsi
    mov [rbp-24], rdx
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    push rax
    mov rax, [rbp-24]
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

distancia:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-8], rdi
    mov [rbp-16], rsi
    mov [rbp-24], rdx
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-24]
    mov rbx, rax
    pop rax
    imul rax, rbx
    push rax
    ; float literal 0.5 (TODO: xmm)
    xor rax, rax
    push rax
    mov rax, [rbp-16]
    mov rbx, rax
    pop rax
    imul rax, rbx
    push rax
    mov rax, [rbp-24]
    mov rbx, rax
    pop rax
    imul rax, rbx
    push rax
    mov rax, [rbp-24]
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
