.text
.globl L0
.type L0, @function
L0:
subq $L0_framesize, %rsp
L8:
movq $10, %rax
movq %rax, 8(%rsp)
L5:
movq $0, %rax
movq 8(%rsp), %r10
cmpq %rax, %r10
jge L6
L1:
movq $0, %rax
jmp L7
L6:
movq 8(%rsp), %rdi
push %rbp
call printi
addq $8, %rsp
movq 8(%rsp), %r10
subq $1, %r10
movq %r10, 8(%rsp)
movq $2, %rax
movq 8(%rsp), %r10
cmpq %rax, %r10
je L2
L3:
movq $0, %rax
L4:
jmp L5
L2:
jmp L1
L9:
movq $0, %rax
jmp L4
L7:
addq $L0_framesize, %rsp
ret

