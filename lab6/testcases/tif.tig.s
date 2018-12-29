.text
.globl L0
.type L0, @function
L0:
subq $L0_framesize, %rsp
L6:
movq $4, %rdi
movq $9, %rsi
push %rbp
call L1
addq $8, %rsp
movq %rax, %rdi
push %rbp
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
cmpq %rsi, %rdi
jg L2
L3:
movq %rsi, %rdi
L4:
movq %rdi, %rax
jmp L7
L2:
jmp L4
L7:
addq $L1_framesize, %rsp

