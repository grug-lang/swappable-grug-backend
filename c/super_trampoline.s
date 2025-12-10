	.section .note.GNU-stack,"",@progbits
	.text
	.globl	super_trampoline_smallreturn
	.type	super_trampoline_smallreturn, @function
super_trampoline_smallreturn:
	.cfi_startproc
	endbr64
	// Function overview:
	// - save the args
	// - if the given fn data is null, do nothing
	// - try to get the ast. If that is null, do nothing and return
	
	// - allocate space for x args
	// - for every tag in the fn data:
	//     - pull out the actual value from the stack (depending on if it's a register argument, float register argument, or 'traditional' stack argument)
	//     - push the value to the union args
	//     - copy the tag from the fn data
	// Note that I am rather inexperienced with assembly, so things may be a bit... badly done.

	// If the ptr we were given is null, do nothing and just return 0 - explicitly 0 to make it consistent (even if consistently broken) in case anyone is reading the value
	testq %rdi,%rdi
	jne .supertramp_keepgoing1
	movq $0, %rax
	movq $0, %rdx
	ret
	.supertramp_keepgoing1:

	/* Save args (strategically in reverse order) */
	pushq %r9
	pushq %r8
	pushq %rcx
	pushq %rdx
	pushq %rsi
	pushq %rdi
	/* this demo doesn't do floats actually, but if it did we would preserve the xmm0 - xmm7 registers in case they are used */

	// rdi has the data pointer
	// Pull out the name and entity name and see if we can get the ast
	movq 8(%rdi), %rsi
	movq (%rdi), %rdi
	call grug_get_ast_for
	testq %rax, %rax
	jne .supertramp_keepgoing2
	// No ast! Do nothing and return 0
	// (don't forget to reset the stack to how the caller had it )
	addq $48, %rsp
	movq $0, %rax
	movq $0, %rdx
	ret

	.supertramp_keepgoing2:
	// rax -> ast pointer, we'll keep that around until we need it
	popq %r10
	// r10 -> data pointer - remember we pushed rdi at the top of the stack which had the data ptr at the time
	// Also moved the stack so it points to the first "real" argument that we care about

	movq %rsp, %r11
	movq $0, %rdi
	// r11 -> pointer to the arguments to this function - we'll shift it up the stack (which is forward in address space, conveniently) as we read out the values
	// rdi -> counter for how many args we've read

	// allocate space for 32 grug tagged union arguments
	subq $512, %rsp
	
	// Loop over every argument until we've read the right number of args
	.supertramp_argloop:
	cmpq 16(%r10), %rdi
	je .supertramp_endloop

	// wait wait wait hold up we have to skip past that PESKY return address the caller gave us
	// We got 6 register args, minus one since the first one was used for something else, so "skip" iteration 5
	// We can just advance the read pointer past that STINKY return address as if nothing happened
	cmpq $5, %rdi
	jne .supertramp_keepgoing3
	addq $8, %r11
	.supertramp_keepgoing3:

	// grab the tag - each tag is one byte so yeh
	movsxb 24(%r10, %rdi), %rcx
	// rcx -> value for the tag of this argument

	// If the demo ever decides it needs to worry about floats, we would need to selectively figure out whether to pull from xmm0 - xmm7 v.s the register stack v.s the arguments stack
	// Because there are no floats to worry about, and due to some niceness in systemv abi, we can just yoink 64 bits off the stack and assume the arguments are popped in order automatically
	// (well, except for the WONKY return address that we unceremoniously skip over on the loops 6th iteration)
	// the systemv abi passes args in 64 bit slots regardless if the arg is small enough to fit

	movq (%r11), %rdx
	addq $8, %r11
	// rdx -> value from the arg stack
	// r11 -> updated to read the next argument

	movq %rdi, %rsi
	salq $4, %rsi
	// rsi -> offset in bytes (index * 8)

	// Again, we can just assume we have 64 bits of argument and can write that out to the grug args list
	// grug doesn't do args that are mote than 64 bits so the tag can just be copied directly
	movq %rcx, (%rsp, %rsi)
	movq %rdx, 8(%rsp, %rsi)

	addq $1, %rdi
	jmp .supertramp_argloop

	.supertramp_endloop:
	// Call the execute function
	// rdi contains the number of args read, passed to third argument rdx
	movq %rdi, %rdx
	// rax contains the ast pointer, passed to first argument rdi
	movq %rax, %rdi
	// rsp contains the pointer to the grug args (which is also the stack pointer itself lol), passed to second argument rsi
	movq %rsp, %rsi
	call grug_call_backend

	// our theoretical function has been executed so it's time to clean up and return
	// 512 bytes (grug args) + 6*8 bytes (saved stack args) - 8 bytes (that one register we popped out earlier)
	addq $552, %rsp
	ret
	.cfi_endproc

