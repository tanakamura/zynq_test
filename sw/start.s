	.global _start
	.section .text_ocm, "ax"
	.type _start,%function
_start:
	movw	sp, #:lower16:_stack
	movt	sp, #:upper16:_stack

	bl	main

quit:
	b	quit
