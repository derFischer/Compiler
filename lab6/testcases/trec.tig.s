.text
.globl L0
.type L0, @function
L0:
subq $L0_framesize, %rsp
movq %rdi, -8(%rbp)
L2:
movq $16, %rdi
call malloc
movq %rax, 0(%rsp)
movq 0(%rsp), %r10
movq 0(%r10), %rdi
call printi
movq 0(%rsp), %r10
movq 8(%r10), %rdi
call printi
jmp L1
L1:
addq $L0_framesize, %rsp
ret

