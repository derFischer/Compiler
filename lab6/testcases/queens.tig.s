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
.int 2
.string " ."
.section .rodata
.L3:
.int 2
.string " O"
.text
.globl tigermain
.type tigermain, @function
tigermain:
pushq %rbp
movq %rsp, %rbp
subq $48, %rsp
movq %r12, -48(%rbp)
.L34:
movq %rbp, %rax
addq $-8, %rax
movq $8, (%rax)
movq %rbp, %r12
addq $-16, %r12
movq $0, %rax
movq %rbp, %rdi
addq $-8, %rdi
movq (%rdi), %rdi
imulq $2, %rdi
movq $0, %rsi
pushq %rax
call initArray
movq %rax, (%r12)
movq %rbp, %r12
addq $-24, %r12
movq $0, %rax
movq %rbp, %rdi
addq $-8, %rdi
movq (%rdi), %rdi
imulq $2, %rdi
movq $0, %rsi
pushq %rax
call initArray
movq %rax, (%r12)
movq %rbp, %r12
addq $-32, %r12
movq $0, %rax
movq %rbp, %rdi
addq $-8, %rdi
movq (%rdi), %rdi
movq %rbp, %rsi
addq $-8, %rsi
movq (%rsi), %rsi
addq %rsi, %rdi
subq $1, %rdi
imulq $2, %rdi
movq $0, %rsi
pushq %rax
call initArray
movq %rax, (%r12)
movq %rbp, %r12
addq $-40, %r12
movq $0, %rax
movq %rbp, %rdi
addq $-8, %rdi
movq (%rdi), %rdi
movq %rbp, %rsi
addq $-8, %rsi
movq (%rsi), %rsi
addq %rsi, %rdi
subq $1, %rdi
imulq $2, %rdi
movq $0, %rsi
pushq %rax
call initArray
movq %rax, (%r12)
movq $0, %rdi
pushq %rbp
call try
jmp .L33
.L33:
movq -48(%rbp), %r12
movq %rbp, %rsp
popq %rbp
ret

.text
.globl try
.type try, @function
try:
pushq %rbp
movq %rsp, %rbp
subq $40, %rsp
movq %rdi, %rax
movq %rax, -32(%rbp)
movq %r12, -24(%rbp)
movq %r15, -40(%rbp)
movq %rbx, -16(%rbp)
.L36:
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-8, %rax
movq (%rax), %rbx
movq -32(%rbp), %rax
cmpq %rbx, %rax
je .L31
jmp .L32
.L32:
movq %rbp, %rax
addq $-8, %rax
movq $0, (%rax)
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-8, %rax
movq (%rax), %rbx
subq $1, %rbx
.L28:
movq %rbp, %rax
addq $-8, %rax
movq (%rax), %rax
cmpq %rbx, %rax
jle .L29
jmp .L14
.L14:
movq $0, %rax
.L30:
jmp .L35
.L31:
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
pushq %rax
call printboard
jmp .L30
.L29:
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-16, %rax
movq (%rax), %rax
movq %rbp, %r12
addq $-8, %r12
movq (%r12), %r12
imulq $8, %r12
addq %r12, %rax
movq (%rax), %r12
movq $0, %rax
cmpq %rax, %r12
je .L16
jmp .L19
.L19:
movq $0, %rax
.L15:
movq $0, %r12
cmpq %r12, %rax
jne .L21
jmp .L24
.L24:
movq $0, %rax
.L20:
movq $0, %r12
cmpq %r12, %rax
jne .L26
jmp .L27
.L27:
movq $0, %rax
.L25:
movq %rbp, %r12
addq $-8, %r12
movq %rbp, %rax
addq $-8, %rax
movq (%rax), %rax
addq $1, %rax
movq %rax, (%r12)
jmp .L28
.L16:
movq $1, %rax
movq %rbp, %r12
addq $16, %r12
movq (%r12), %r12
addq $-32, %r12
movq (%r12), %r12
movq %rbp, %r15
addq $-8, %r15
movq (%r15), %r15
movq -32(%rbp), %rdi
addq %rdi, %r15
imulq $8, %r15
addq %r15, %r12
movq (%r12), %r15
movq $0, %r12
cmpq %r12, %r15
je .L17
jmp .L18
.L18:
movq $0, %rax
.L17:
jmp .L15
.L21:
movq $1, %rax
movq %rbp, %r12
addq $16, %r12
movq (%r12), %r12
addq $-40, %r12
movq (%r12), %r12
movq %rbp, %r15
addq $-8, %r15
movq (%r15), %r15
addq $7, %r15
movq -32(%rbp), %rdi
subq %rdi, %r15
imulq $8, %r15
addq %r15, %r12
movq (%r12), %r15
movq $0, %r12
cmpq %r12, %r15
je .L22
jmp .L23
.L23:
movq $0, %rax
.L22:
jmp .L20
.L26:
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-16, %rax
movq (%rax), %rax
movq %rbp, %r12
addq $-8, %r12
movq (%r12), %r12
imulq $8, %r12
addq %r12, %rax
movq $1, (%rax)
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-32, %rax
movq (%rax), %rax
movq %rbp, %r12
addq $-8, %r12
movq (%r12), %r12
movq -32(%rbp), %r15
addq %r15, %r12
imulq $8, %r12
addq %r12, %rax
movq $1, (%rax)
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-40, %rax
movq (%rax), %rax
movq %rbp, %r12
addq $-8, %r12
movq (%r12), %r12
addq $7, %r12
movq -32(%rbp), %r15
subq %r15, %r12
imulq $8, %r12
addq %r12, %rax
movq $1, (%rax)
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-24, %rax
movq (%rax), %rax
movq -32(%rbp), %r12
imulq $8, %r12
addq %r12, %rax
movq %rbp, %r12
addq $-8, %r12
movq (%r12), %r12
movq %r12, (%rax)
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
movq -32(%rbp), %rdi
addq $1, %rdi
pushq %rax
call try
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-16, %rax
movq (%rax), %rax
movq %rbp, %r12
addq $-8, %r12
movq (%r12), %r12
imulq $8, %r12
addq %r12, %rax
movq $0, (%rax)
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-32, %rax
movq (%rax), %rax
movq %rbp, %r12
addq $-8, %r12
movq (%r12), %r12
movq -32(%rbp), %r15
addq %r15, %r12
imulq $8, %r12
addq %r12, %rax
movq $0, (%rax)
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-40, %rax
movq (%rax), %rax
movq %rbp, %r12
addq $-8, %r12
movq (%r12), %r12
addq $7, %r12
movq -32(%rbp), %r15
subq %r15, %r12
imulq $8, %r12
addq %r12, %rax
movq $0, (%rax)
movq $0, %rax
jmp .L25
.L35:
movq -24(%rbp), %r12
movq -40(%rbp), %r15
movq -16(%rbp), %rbx
movq %rbp, %rsp
popq %rbp
ret

.text
.globl printboard
.type printboard, @function
printboard:
pushq %rbp
movq %rsp, %rbp
subq $40, %rsp
movq %r12, -32(%rbp)
movq %r15, -24(%rbp)
movq %rbx, -40(%rbp)
.L38:
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
jmp .L37
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
movq %rbp, %r15
addq $16, %r15
movq (%r15), %r15
addq $-24, %r15
movq (%r15), %rdi
movq %rbp, %r15
addq $-8, %r15
movq (%r15), %r15
imulq $8, %r15
addq %r15, %rdi
movq (%rdi), %rdi
movq %rbp, %r15
addq $-16, %r15
movq (%r15), %r15
cmpq %r15, %rdi
je .L6
jmp .L7
.L7:
leaq .L4, %rdi
.L5:
pushq %rax
call print
movq %rbp, %r15
addq $-16, %r15
movq %rbp, %rax
addq $-16, %rax
movq (%rax), %rax
addq $1, %rax
movq %rax, (%r15)
jmp .L8
.L6:
leaq .L3, %rdi
jmp .L5
.L37:
movq -32(%rbp), %r12
movq -24(%rbp), %r15
movq -40(%rbp), %rbx
movq %rbp, %rsp
popq %rbp
ret

