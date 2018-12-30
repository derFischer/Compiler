.text
.globl L0
.type L0, @function
L0:
subq $L0_framesize, %rsp
movq %rdi, -8(%rbp)
movq %rbp, 8(%rsp)
movq %r12, 16(%rsp)
movq %r13, 24(%rsp)
movq %r14, 32(%rsp)
movq %r15, 40(%rsp)
L2:
movq $16, %rdi
call malloc
movq $4, 8(%rax)
movq $3, 0(%rax)
movq %rax, 0(%rsp)
movq 0(%rsp), %r12
movq 0(%r12), %rdi
call printi
movq 0(%rsp), %r12
movq 8(%r12), %rdi
call printi
jmp L1
L1:
movq 8(%rsp), %rbp
movq 16(%rsp), %r12
movq 24(%rsp), %r13
movq 32(%rsp), %r14
movq 40(%rsp), %r15
addq $L0_framesize, %rsp
ret

