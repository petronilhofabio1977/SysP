
section .data
    str_0 db 67, 105, 114, 99, 117, 108, 111, 32, 114, 97, 105, 111, 61, 53, 44, 32, 97, 114, 101, 97, 32, 97, 112, 114, 111, 120, 58, 10
    str_0_null db 0
    str_0_len equ str_0_null - str_0
    str_1 db 82, 101, 116, 97, 110, 103, 117, 108, 111, 32, 51, 120, 52, 44, 32, 97, 114, 101, 97, 58, 10
    str_1_null db 0
    str_1_len equ str_1_null - str_1
    str_2 db 82, 101, 116, 97, 110, 103, 117, 108, 111, 32, 54, 120, 55, 44, 32, 97, 114, 101, 97, 58, 10
    str_2_null db 0
    str_2_len equ str_2_null - str_2
    str_3 db 99, 32, 101, 32, 67, 105, 114, 99, 117, 108, 111, 63, 10
    str_3_null db 0
    str_3_len equ str_3_null - str_3
    str_4 db 114, 32, 101, 32, 82, 101, 116, 97, 110, 103, 117, 108, 111, 63, 10
    str_4_null db 0
    str_4_len equ str_4_null - str_4

    sysp_str_true  db 't','r','u','e',10
    sysp_str_false db 'f','a','l','s','e',10
section .text
global _start

area_circulo:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-8], rdi
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-8]
    mov rbx, rax
    pop rax
    imul rax, rbx
    push rax
    mov rax, 3
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

area_retangulo:
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

main:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    ; Forma::Circulo c tag=0 base=[rbp-8]
    mov qword [rbp-8], 0
    mov rax, 5
    mov [rbp-16], rax
    ; Forma::Retangulo r tag=1 base=[rbp-24]
    mov qword [rbp-24], 1
    mov rax, 3
    mov [rbp-32], rax
    mov rax, 4
    mov [rbp-40], rax
    ; Forma::Retangulo r2 tag=1 base=[rbp-48]
    mov qword [rbp-48], 1
    mov rax, 6
    mov [rbp-56], rax
    mov rax, 7
    mov [rbp-64], rax
    mov rax, 1
    mov rdi, 1
    mov rsi, str_0
    mov rdx, str_0_len
    syscall
    ; match begin
    mov rax, [rbp-8]
    cmp rax, 0
    jne .match_arm_skip_1
    mov rax, [rbp-16]
    mov [rbp-72], rax    ; bind raio
    mov rax, [rbp-72]
    push rax
    pop rdi
    mov rax, area_circulo
    call rax
    mov rdi, rax
    call sysp_println_int
    jmp .match_end_0
.match_arm_skip_1:
    mov rax, [rbp-8]
    cmp rax, 1
    jne .match_arm_skip_2
    mov rax, [rbp-16]
    mov [rbp-80], rax    ; bind w
    mov rax, [rbp-24]
    mov [rbp-88], rax    ; bind h
    mov rdi, 0
    call sysp_println_int
    jmp .match_end_0
.match_arm_skip_2:
.match_end_0:
    ; match end
    mov rax, 1
    mov rdi, 1
    mov rsi, str_1
    mov rdx, str_1_len
    syscall
    ; match begin
    mov rax, [rbp-24]
    cmp rax, 0
    jne .match_arm_skip_4
    mov rax, [rbp-32]
    mov [rbp-96], rax    ; bind raio
    mov rdi, 0
    call sysp_println_int
    jmp .match_end_3
.match_arm_skip_4:
    mov rax, [rbp-24]
    cmp rax, 1
    jne .match_arm_skip_5
    mov rax, [rbp-32]
    mov [rbp-104], rax    ; bind w
    mov rax, [rbp-40]
    mov [rbp-112], rax    ; bind h
    mov rax, [rbp-112]
    push rax
    mov rax, [rbp-104]
    push rax
    pop rdi
    pop rsi
    mov rax, area_retangulo
    call rax
    mov rdi, rax
    call sysp_println_int
    jmp .match_end_3
.match_arm_skip_5:
.match_end_3:
    ; match end
    mov rax, 1
    mov rdi, 1
    mov rsi, str_2
    mov rdx, str_2_len
    syscall
    ; match begin
    mov rax, [rbp-48]
    cmp rax, 0
    jne .match_arm_skip_7
    mov rax, [rbp-56]
    mov [rbp-120], rax    ; bind raio
    mov rdi, 0
    call sysp_println_int
    jmp .match_end_6
.match_arm_skip_7:
    mov rax, [rbp-48]
    cmp rax, 1
    jne .match_arm_skip_8
    mov rax, [rbp-56]
    mov [rbp-128], rax    ; bind w
    mov rax, [rbp-64]
    mov [rbp-136], rax    ; bind h
    mov rax, [rbp-136]
    push rax
    mov rax, [rbp-128]
    push rax
    pop rdi
    pop rsi
    mov rax, area_retangulo
    call rax
    mov rdi, rax
    call sysp_println_int
    jmp .match_end_6
.match_arm_skip_8:
.match_end_6:
    ; match end
    mov rax, 1
    mov rdi, 1
    mov rsi, str_3
    mov rdx, str_3_len
    syscall
    ; match begin
    mov rax, [rbp-8]
    cmp rax, 0
    jne .match_arm_skip_10
    mov rax, [rbp-16]
    mov [rbp-144], rax    ; bind x
    mov rdi, 1
    call sysp_println_int
    jmp .match_end_9
.match_arm_skip_10:
    mov rax, [rbp-8]
    mov [rbp-152], rax    ; bind _
    mov rdi, 0
    call sysp_println_int
    jmp .match_end_9
.match_arm_skip_11:
.match_end_9:
    ; match end
    mov rax, 1
    mov rdi, 1
    mov rsi, str_4
    mov rdx, str_4_len
    syscall
    ; match begin
    mov rax, [rbp-24]
    cmp rax, 0
    jne .match_arm_skip_13
    mov rax, [rbp-32]
    mov [rbp-160], rax    ; bind x
    mov rdi, 0
    call sysp_println_int
    jmp .match_end_12
.match_arm_skip_13:
    mov rax, [rbp-24]
    cmp rax, 1
    jne .match_arm_skip_14
    mov rax, [rbp-32]
    mov [rbp-168], rax    ; bind w
    mov rax, [rbp-40]
    mov [rbp-176], rax    ; bind h
    mov rdi, 1
    call sysp_println_int
    jmp .match_end_12
.match_arm_skip_14:
.match_end_12:
    ; match end
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
