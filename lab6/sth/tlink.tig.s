.text
.globl L0
.type L0, @function
L0:
subq $L0_framesize, %rsp
movq %rdi, -8(%rbp)
L4:
movq %rbp, %rdi
movq $2, %rsi
call L1
movq %rax, %rdi
call printi
jmp L3
L3:
addq $L0_framesize, %rsp
ret

.text
.globl L1
.type L1, @function
L1:
subq $L1_framesize, %rsp
movq %rdi, -8(%rbp)
movq %rsi, -16(%rbp)
L6:
movq %rbp, %rdi
movq $3, %rsi
call L2
jmp L5
L5:
addq $L1_framesize, %rsp
ret

.text
.globl L2
.type L2, @function
L2:
subq $L2_framesize, %rsp
movq %rdi, -8(%rbp)
L8:
movq -8(%rbp), %rax
addq $-16, %rax
movq (%rax), %rax
addq %rsi, %rax
jmp L7
L7:
addq $L2_framesize, %rsp
ret

