	.file	"../lib/test_files/files/test.c"
	.text
	.type	func
	.type	func
, @function
func:
.LFB0:
	pushl %ebp
	movl %esp, %ebp
	subl $44, %esp
	movl 12(%ebp), 
	movl %ecx, %ebx
	addl $4, %ebx
	movl 20(%ebp), 
	movl $4, %eax
	addl %, %eax
	movl %eax, 36(%ebp)
	movl 24(%ebp), 
	movl %edx, %eax
	addl %edx, %eax
	movl %eax, 44(%ebp)
	movl 40(%ebp), %ebx
	addl %-1, %ebx
	movl %ecx, %ecx
	addl %ecx, %ecx
	movl 16(%ebp), 
	movl 32(%ebp), %ecx
	addl %-1, %ecx
	movl 20(%ebp), 
	movl %edx, %edx
	addl %edx, %edx
	movl 28(%ebp), 
	movl 4(%ebp), 
	movl %edx, %eax
	leave
	ret

