.text
.globl L0
.type L0, @function
L0:
subq $L0_framesize, %rsp
L8:
movq $4, %rbx
movq $0, %r8
L6:
cmpq %rbx, %r8
jle L5
L1:
movq $0, %r8
movq %r8, %rax
jmp L7
L5:
movq %r8, %rdi
push %rbp
call printi
addq $8, %rsp
movq $0, %r8
cmpq %r8, %r12
jne L2
L3:
movq $0, %rbx
L4:
movq %r9, %r8
jmp L6
L2:
jmp L1
L9:
movq $0, %rbx
jmp L4
L7:
addq $L0_framesize, %rsp

