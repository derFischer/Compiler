.text
.globl L0
.type L0, @function
L0:
subq $L0_framesize, %rsp
movq %rdi, -8(%rbp)
L10:
movq $5, -16(%rbp)
movq -16(%rbp), %rdi
call printi
leaq L8, %rdi
call print
movq %rbp, %rdi
movq $2, %rsi
call L1
movq -16(%rbp), %rdi
call printi
jmp L9
L9:
addq $L0_framesize, %rsp
ret

.section .rodata
L8:
.int 1
.string "\n"
.text
.globl L1
.type L1, @function
L1:
subq $L1_framesize, %rsp
movq %rdi, -8(%rbp)
L12:
movq $1, %rax
movq $3, %r10
cmpq %r10, %rsi
jg L2
L3:
movq $0, %rax
L2:
movq $0, %r9
cmpq %r9, %rax
jne L5
L6:
movq -8(%rbp), %r10
addq $-16, %r10
movq $4, (%r10)
movq $0, %rax
L7:
jmp L11
L5:
leaq L4, %rdi
call print
jmp L7
L11:
addq $L1_framesize, %rsp
ret

.section .rodata
L4:
.int 20
.string "hey! Bigger than 3!\n"
