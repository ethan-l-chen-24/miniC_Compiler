	.file	"../lib/test_files/files/p1.c"
	.text
	.type	fun
	.type	fun, @function
fun:
.LFB0:
	pushl %ebp
	movl %esp, %ebp
	subl $16, %esp
	movl 12(%ebp), 
	movl 8(%ebp), 
	jl 
	jmp .L2

.L1:
	jmp .L4

.L2:
	movl 16(%ebp), 
	movl 8(%ebp), 
	jl 
	jmp .L8

.L3:
	movl 0, %eax
	leave
	ret

.L4:
	movl 16(%ebp), 
	movl 8(%ebp), 
	jl 
	jmp .L6

.L5:
	movl 16(%ebp), 
	movl %edx, %edx
	addl $20, %edx
	jmp .L4

.L6:
	movl 16(%ebp), 
	movl $10, %edx
	addl %, %edx
	jmp .L3

.L7:
	movl 12(%ebp), 
	jmp .L9

.L8:
	movl 16(%ebp), 
	jmp .L9

.L9:
	jmp .L3

