
section .data
    str_0 db 109, 97, 116, 99, 104, 32, 114, 49, 32, 40, 79, 107, 32, 52, 50, 41, 58, 10
    str_0_null db 0
    str_0_len equ str_0_null - str_0
    str_1 db 101, 114, 114, 111, 10
    str_1_null db 0
    str_1_len equ str_1_null - str_1
    str_2 db 109, 97, 116, 99, 104, 32, 114, 50, 32, 40, 69, 114, 114, 32, 48, 41, 58, 10
    str_2_null db 0
    str_2_len equ str_2_null - str_2
    str_3 db 109, 97, 116, 99, 104, 32, 115, 49, 32, 40, 83, 111, 109, 101, 32, 55, 41, 58, 10
    str_3_null db 0
    str_3_len equ str_3_null - str_3
    str_4 db 109, 97, 116, 99, 104, 32, 115, 50, 32, 40, 78, 111, 110, 101, 41, 58, 10
    str_4_null db 0
    str_4_len equ str_4_null - str_4
    str_5 db 109, 97, 116, 99, 104, 32, 120, 32, 40, 51, 41, 58, 10
    str_5_null db 0
    str_5_len equ str_5_null - str_5
    str_6 db 117, 109, 10
    str_6_null db 0
    str_6_len equ str_6_null - str_6
    str_7 db 100, 111, 105, 115, 10
    str_7_null db 0
    str_7_len equ str_7_null - str_7
    str_8 db 116, 114, 101, 115, 10
    str_8_null db 0
    str_8_len equ str_8_null - str_8
    str_9 db 111, 117, 116, 114, 111, 10
    str_9_null db 0
    str_9_len equ str_9_null - str_9
    str_10 db 49, 48, 32, 47, 32, 50, 32, 61, 10
    str_10_null db 0
    str_10_len equ str_10_null - str_10
    str_11 db 49, 48, 32, 47, 32, 48, 32, 61, 10
    str_11_null db 0
    str_11_len equ str_11_null - str_11

    sysp_str_true  db 't','r','u','e',10
    sysp_str_false db 'f','a','l','s','e',10
section .text
global _start

divide:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-8], rdi
    mov [rbp-16], rsi
    mov rax, [rbp-16]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    sete al
    movzx rax, al
    test rax, rax
    jz .if_else_0
    mov rax, 1
    neg rax
    mov rsp, rbp
    pop rbp
    ret
    jmp .if_end_1
.if_else_0:
.if_end_1:
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
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

classifica:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-8], rdi
    mov rax, [rbp-8]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    setg al
    movzx rax, al
    test rax, rax
    jz .if_else_2
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
    jmp .if_end_3
.if_else_2:
.if_end_3:
    mov rax, [rbp-8]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    setl al
    movzx rax, al
    test rax, rax
    jz .if_else_4
    mov rax, 1
    neg rax
    mov rsp, rbp
    pop rbp
    ret
    jmp .if_end_5
.if_else_4:
.if_end_5:
    mov rax, 0
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
    ; enum var r1 tag=0 base=[rbp-8]
    mov qword [rbp-8], 0
    mov rax, 42
    mov [rbp-16], rax
    ; enum var r2 tag=1 base=[rbp-24]
    mov qword [rbp-24], 1
    mov rax, 0
    mov [rbp-32], rax
    ; enum var r3 tag=0 base=[rbp-40]
    mov qword [rbp-40], 0
    mov rax, 100
    mov [rbp-48], rax
    mov rax, 1
    mov rdi, 1
    mov rsi, str_0
    mov rdx, str_0_len
    syscall
    ; match begin
    mov rax, [rbp-8]
    test rax, rax
    jnz .match_arm_skip_7
    mov rax, [rbp-16]
    mov [rbp-56], rax    ; bind v
    mov rax, [rbp-56]
    mov rdi, rax
    call sysp_println_int
    jmp .match_end_6
.match_arm_skip_7:
    mov rax, [rbp-8]
    cmp rax, 1
    jne .match_arm_skip_8
    mov rax, [rbp-16]
    mov [rbp-64], rax    ; bind e
    mov rax, 1
    mov rdi, 1
    mov rsi, str_1
    mov rdx, str_1_len
    syscall
    jmp .match_end_6
.match_arm_skip_8:
.match_end_6:
    ; match end
    mov rax, 1
    mov rdi, 1
    mov rsi, str_2
    mov rdx, str_2_len
    syscall
    ; match begin
    mov rax, [rbp-24]
    test rax, rax
    jnz .match_arm_skip_10
    mov rax, [rbp-32]
    mov [rbp-72], rax    ; bind v
    mov rax, [rbp-72]
    mov rdi, rax
    call sysp_println_int
    jmp .match_end_9
.match_arm_skip_10:
    mov rax, [rbp-24]
    cmp rax, 1
    jne .match_arm_skip_11
    mov rax, [rbp-32]
    mov [rbp-80], rax    ; bind e
    mov rax, [rbp-80]
    mov rdi, rax
    call sysp_println_int
    jmp .match_end_9
.match_arm_skip_11:
.match_end_9:
    ; match end
    ; enum var s1 tag=1 base=[rbp-88]
    mov qword [rbp-88], 1
    mov rax, 7
    mov [rbp-96], rax
    ; enum var s2 tag=0 base=[rbp-104]
    mov qword [rbp-104], 0
    mov rax, 1
    mov rdi, 1
    mov rsi, str_3
    mov rdx, str_3_len
    syscall
    ; match begin
    mov rax, [rbp-88]
    cmp rax, 1
    jne .match_arm_skip_13
    mov rax, [rbp-96]
    mov [rbp-112], rax    ; bind n
    mov rax, [rbp-112]
    mov rdi, rax
    call sysp_println_int
    jmp .match_end_12
.match_arm_skip_13:
    mov rax, [rbp-88]
    test rax, rax
    jnz .match_arm_skip_14
    mov rdi, 0
    call sysp_println_int
    jmp .match_end_12
.match_arm_skip_14:
.match_end_12:
    ; match end
    mov rax, 1
    mov rdi, 1
    mov rsi, str_4
    mov rdx, str_4_len
    syscall
    ; match begin
    mov rax, [rbp-104]
    cmp rax, 1
    jne .match_arm_skip_16
    mov rax, [rbp-112]
    mov [rbp-120], rax    ; bind n
    mov rax, [rbp-120]
    mov rdi, rax
    call sysp_println_int
    jmp .match_end_15
.match_arm_skip_16:
    mov rax, [rbp-104]
    test rax, rax
    jnz .match_arm_skip_17
    mov rdi, 0
    call sysp_println_int
    jmp .match_end_15
.match_arm_skip_17:
.match_end_15:
    ; match end
    mov rax, 3
    mov [rbp-128], rax
    mov rax, 1
    mov rdi, 1
    mov rsi, str_5
    mov rdx, str_5_len
    syscall
    mov rax, [rbp-128]
    mov [rbp-136], rax    ; match subject
    ; match begin
    mov rax, [rbp-136]
    cmp rax, 1
    jne .match_arm_skip_19
    mov rax, 1
    mov rdi, 1
    mov rsi, str_6
    mov rdx, str_6_len
    syscall
    jmp .match_end_18
.match_arm_skip_19:
    mov rax, [rbp-136]
    cmp rax, 2
    jne .match_arm_skip_20
    mov rax, 1
    mov rdi, 1
    mov rsi, str_7
    mov rdx, str_7_len
    syscall
    jmp .match_end_18
.match_arm_skip_20:
    mov rax, [rbp-136]
    cmp rax, 3
    jne .match_arm_skip_21
    mov rax, 1
    mov rdi, 1
    mov rsi, str_8
    mov rdx, str_8_len
    syscall
    jmp .match_end_18
.match_arm_skip_21:
    mov rax, [rbp-136]
    mov [rbp-144], rax    ; bind _
    mov rax, 1
    mov rdi, 1
    mov rsi, str_9
    mov rdx, str_9_len
    syscall
    jmp .match_end_18
.match_arm_skip_22:
.match_end_18:
    ; match end
    mov rax, 1
    mov rdi, 1
    mov rsi, str_10
    mov rdx, str_10_len
    syscall
    mov rax, 2
    push rax
    mov rax, 10
    push rax
    pop rdi
    pop rsi
    mov rax, divide
    call rax
    mov rdi, rax
    call sysp_println_int
    mov rax, 1
    mov rdi, 1
    mov rsi, str_11
    mov rdx, str_11_len
    syscall
    mov rax, 0
    push rax
    mov rax, 10
    push rax
    pop rdi
    pop rsi
    mov rax, divide
    call rax
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
