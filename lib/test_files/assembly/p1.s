	.file	"../lib/test_files/files/p1.c"
	.text
	.globl	fun
	.type	fun, @function
fun:
.LFB0:
	pushl %ebp
	movl %esp, %ebp
	subl $12, %esp
	movl $2, -8(%ebp)
	movl $3, -12(%ebp)
	jmp .L1

.L1:
	movl -8(%ebp), %edx
	movl %edx, %ecx
	cmpl $10, %ecx
	jg .L3
	jmp .L2

.L2:
	movl -8(%ebp), %edx
	movl %edx, %edx
	addl $3, %edx
	movl %edx, -8(%ebp)
	jmp .L1

.L3:
	movl -8(%ebp), %edx
	movl %edx, -4(%ebp)
	movl -4(%ebp), %edx
	movl %edx, %eax
	leave
	ret

