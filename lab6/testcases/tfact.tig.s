.text
.globl tigermain
.type tigermain, @function
tigermain:
pushq %rbp
movq %rsp, %rbp
subq $8, %rsp
movq %r12, -8(%rbp)
.L5:
movq %rbp, %r12
movq $10, %rdi
pushq %rbp
call nfactor
pushq %r12
movq %rax, %rdi
call printi
jmp .L4
.L4:
movq -8(%rbp), %r12
movq %rbp, %rsp
popq %rbp
ret

.text
.globl nfactor
.type nfactor, @function
nfactor:
pushq %rbp
movq %rsp, %rbp
subq $8, %rsp
movq %r12, -8(%rbp)
.L7:
movq $0, %rax
cmpq %rax, %rdi
je .L2
jmp .L3
.L3:
movq %rdi, %r12
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
subq $1, %rdi
pushq %rax
call nfactor
imulq %rax, %r12
.L1:
movq %r12, %rax
jmp .L6
.L2:
movq $1, %r12
jmp .L1
.L6:
movq -8(%rbp), %r12
movq %rbp, %rsp
popq %rbp
ret

