.section .rodata
.L31:
.int 1
.string "\n"
.section .rodata
.L28:
.int 1
.string " "
.section .rodata
.L1:
.int 0
.string ""
.text
.globl tigermain
.type tigermain, @function
tigermain:
pushq %rbp
movq %rsp, %rbp
subq $24, %rsp
movq %r12, -24(%rbp)
.L33:
movq %rbp, %rax
addq $-8, %rax
movq $16, (%rax)
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
pushq %rbp
call dosort
jmp .L32
.L32:
movq -24(%rbp), %r12
movq %rbp, %rsp
popq %rbp
ret

.text
.globl dosort
.type dosort, @function
dosort:
pushq %rbp
movq %rsp, %rbp
subq $16, %rsp
movq %rbx, -16(%rbp)
.L35:
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
pushq %rax
call init
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
movq $0, %rdi
movq %rbp, %rbx
addq $16, %rbx
movq (%rbx), %rbx
addq $-8, %rbx
movq (%rbx), %rsi
subq $1, %rsi
pushq %rax
call quicksort
movq %rbp, %rax
addq $-8, %rax
movq $0, (%rax)
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-8, %rax
movq (%rax), %rbx
subq $1, %rbx
.L29:
movq %rbp, %rax
addq $-8, %rax
movq (%rax), %rax
cmpq %rbx, %rax
jle .L30
jmp .L27
.L27:
leaq .L31, %rdi
pushq %rbp
call print
jmp .L34
.L30:
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-16, %rax
movq (%rax), %rdi
movq %rbp, %rax
addq $-8, %rax
movq (%rax), %rax
imulq $8, %rax
addq %rax, %rdi
movq (%rdi), %rdi
pushq %rbp
call printi
leaq .L28, %rdi
pushq %rbp
call print
movq %rbp, %rax
addq $-8, %rax
movq %rbp, %rdi
addq $-8, %rdi
movq (%rdi), %rdi
addq $1, %rdi
movq %rdi, (%rax)
jmp .L29
.L34:
movq -16(%rbp), %rbx
movq %rbp, %rsp
popq %rbp
ret

.text
.globl quicksort
.type quicksort, @function
quicksort:
pushq %rbp
movq %rsp, %rbp
subq $40, %rsp
movq %rsi, -40(%rbp)
movq %r12, -16(%rbp)
movq %r14, -8(%rbp)
movq %r15, -24(%rbp)
movq %rbx, -32(%rbp)
.L37:
movq %rdi, %rbx
movq -40(%rbp), %r14
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-16, %rax
movq (%rax), %rax
movq %rdi, %r12
imulq $8, %r12
addq %r12, %rax
movq (%rax), %r12
movq -40(%rbp), %rax
cmpq %rax, %rdi
jl .L25
jmp .L26
.L26:
movq $0, %rax
.L24:
jmp .L36
.L25:
.L22:
cmpq %r14, %rbx
jl .L23
jmp .L5
.L5:
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-16, %rax
movq (%rax), %rax
movq %rbx, %r14
imulq $8, %r14
addq %r14, %rax
movq %r12, (%rax)
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
movq %rbx, %rsi
subq $1, %rsi
pushq %rax
call quicksort
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
movq %rbx, %rdi
addq $1, %rdi
pushq %rax
movq -40(%rbp), %rsi
call quicksort
jmp .L24
.L23:
.L12:
cmpq %r14, %rbx
jl .L8
jmp .L11
.L11:
movq $0, %rax
.L7:
movq $0, %r15
cmpq %r15, %rax
jne .L13
jmp .L6
.L6:
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-16, %rax
movq (%rax), %r15
movq %rbx, %rax
imulq $8, %rax
addq %rax, %r15
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-16, %rax
movq (%rax), %rsi
movq %r14, %rax
imulq $8, %rax
addq %rax, %rsi
movq (%rsi), %rax
movq %rax, (%r15)
.L20:
cmpq %r14, %rbx
jl .L16
jmp .L19
.L19:
movq $0, %rax
.L15:
movq $0, %r15
cmpq %r15, %rax
jne .L21
jmp .L14
.L14:
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-16, %rax
movq (%rax), %r15
movq %r14, %rax
imulq $8, %rax
addq %rax, %r15
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-16, %rax
movq (%rax), %rsi
movq %rbx, %rax
imulq $8, %rax
addq %rax, %rsi
movq (%rsi), %rax
movq %rax, (%r15)
jmp .L22
.L8:
movq $1, %rax
movq %rbp, %r15
addq $16, %r15
movq (%r15), %r15
addq $-16, %r15
movq (%r15), %rsi
movq %r14, %r15
imulq $8, %r15
addq %r15, %rsi
movq (%rsi), %r15
cmpq %r15, %r12
jle .L9
jmp .L10
.L10:
movq $0, %rax
.L9:
jmp .L7
.L13:
subq $1, %r14
jmp .L12
.L16:
movq $1, %rax
movq %rbp, %r15
addq $16, %r15
movq (%r15), %r15
addq $-16, %r15
movq (%r15), %rsi
movq %rbx, %r15
imulq $8, %r15
addq %r15, %rsi
movq (%rsi), %r15
cmpq %r15, %r12
jge .L17
jmp .L18
.L18:
movq $0, %rax
.L17:
jmp .L15
.L21:
addq $1, %rbx
jmp .L20
.L36:
movq -16(%rbp), %r12
movq -8(%rbp), %r14
movq -24(%rbp), %r15
movq -32(%rbp), %rbx
movq %rbp, %rsp
popq %rbp
ret

.text
.globl init
.type init, @function
init:
pushq %rbp
movq %rsp, %rbp
subq $32, %rsp
movq %r12, -24(%rbp)
movq %r15, -16(%rbp)
movq %rbx, -32(%rbp)
.L39:
movq %rbp, %rax
addq $-8, %rax
movq $0, (%rax)
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-8, %rax
movq (%rax), %rbx
subq $1, %rbx
.L3:
movq %rbp, %rax
addq $-8, %rax
movq (%rax), %rax
cmpq %rbx, %rax
jle .L4
jmp .L2
.L2:
movq $0, %rax
jmp .L38
.L4:
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-16, %rax
movq (%rax), %r12
movq %rbp, %rax
addq $-8, %rax
movq (%rax), %rax
imulq $8, %rax
addq %rax, %r12
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
addq $-8, %rax
movq (%rax), %r15
movq %rbp, %rax
addq $-8, %rax
movq (%rax), %rax
subq %rax, %r15
movq %r15, (%r12)
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
pushq %rax
call nop
movq %rbp, %r12
addq $-8, %r12
movq %rbp, %rax
addq $-8, %rax
movq (%rax), %rax
addq $1, %rax
movq %rax, (%r12)
jmp .L3
.L38:
movq -24(%rbp), %r12
movq -16(%rbp), %r15
movq -32(%rbp), %rbx
movq %rbp, %rsp
popq %rbp
ret

.text
.globl nop
.type nop, @function
nop:
pushq %rbp
movq %rsp, %rbp
subq $0, %rsp
.L41:
leaq .L1, %rdi
pushq %rbp
call print
jmp .L40
.L40:
movq %rbp, %rsp
popq %rbp
ret

