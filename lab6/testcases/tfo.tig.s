.text
.globl L0
.type L0, @function
L0:
subq $L0_framesize, %rsp
movq %rdi, -8(%rbp)
movq %r14, 8(%rsp)
movq %r15, 0(%rsp)
L8:
movq $4, %r15
movq $0, %r14
L6:
cmpq %r15, %r14
jle L5
L1:
movq $0, %rax
jmp L7
L5:
movq %r14, %rdi
call printi
movq $3, %rax
cmpq %rax, %r14
je L2
L3:
movq $0, %rax
L4:
addq $1, %r14
jmp L6
L2:
jmp L1
L9:
movq $0, %rax
jmp L4
L7:
movq 8(%rsp), %r14
movq 0(%rsp), %r15
addq $L0_framesize, %rsp
ret

