.text
.globl L0
.type L0, @function
L0:
subq $L0_framesize, %rsp
L6:
movq %rbp, %rbx
movq $4, %r9
movq %r9, %rdi
movq $9, %r9
movq %r9, %rsi
push %rbp
call L1
addq $8, %rsp
movq %rax, %r9
movq %r9, %rdi
push %rbx
call printi
addq $8, %rsp
jmp L5
L5:
addq $L0_framesize, %rsp

.text
.globl L1
.type L1, @function
L1:
subq $L1_framesize, %rsp
L8:
movq 8(%rbp), %rbx
cmpq %rbx, %r9
jgt L2
L3:
movq 8(%rbp), %rbx
L4:
movq %rbx, %rax
jmp L7
L2:
movq %r9, %rbx
jmp L4
L7:
addq $L1_framesize, %rsp

