.section .rodata
.L11:
.int 1
.string "\n"
.section .rodata
.L10:
.int 4
.string "\t->\t"
.section .rodata
.L9:
.int 1
.string "\n"
.section .rodata
.L8:
.int 4
.string "\t->\t"
.section .rodata
.L7:
.int 1
.string "\n"
.section .rodata
.L6:
.int 4
.string "\t->\t"
.section .rodata
.L5:
.int 1
.string "\n"
.section .rodata
.L4:
.int 4
.string "\t->\t"
.text
.globl tigermain
.type tigermain, @function
tigermain:
pushq %rbp
movq %rsp, %rbp
subq $0, %rsp
.L13:
pushq %rbp
call try
jmp .L12
.L12:
movq %rbp, %rsp
popq %rbp
ret

.text
.globl try
.type try, @function
try:
pushq %rbp
movq %rsp, %rbp
subq $0, %rsp
.L15:
movq $100, %rdi
pushq %rbp
call printi
leaq .L4, %rdi
pushq %rbp
call print
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
movq $100, %rdi
pushq %rax
call dec2bin
leaq .L5, %rdi
pushq %rbp
call print
movq $200, %rdi
pushq %rbp
call printi
leaq .L6, %rdi
pushq %rbp
call print
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
movq $200, %rdi
pushq %rax
call dec2bin
leaq .L7, %rdi
pushq %rbp
call print
movq $789, %rdi
pushq %rbp
call printi
leaq .L8, %rdi
pushq %rbp
call print
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
movq $789, %rdi
pushq %rax
call dec2bin
leaq .L9, %rdi
pushq %rbp
call print
movq $567, %rdi
pushq %rbp
call printi
leaq .L10, %rdi
pushq %rbp
call print
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rax
movq $567, %rdi
pushq %rax
call dec2bin
leaq .L11, %rdi
pushq %rbp
call print
jmp .L14
.L14:
movq %rbp, %rsp
popq %rbp
ret

.text
.globl dec2bin
.type dec2bin, @function
dec2bin:
pushq %rbp
movq %rsp, %rbp
subq $16, %rsp
movq %rdi, -16(%rbp)
movq %rbx, -8(%rbp)
.L17:
movq $0, %rbx
movq -16(%rbp), %rax
cmpq %rbx, %rax
jg .L2
jmp .L3
.L3:
movq $0, %rax
.L1:
jmp .L16
.L2:
movq %rbp, %rax
addq $16, %rax
movq (%rax), %rbx
movq $2, %rdi
movq -16(%rbp), %rax
cqto
idivq %rdi
pushq %rbx
movq %rax, %rdi
call dec2bin
movq $2, %rbx
movq -16(%rbp), %rax
cqto
idivq %rbx
imulq $2, %rax
movq -16(%rbp), %rdi
subq %rax, %rdi
pushq %rbp
call printi
jmp .L1
.L16:
movq -8(%rbp), %rbx
movq %rbp, %rsp
popq %rbp
ret

