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
movq %rbp, 0(%rsp)
L12:
movq $1, %r10
movq $3, %rax
cmpq %rax, %rsi
jg L2
L3:
movq $0, %r10
L2:
movq $0, %r11
cmpq %r11, %r10
jne L5
L6:
movq -8(%rbp), %rax
addq $-16, %rax
movq $4, (%rax)
movq $0, %rax
L7:
jmp L11
L5:
leaq L4, %rdi
call print
jmp L7
L11:
movq 0(%rsp), %rbp
addq $L1_framesize, %rsp
ret

.section .rodata
L4:
.int 20
.string "hey! Bigger than 3!\n"
