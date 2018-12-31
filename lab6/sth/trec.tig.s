.text
.globl L0
.type L0, @function
L0:
subq $L0_framesize, %rsp
movq %rdi, -8(%rbp)
movq %rbp, -24(%rbp)
movq %r15, -16(%rbp)
L2:
movq $16, %rdi
call malloc
movq %rax, %r15
movq $4, 8(%r15)
movq $3, 0(%r15)
movq 0(%r15), %rdi
call printi
movq 8(%r15), %rdi
call printi
jmp L1
L1:
movq -24(%rbp), %rbp
movq -16(%rbp), %r15
addq $L0_framesize, %rsp
ret

