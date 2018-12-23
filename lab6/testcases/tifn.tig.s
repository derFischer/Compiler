.section .rodata
.L7:
.int 1
.string "\n"
.section .rodata
.L3:
.int 20
.string "hey! Bigger than 3!\n"
.text
.globl tigermain
.type tigermain, @function
tigermain:
pushq %rbp
movq %rsp, %rbp
subq $8, %rsp
.L9:
movq %rbp, %rax
addq $-8, %rax
movq $5, (%rax)
movq %rbp, %rax
addq $-8, %rax
movq (%rax), %rdi
pushq %rbp
call printi
leaq .L7, %rdi
pushq %rbp
call print
movq $2, %rdi
pushq %rbp
call g
movq %rbp, %rax
addq $-8, %rax
movq (%rax), %rdi
pushq %rbp
call printi
jmp .L8
.L8:
movq %rbp, %rsp
popq %rbp
ret

.text
.globl g
.type g, @function
g:
pushq %rbp
movq %rsp, %rbp
subq $8, %rsp
movq %r12, -8(%rbp)
.L11:
movq $1, %r12
movq $3, %rax
cmpq %rax, %rdi
jg .L1
jmp .L2
.L2:
movq $0, %r12
.L1:
movq $0, %rax
cmpq %rax, %r12
jne .L5
jmp .L6
.L6:
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-8, %rax
movq $4, (%rax)
movq $0, %rax
.L4:
jmp .L10
.L5:
leaq .L3, %rdi
pushq %rbp
call print
jmp .L4
.L10:
movq -8(%rbp), %r12
movq %rbp, %rsp
popq %rbp
ret

