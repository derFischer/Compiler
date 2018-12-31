.text
.globl L0
.type L0, @function
L0:
subq $L0_framesize, %rsp
L6:
movq $10, %rdi
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
ret

.text
.globl L1
.type L1, @function
L1:
subq $L1_framesize, %rsp
L8:
movq $0, %r10
cmpq %r10, %rdi
je L2
L3:
movq %rdi, 8(%rsp)
movq 8(%rbp), %r10
subq $1, %rdi
push %r10
call L1
addq $8, %rsp
movq %rax, %r10
movq 8(%rsp), %rax
imulq %r10, %rax
L4:
jmp L7
L2:
movq $1, %rax
jmp L4
L7:
addq $L1_framesize, %rsp
ret

