	.file	"../lib/test_files/assembly/p1.s"
	.text
	.globl	func
	.type	func, @function
func:
.LFB0:
	pushl %ebp
	movl %esp, %ebp
	subl $4, %esp
	pushl %ebx
	movl $1, -4(%ebp)
	movl $1, %eax
	popl %ebx
	leave
	ret

