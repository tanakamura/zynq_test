	.global exc_vector
	.text
exc_vector:
	b _fatal // reset
	b _fatal // undefiened instruction
	b _fatal // svc
	b _fatal // prefetch abort
	b _data_abort 
	b _fatal // hyp trap
	b _irq_handler
	b _fiq_handler


_fatal:
	push	{r0-r12}
	bl	fatal
	pop	{r0-r12}

	movs	pc, lr

_data_abort:
	push	{r0-r14}
	mov	r0, sp		// arg
	bl	data_abort
	pop	{r0-r14}

	movs	pc, lr

_irq_handler:
	push	{r0-r12}
	bl	irq_handler
	pop	{r0-r12}

	movs	pc, lr

_fiq_handler:
	push	{r0-r12}
	bl	fiq_handler
	pop	{r0-r12}

	movs	pc, lr
