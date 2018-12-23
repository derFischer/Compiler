.section .rodata
.L11:
.int 1
.string "\n"
.section .rodata
.L1:
.int 0
.string ""
.text
.globl tigermain
.type tigermain, @function
tigermain:
pushq %rbp
movq %rsp, %rbp
subq $24, %rsp
movq %r12, -24(%rbp)
.L13:
movq %rbp, %rax
addq $-8, %rax
movq $16, (%rax)
movq %rbp, %r12
addq $-16, %r12
movq $0, %rax
movq %rbp, %rdi
addq $-8, %rdi
movq (%rdi), %rdi
imulq $2, %rdi
movq $0, %rsi
pushq %rax
call initArray
movq %rax, (%r12)
pushq %rbp
call try
jmp .L12
.L12:
movq -24(%rbp), %r12
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
.L15:
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
pushq %rax
call init
movq %rbp, %rbx
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
movq $0, %rdi
movq %rbp, %rsi
addq $16, %rsi
movq (%rsi), %rsi
addq $-8, %rsi
movq (%rsi), %rsi
subq $1, %rsi
movq $7, %rdx
pushq %rax
call bsearch
pushq %rbx
movq %rax, %rdi
call printi
leaq .L11, %rdi
pushq %rbp
call print
jmp .L14
.L14:
movq -8(%rbp), %rbx
movq %rbp, %rsp
popq %rbp
ret

.text
.globl bsearch
.type bsearch, @function
bsearch:
pushq %rbp
movq %rsp, %rbp
subq $16, %rsp
movq %rdi, -8(%rbp)
movq %rdx, -16(%rbp)
.L17:
movq -8(%rbp), %rax
cmpq %rsi, %rax
je .L9
jmp .L10
.L10:
movq -8(%rbp), %rax
addq %rsi, %rax
movq $2, %rdi
cqto
idivq %rdi
movq %rbp, %rdi
addq $16, %rdi
movq (%rdi), %rdi
addq $-16, %rdi
movq (%rdi), %rdx
movq %rax, %rdi
imulq $8, %rdi
addq %rdi, %rdx
movq (%rdx), %rdx
movq -16(%rbp), %rdi
cmpq %rdi, %rdx
jl .L6
jmp .L7
.L7:
movq %rbp, %rdi
addq $16, %rdi
movq (%rdi), %rdi
pushq %rdi
movq -8(%rbp), %rdi
movq %rax, %rsi
movq -16(%rbp), %rdx
call bsearch
.L5:
.L8:
jmp .L16
.L9:
movq -8(%rbp), %rax
jmp .L8
.L6:
movq %rbp, %rdi
addq $16, %rdi
movq (%rdi), %rdi
addq $1, %rax
pushq %rdi
movq %rax, %rdi
movq -16(%rbp), %rdx
call bsearch
jmp .L5
.L16:
movq %rbp, %rsp
popq %rbp
ret

.text
.globl init
.type init, @function
init:
pushq %rbp
movq %rsp, %rbp
subq $24, %rsp
movq %r12, -16(%rbp)
movq %rbx, -24(%rbp)
.L19:
movq %rbp, %rax
addq $-8, %rax
movq $0, (%rax)
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-8, %rax
movq (%rax), %rbx
subq $1, %rbx
.L3:
movq %rbp, %rax
addq $-8, %rax
movq (%rax), %rax
cmpq %rbx, %rax
jle .L4
jmp .L2
.L2:
movq $0, %rax
jmp .L18
.L4:
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-16, %rax
movq (%rax), %rax
movq %rbp, %r12
addq $-8, %r12
movq (%r12), %r12
imulq $8, %r12
addq %r12, %rax
movq %rbp, %r12
addq $-8, %r12
movq (%r12), %r12
imulq $2, %r12
addq $1, %r12
movq %r12, (%rax)
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
pushq %rax
call nop
movq %rbp, %rax
addq $-8, %rax
movq %rbp, %r12
addq $-8, %r12
movq (%r12), %r12
addq $1, %r12
movq %r12, (%rax)
jmp .L3
.L18:
movq -16(%rbp), %r12
movq -24(%rbp), %rbx
movq %rbp, %rsp
popq %rbp
ret

.text
.globl nop
.type nop, @function
nop:
pushq %rbp
movq %rsp, %rbp
subq $0, %rsp
.L21:
leaq .L1, %rdi
pushq %rbp
call print
jmp .L20
.L20:
movq %rbp, %rsp
popq %rbp
ret

