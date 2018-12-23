.text
.globl tigermain
.type tigermain, @function
tigermain:
pushq %rbp
movq %rsp, %rbp
subq $8, %rsp
movq %r12, -8(%rbp)
.L5:
movq %rbp, %r12
movq $9, %rdi
movq $4, %rsi
pushq %rbp
call g
pushq %r12
movq %rax, %rdi
call printi
jmp .L4
.L4:
movq -8(%rbp), %r12
movq %rbp, %rsp
popq %rbp
ret

.text
.globl g
.type g, @function
g:
pushq %rbp
movq %rsp, %rbp
subq $0, %rsp
.L7:
cmpq %rsi, %rdi
jg .L2
jmp .L3
.L3:
.L1:
movq %rsi, %rax
jmp .L6
.L2:
movq %rdi, %rsi
jmp .L1
.L6:
movq %rbp, %rsp
popq %rbp
ret

