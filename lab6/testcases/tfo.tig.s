.text
.globl L0
.type L0, @function
L0:
subq $L0_framesize, %rsp
movq %rdi, -8(%rbp)
movq %rbx, 56(%rsp)
movq %rbp, 48(%rsp)
movq %r12, 40(%rsp)
movq %r13, 32(%rsp)
movq %r14, 24(%rsp)
L8:
movq $4, %rax
movq %rax, 8(%rsp)
movq $0, %rbx
movq %rbx, 0(%rsp)
L6:
movq 0(%rsp), %rbx
movq 8(%rsp), %rax
cmpq %rax, %rbx
jle L5
L1:
movq $0, %rax
jmp L7
L5:
movq 0(%rsp), %rdi
call printi
movq $3, %rbx
movq 0(%rsp), %rax
cmpq %rbx, %rax
je L2
L3:
movq $0, %rbx
L4:
movq 0(%rsp), %rax
addq $1, %rax
movq %rax, 0(%rsp)
jmp L6
L2:
jmp L1
L9:
movq $0, %rbx
jmp L4
L7:
movq 56(%rsp), %rbx
movq 48(%rsp), %rbp
movq 40(%rsp), %r12
movq 32(%rsp), %r13
movq 24(%rsp), %r14
addq $L0_framesize, %rsp
ret

