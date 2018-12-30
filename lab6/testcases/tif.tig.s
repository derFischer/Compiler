.text
.globl L0
.type L0, @function
L0:
subq $L0_framesize, %rsp
L6:
movq %rbp, %rdi
movq $4, %rsi
movq $9, %rdx
call L1
movq %rax, %rdi
call printi
jmp L5
L5:
addq $L0_framesize, %rsp
ret

.text
.globl L1
.type L1, @function
L1:
subq $L1_framesize, %rsp
L8:
cmpq %rdx, %rsi
jg L2
L3:
movq %rdx, %rsi
L4:
movq %rsi, %rax
jmp L7
L2:
jmp L4
L7:
addq $L1_framesize, %rsp
ret

