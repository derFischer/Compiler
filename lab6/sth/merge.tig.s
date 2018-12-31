.section .rodata
L54:
.int 1
.string " "
.section .rodata
L53:
.int 1
.string "\n"
.section .rodata
L46:
.int 1
.string "0"
.section .rodata
L45:
.int 1
.string "-"
.section .rodata
L41:
.int 1
.string "0"
.section .rodata
L21:
.int 1
.string "0"
.section .rodata
L11:
.int 1
.string "\n"
.section .rodata
L10:
.int 1
.string " "
.section .rodata
L4:
.int 1
.string "9"
.section .rodata
L3:
.int 1
.string "0"
.text
.globl tigermain
.type tigermain, @function
tigermain:
subq $tigermain_framesize, %rsp
movq %rdi, -8(%rbp)
movq %rbx, -32(%rbp)
movq %rbp, -24(%rbp)
movq %r15, -40(%rbp)
L59:
movq %rbp, %rbx
addq $-16, %rbx
call getchar
movq %rax, (%rbx)
movq %rbp, %rdi
call L24
movq %rax, %rbx
movq %rbp, %r15
addq $-16, %r15
call getchar
movq %rax, (%r15)
movq %rbp, %rdi
call L24
movq %rax, %rsi
movq %rbp, %rdi
movq %rbx, %rdx
call L25
movq %rax, %rsi
movq %rbp, %rdi
call L27
jmp L58
L58:
movq -32(%rbp), %rbx
movq -24(%rbp), %rbp
movq -40(%rbp), %r15
addq $tigermain_framesize, %rsp
ret

.text
.globl L27
.type L27, @function
L27:
subq $L27_framesize, %rsp
movq %rdi, -8(%rbp)
movq %rsi, -24(%rbp)
movq %rbp, -16(%rbp)
L61:
movq $0, %r10
movq -24(%rbp), %rax
cmpq %r10, %rax
je L55
L56:
movq 8(%rbp), %rdi
movq -24(%rbp), %rax
movq 0(%rax), %rsi
call L26
leaq L54, %rdi
call print
movq 8(%rbp), %rdi
movq -24(%rbp), %rax
movq 8(%rax), %rsi
call L27
L57:
jmp L60
L55:
leaq L53, %rdi
call print
jmp L57
L60:
movq -16(%rbp), %rbp
addq $L27_framesize, %rsp
ret

.text
.globl L26
.type L26, @function
L26:
subq $L26_framesize, %rsp
movq %rdi, -8(%rbp)
movq %rsi, -24(%rbp)
movq %rbp, -16(%rbp)
L63:
movq $0, %r10
movq -24(%rbp), %rax
cmpq %r10, %rax
jl L50
L51:
movq $0, %r10
movq -24(%rbp), %rax
cmpq %r10, %rax
jg L47
L48:
leaq L46, %rdi
call print
L49:
L52:
jmp L62
L50:
leaq L45, %rdi
call print
movq %rbp, %rdi
movq -24(%rbp), %rsi
subq $0, %rsi
call L40
jmp L52
L47:
movq %rbp, %rdi
movq -24(%rbp), %rsi
call L40
jmp L49
L62:
movq -16(%rbp), %rbp
addq $L26_framesize, %rsp
ret

.text
.globl L40
.type L40, @function
L40:
subq $L40_framesize, %rsp
movq %rdi, -8(%rbp)
movq %rsi, -24(%rbp)
movq %rbp, -16(%rbp)
movq %r15, -32(%rbp)
L65:
movq $0, %r15
movq -24(%rbp), %rax
cmpq %r15, %rax
jg L42
L43:
movq $0, %rax
L44:
jmp L64
L42:
movq 8(%rbp), %rdi
movq $10, %r9
movq -24(%rbp), %rax
cqto
idivq %rdx
movq %rax, %rsi
call L40
movq $10, %r15
movq -24(%rbp), %rax
cqto
idivq %rdx
imulq $10, %rax
movq -24(%rbp), %r15
subq %rax, %r15
leaq L41, %rdi
call ord
movq %r15, %rdi
addq %rax, %rdi
call chr
movq %rax, %rdi
call print
jmp L44
L64:
movq -16(%rbp), %rbp
movq -32(%rbp), %r15
addq $L40_framesize, %rsp
ret

.text
.globl L25
.type L25, @function
L25:
subq $L25_framesize, %rsp
movq %rdi, -8(%rbp)
movq %rsi, -48(%rbp)
movq %rdx, -40(%rbp)
movq %rbx, -32(%rbp)
movq %rbp, -16(%rbp)
movq %r15, -24(%rbp)
L67:
movq $0, %rbx
movq -48(%rbp), %rax
cmpq %rbx, %rax
je L37
L38:
movq $0, %rbx
movq -40(%rbp), %rax
cmpq %rbx, %rax
je L34
L35:
movq -48(%rbp), %rbx
movq 0(%rbx), %rbx
movq -40(%rbp), %rax
movq 0(%rax), %rax
cmpq %rax, %rbx
jl L31
L32:
movq $16, %rdi
call malloc
movq %rax, %rbx
movq %rbx, %r15
addq $8, %r15
movq 8(%rbp), %rdi
movq -40(%rbp), %rax
movq 8(%rax), %rsi
movq -48(%rbp), %rdx
call L25
movq %rax, (%r15)
movq -40(%rbp), %r15
movq 0(%r15), %r15
movq %r15, 0(%rbx)
L33:
movq %rbx, %rax
L36:
L39:
jmp L66
L37:
movq -40(%rbp), %rax
jmp L39
L34:
movq -48(%rbp), %rax
jmp L36
L31:
movq $16, %rdi
call malloc
movq %rax, %rbx
movq %rbx, %r15
addq $8, %r15
movq 8(%rbp), %rdi
movq -40(%rbp), %rsi
movq -48(%rbp), %rax
movq 8(%rax), %rdx
call L25
movq %rax, (%r15)
movq -48(%rbp), %rax
movq 0(%rax), %rax
movq %rax, 0(%rbx)
jmp L33
L66:
movq -32(%rbp), %rbx
movq -16(%rbp), %rbp
movq -24(%rbp), %r15
addq $L25_framesize, %rsp
ret

.text
.globl L24
.type L24, @function
L24:
subq $L24_framesize, %rsp
movq %rdi, -8(%rbp)
movq %rbx, -40(%rbp)
movq %rbp, -16(%rbp)
movq %r14, -24(%rbp)
movq %r15, -32(%rbp)
L69:
movq $8, %rdi
call malloc
movq %rax, %rbx
movq $0, 0(%rbx)
movq 8(%rbp), %rdi
movq %rbx, %rsi
call L0
movq %rax, %r14
movq 0(%rbx), %rbx
movq $0, %rax
cmpq %rax, %rbx
jne L2