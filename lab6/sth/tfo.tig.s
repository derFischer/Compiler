.text
.globl L0
.type L0, @function
L0:
subq $L0_framesize, %rsp
movq %rdi, -8(%rbp)
movq %rbx, -24(%rbp)
movq %rbp, -16(%rbp)
movq %r15, -32(%rbp)
L8:
movq $4, %rbx
movq $0, %r15
L6:
cmpq %rbx, %r15
jle L5
L1:
movq $0, %rax
jmp L7
L5:
movq %r15, %rdi
call printi
movq $3, %r10
cmpq %r10, %r15
je L2
L3:
movq $0, %r10
L4:
addq $1, %r15
jmp L6
L2:
jmp L1
L9:
movq $0, %r10
jmp L4
L7:
movq -24(%rbp), %rbx
movq -16(%rbp), %rbp
movq -32(%rbp), %r15
addq $L0_framesize, %rsp
ret

