	.global _start
	.section .text_ocm, "ax"
	.type _start,%function
_start:
	movw	sp, #:lower16:_stack
	movt	sp, #:upper16:_stack

	bl	main

quit:
	b	quit

	.global _secondary_start
	.type _secondary_start,%function


_secondary_start:
	movw	sp, #:lower16:_stack2
	movt	sp, #:upper16:_stack2

	bl	main2

	b	quit
	
