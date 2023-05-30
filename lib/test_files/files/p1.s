	.file	"p1.c"
	.text
	.globl	fun
	.type	fun, @function
fun:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -20(%rbp)
	movl	-4(%rbp), %eax
	cmpl	-20(%rbp), %eax
	jge	.L2
	jmp	.L3
.L4:
	addl	$20, -8(%rbp)
.L3:
	movl	-8(%rbp), %eax
	cmpl	-20(%rbp), %eax
	jl	.L4
	movl	-8(%rbp), %eax
	addl	$10, %eax
	movl	%eax, -4(%rbp)
	jmp	.L5
.L2:
	movl	-8(%rbp), %eax
	cmpl	-20(%rbp), %eax
	jge	.L6
	movl	-4(%rbp), %eax
	movl	%eax, -8(%rbp)
	jmp	.L5
.L6:
	movl	-8(%rbp), %eax
	movl	%eax, -4(%rbp)
.L5:
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	fun, .-fun
	.ident	"GCC: (Ubuntu 11.3.0-1ubuntu1~22.04.1) 11.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
