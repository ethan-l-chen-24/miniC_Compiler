	.file	"../lib/test_files/files/test.c"
	.text
	.globl	func
	.type	func, @function
func:
.LFB0:
	pushl %ebp
	movl %esp, %ebp
	subl $24, %esp
	movl 8(%ebp), %edx
	movl %edx, %edx
	imull %edx, %edx
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %edx
	call print
	addl $4, %esp
	popl %edx
	popl %ecx
	popl %ebx
	jmp .L1

.L1:
	jmp .L2

.L2:
	pushl %ebx
	pushl %ecx
	pushl %edx
	call read
	popl %edx
	popl %ecx
	popl %ebx
	movl %edx, -12(%ebp)
	movl -12(%ebp), %edx
	movl %edx, %ecx
	cmpl $0, %ecx
	je .L5
	jmp .L4

.L3:
	movl $0, -4(%ebp)
	jmp .L27

.L4:
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl $-1
	call print
	addl $4, %esp
	popl %edx
	popl %ecx
	popl %ebx
	pushl %ebx
	pushl %ecx
	pushl %edx
	call read
	popl %edx
	popl %ecx
	popl %ebx
	movl %edx, -16(%ebp)
	pushl %ebx
	pushl %ecx
	pushl %edx
	call read
	popl %edx
	popl %ecx
	popl %ebx
	movl %edx, -20(%ebp)
	movl -16(%ebp), %edx
	movl -20(%ebp), %ecx
	movl %edx, %edx
	addl %ecx, %edx
	movl %edx, -24(%ebp)
	movl -24(%ebp), %edx
	movl 8(%ebp), %ecx
	movl %edx, %ebx
	cmpl %ecx, %ebx
	jg .L8
	jmp .L7

.L5:
	movl -12(%ebp), %edx
	movl %edx, %ecx
	cmpl $1, %ecx
	je .L14
	jmp .L13

.L6:
	jmp .L1

.L7:
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl $1
	call print
	addl $4, %esp
	popl %edx
	popl %ecx
	popl %ebx
	jmp .L8

.L8:
	movl -24(%ebp), %edx
	movl 8(%ebp), %ecx
	movl %edx, %ebx
	cmpl %ecx, %ebx
	jg .L10
	jmp .L9

.L9:
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl $11
	call print
	addl $4, %esp
	popl %edx
	popl %ecx
	popl %ebx
	jmp .L10

.L10:
	movl -24(%ebp), %edx
	movl 8(%ebp), %ecx
	movl %edx, %ebx
	cmpl %ecx, %ebx
	jl .L12
	jmp .L11

.L11:
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl $-11
	call print
	addl $4, %esp
	popl %edx
	popl %ecx
	popl %ebx
	jmp .L12

.L12:
	jmp .L6

.L13:
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl $-2
	call print
	addl $4, %esp
	popl %edx
	popl %ecx
	popl %ebx
	pushl %ebx
	pushl %ecx
	pushl %edx
	call read
	popl %edx
	popl %ecx
	popl %ebx
	movl %edx, -16(%ebp)
	movl -16(%ebp), %edx
	movl 8(%ebp), %ecx
	movl %edx, %ebx
	cmpl %ecx, %ebx
	jle .L17
	jmp .L16

.L14:
	movl -12(%ebp), %edx
	movl $0, %ecx
	cmpl %edx, %ecx
	jge .L20
	jmp .L19

.L15:
	jmp .L6

.L16:
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl $1
	call print
	addl $4, %esp
	popl %edx
	popl %ecx
	popl %ebx
	jmp .L18

.L17:
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl $0
	call print
	addl $4, %esp
	popl %edx
	popl %ecx
	popl %ebx
	jmp .L18

.L18:
	jmp .L15

.L19:
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl $-3
	call print
	addl $4, %esp
	popl %edx
	popl %ecx
	popl %ebx
	pushl %ebx
	pushl %ecx
	pushl %edx
	call read
	popl %edx
	popl %ecx
	popl %ebx
	movl %edx, -16(%ebp)
	movl $0, -8(%ebp)
	jmp .L21

.L20:
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl $100000001
	call print
	addl $4, %esp
	popl %edx
	popl %ecx
	popl %ebx
	movl $0, -8(%ebp)
	jmp .L24

.L21:
	movl -8(%ebp), %edx
	movl -16(%ebp), %ecx
	movl %edx, %ebx
	cmpl %ecx, %ebx
	jl .L23
	jmp .L22

.L22:
	movl -8(%ebp), %edx
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %edx
	call print
	addl $4, %esp
	popl %edx
	popl %ecx
	popl %ebx
	movl %edx, %edx
	subl $1, %edx
	movl %edx, -8(%ebp)
	jmp .L21

.L23:
	jmp .L15

.L24:
	movl -8(%ebp), %edx
	movl %edx, %ecx
	cmpl $3, %ecx
	jg .L26
	jmp .L25

.L25:
	movl -8(%ebp), %edx
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %edx
	call print
	addl $4, %esp
	popl %edx
	popl %ecx
	popl %ebx
	movl $1, %edx
	addl %edx, %edx
	movl %edx, -8(%ebp)
	jmp .L24

.L26:
	movl 8(%ebp), %edx
	movl %edx, %edx
	imull $-1, %edx
	movl %edx, -4(%ebp)
	jmp .L27

.L27:
	movl -4(%ebp), %edx
	movl %edx, %eax
	leave
	ret

