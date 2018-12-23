.text
.globl tigermain
.type tigermain, @function
tigermain:
pushq %rbp
movq %rsp, %rbp
subq $8, %rsp
movq %r12, -8(%rbp)
.L2:
movq %rbp, %r12
movq $2, %rdi
pushq %rbp
call a
pushq %r12
movq %rax, %rdi
call printi
jmp .L1
.L1:
movq -8(%rbp), %r12
movq %rbp, %rsp
popq %rbp
ret

.text
.globl a
.type a, @function
a:
pushq %rbp
movq %rsp, %rbp
subq $8, %rsp
movq %rbp, %rax
addq $-8, %rax
movq %rdi, (%rax)
.L4:
movq $3, %rdi
pushq %rbp
call b
jmp .L3
.L3:
movq %rbp, %rsp
popq %rbp
ret

.text
.globl b
.type b, @function
b:
pushq %rbp
movq %rsp, %rbp
subq $0, %rsp
.L6:
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-8, %rax
movq (%rax), %rax
addq %rdi, %rax
jmp .L5
.L5:
movq %rbp, %rsp
popq %rbp
ret

