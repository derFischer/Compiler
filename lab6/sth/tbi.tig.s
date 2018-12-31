.section .rodata
.L13:
.int 1
.string "\n"
.section .rodata
.L10:
.int 1
.string "\n"
.section .rodata
.L4:
.int 1
.string "y"
.section .rodata
.L3:
.int 1
.string "x"
.text
.globl tigermain
.type tigermain, @function
tigermain:
pushq %rbp
movq %rsp, %rbp
subq $8, %rsp
.L15:
movq %rbp, %rax
addq $-8, %rax
movq $8, (%rax)
pushq %rbp
call printb
jmp .L14
.L14:
movq %rbp, %rsp
popq %rbp
ret

.text
.globl printb
.type printb, @function
printb:
pushq %rbp
movq %rsp, %rbp
subq $40, %rsp
movq %r12, -32(%rbp)
movq %r13, -24(%rbp)
movq %rbx, -40(%rbp)
.L17:
movq %rbp, %rax
addq $-8, %rax
movq $0, (%rax)
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-8, %rax
movq (%rax), %rbx
subq $1, %rbx
.L11:
movq %rbp, %rax
addq $-8, %rax
movq (%rax), %rax
cmpq %rbx, %rax
jle .L12
jmp .L1
.L1:
leaq .L13, %rdi
pushq %rbp
call print
jmp .L16
.L12:
movq %rbp, %rax
addq $-16, %rax
movq $0, (%rax)
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-8, %rax
movq (%rax), %r12
subq $1, %r12
.L8:
movq %rbp, %rax
addq $-16, %rax
movq (%rax), %rax
cmpq %r12, %rax
jle .L9
jmp .L2
.L2:
leaq .L10, %rdi
pushq %rbp
call print
movq %rbp, %r12
addq $-8, %r12
movq %rbp, %rax
addq $-8, %rax
movq (%rax), %rax
addq $1, %rax
movq %rax, (%r12)
jmp .L11
.L9:
movq %rbp, %rax
movq %rbp, %r13
addq $-8, %r13
movq (%r13), %rdi
movq %rbp, %r13
addq $-16, %r13
movq (%r13), %r13
cmpq %r13, %rdi
jg .L6
jmp .L7
.L7:
leaq .L4, %rdi
.L5:
pushq %rax
call print
movq %rbp, %r13
addq $-16, %r13
movq %rbp, %rax
addq $-16, %rax
movq (%rax), %rax
addq $1, %rax
movq %rax, (%r13)
jmp .L8
.L6:
leaq .L3, %rdi
jmp .L5
.L16:
movq -32(%rbp), %r12
movq -24(%rbp), %r13
movq -40(%rbp), %rbx
movq %rbp, %rsp
popq %rbp
ret

