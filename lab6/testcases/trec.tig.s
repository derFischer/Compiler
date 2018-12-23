.text
.globl tigermain
.type tigermain, @function
tigermain:
pushq %rbp
movq %rsp, %rbp
subq $8, %rsp
movq %rbx, -8(%rbp)
.L2:
movq $0, %rax
movq $2, %rdi
imulq $8, %rdi
pushq %rax
call malloc
movq $8, %rdi
imulq $0, %rdi
movq %rax, %rbx
addq %rdi, %rbx
movq $3, (%rbx)
movq $8, %rdi
imulq $1, %rdi
movq %rax, %rbx
addq %rdi, %rbx
movq $4, (%rbx)
movq %rax, %rbx
movq %rbx, %rax
addq $0, %rax
movq (%rax), %rdi
pushq %rbp
call printi
addq $8, %rbx
movq (%rbx), %rdi
pushq %rbp
call printi
jmp .L1
.L1:
movq -8(%rbp), %rbx
movq %rbp, %rsp
popq %rbp
ret

