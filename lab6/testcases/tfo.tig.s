.text
.globl L0
.type L0, @function
L0:
subq $L0_framesize, %rsp
L8:
movq $4, %rbx
movq $0, %r9
L6:
cmpq %rbx, %r9
jle L5
L1:
movq $0, %rbx
movq %rbx, %rax
jmp L7
L5:
movq %r9, %rdi
push %rbp
call printi
addq $8, %rsp
movq $3, %r12
cmpq %r12, %r9
je L2
L3:
movq $0, %r12
L4:
addq $1, %r9
jmp L6
L2:
jmp L1
L9:
movq $0, %r12
jmp L4
L7:
addq $L0_framesize, %rsp

