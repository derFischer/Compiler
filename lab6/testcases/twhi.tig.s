.text
.globl tigermain
.type tigermain, @function
tigermain:
pushq %rbp
movq %rsp, %rbp
subq $8, %rsp
movq %rbx, -8(%rbp)
.L8:
movq $10, %rbx
.L5:
movq $0, %rax
cmpq %rax, %rbx
jge .L6
jmp .L1
.L1:
movq $0, %rax
jmp .L7
.L6:
pushq %rbp
movq %rbx, %rdi
call printi
subq $1, %rbx
movq $2, %rax
cmpq %rax, %rbx
je .L3
jmp .L4
.L4:
movq $0, %rax
.L2:
jmp .L5
.L3:
jmp .L1
.L9:
movq $0, %rax
jmp .L2
.L7:
movq -8(%rbp), %rbx
movq %rbp, %rsp
popq %rbp
ret

