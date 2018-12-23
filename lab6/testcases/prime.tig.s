.section .rodata
.L17:
.int 1
.string "\n"
.section .rodata
.L16:
.int 1
.string "\n"
.section .rodata
.L15:
.int 1
.string "\n"
.section .rodata
.L14:
.int 1
.string "\n"
.section .rodata
.L13:
.int 1
.string "\n"
.section .rodata
.L12:
.int 1
.string "\n"
.section .rodata
.L11:
.int 1
.string "\n"
.section .rodata
.L10:
.int 1
.string "\n"
.section .rodata
.L9:
.int 1
.string "\n"
.section .rodata
.L8:
.int 1
.string "\n"
.section .rodata
.L7:
.int 1
.string "\n"
.text
.globl tigermain
.type tigermain, @function
tigermain:
pushq %rbp
movq %rsp, %rbp
subq $0, %rsp
.L19:
pushq %rbp
call try
jmp .L18
.L18:
movq %rbp, %rsp
popq %rbp
ret

.text
.globl try
.type try, @function
try:
pushq %rbp
movq %rsp, %rbp
subq $8, %rsp
movq %rbx, -8(%rbp)
.L21:
movq %rbp, %rbx
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
movq $56, %rdi
pushq %rax
call check
pushq %rbx
movq %rax, %rdi
call printi
leaq .L7, %rdi
pushq %rbp
call print
movq %rbp, %rbx
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
movq $23, %rdi
pushq %rax
call check
pushq %rbx
movq %rax, %rdi
call printi
leaq .L8, %rdi
pushq %rbp
call print
movq %rbp, %rbx
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
movq $71, %rdi
pushq %rax
call check
pushq %rbx
movq %rax, %rdi
call printi
leaq .L9, %rdi
pushq %rbp
call print
movq %rbp, %rbx
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
movq $72, %rdi
pushq %rax
call check
pushq %rbx
movq %rax, %rdi
call printi
leaq .L10, %rdi
pushq %rbp
call print
movq %rbp, %rbx
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
movq $173, %rdi
pushq %rax
call check
pushq %rbx
movq %rax, %rdi
call printi
leaq .L11, %rdi
pushq %rbp
call print
movq %rbp, %rbx
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
movq $181, %rdi
pushq %rax
call check
pushq %rbx
movq %rax, %rdi
call printi
leaq .L12, %rdi
pushq %rbp
call print
movq %rbp, %rbx
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
movq $281, %rdi
pushq %rax
call check
pushq %rbx
movq %rax, %rdi
call printi
leaq .L13, %rdi
pushq %rbp
call print
movq %rbp, %rbx
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
movq $659, %rdi
pushq %rax
call check
pushq %rbx
movq %rax, %rdi
call printi
leaq .L14, %rdi
pushq %rbp
call print
movq %rbp, %rbx
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
movq $729, %rdi
pushq %rax
call check
pushq %rbx
movq %rax, %rdi
call printi
leaq .L15, %rdi
pushq %rbp
call print
movq %rbp, %rbx
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
movq $947, %rdi
pushq %rax
call check
pushq %rbx
movq %rax, %rdi
call printi
leaq .L16, %rdi
pushq %rbp
call print
movq %rbp, %rbx
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
movq $945, %rdi
pushq %rax
call check
pushq %rbx
movq %rax, %rdi
call printi
leaq .L17, %rdi
pushq %rbp
call print
jmp .L20
.L20:
movq -8(%rbp), %rbx
movq %rbp, %rsp
popq %rbp
ret

.text
.globl check
.type check, @function
check:
pushq %rbp
movq %rsp, %rbp
subq $8, %rsp
.L23:
movq $1, %r10
movq %rbp, %rax
addq $-8, %rax
movq $2, (%rax)
movq $2, %r11
movq %rdi, %rax
cqto
idivq %r11
movq %rax, %r11
.L5:
movq %rbp, %rax
addq $-8, %rax
movq (%rax), %rax
cmpq %r11, %rax
jle .L6
jmp .L1
.L1:
movq %r10, %rax
jmp .L22
.L6:
movq %rbp, %rax
addq $-8, %rax
movq (%rax), %rsi
movq %rdi, %rax
cqto
idivq %rsi
movq %rbp, %rsi
addq $-8, %rsi
movq (%rsi), %rsi
imulq %rsi, %rax
cmpq %rdi, %rax
je .L3
jmp .L4
.L4:
movq $0, %rax
.L2:
movq %rbp, %rsi
addq $-8, %rsi
movq %rbp, %rax
addq $-8, %rax
movq (%rax), %rax
addq $1, %rax
movq %rax, (%rsi)
jmp .L5
.L3:
movq $0, %r10
jmp .L1
.L24:
movq $0, %rax
jmp .L2
.L22:
movq %rbp, %rsp
popq %rbp
ret

