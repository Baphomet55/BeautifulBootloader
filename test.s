.global _start

.bss 
.fill 0x38, 1, 0

.data
dummy:
.long 0
str:
.asciz "S\0a\0t\0a\0n\0\n"


.section .reloc
label1:
.long dummy-label1
.long 10
.word 0

.text

xcap:
	mov 0x10(%eax), %edi
	and $0xffff0000, %edi
	shr $0x10, %edi
	shl $0x2, %edi
	add %edi, %eax
	ret

_start:
	lea -0x7(%rip), %rdi

	mov %rcx, 0x3000(%rdi)
	mov %rdx, 0x3008(%rdi)

	sub $0x8, %rsp

	movq 0x60(%rdx), %rsi
	movq 0x38(%rsi), %rsi

	leaq 0x3010(%rdi), %rcx
	mov $0x0, %rdx
	leaq 0x3018(%rdi), %r8
	leaq 0x3020(%rdi), %r9
	push 0x3028(%rdi)
	call *%rsi

	pop %rax

	movq 0x3008(%rdi), %rdx
	movq 0x60(%rdx), %rsi
	movq 0x40(%rsi), %rsi

	movq $0x2, %rcx
	movq 0x3010(%rdi), %rdx
	mov 0x3020(%rdi), %r9
	leaq (%rdx, %r9, 2), %rdx
	mov %rdx, 0x3010(%rdi)
	leaq 0x3030(%rdi), %r8
	
	call *%rsi

	movq 0x3008(%rdi), %rdx
	movq 0x60(%rdx), %rsi
	movq 0x38(%rsi), %rsi

	leaq 0x3010(%rdi), %rcx
	movq 0x3030(%rdi), %rdx
	leaq 0x3018(%rdi), %r8
	leaq 0x3020(%rdi), %r9
	leaq 0x3028(%rdi), %rax
	pushq 0x3028(%rdi)

	call *%rsi
	pop %rax

	mov 0x3008(%rdi), %rdx
	movq 0x60(%rdx), %rsi
	mov 0xe8(%rsi), %rsi
	
	mov 0x3000(%rdi), %rcx
	mov 0x3018(%rdi), %rdx

	call *%rsi

	movl $0x80006010, %eax

	mov $0xcf8, %edx
	outl %eax, %edx
	mov $0xcfc, %edx
	inl %edx, %eax

	mov (%eax), %edi
	and $0xff, %edi
	add %edi, %eax

	//restart controller
	mov $0x2, (%eax)

	mov $0x10000, 0x18(%eax)
	mov $0x100000, 0x30(%eax)
	
	sub %edi, %eax

	//event ring setup

	mov 0x18(%eax), %edi
	add %edi, %eax
	mov $0x1, 0x28(%eax)

	mov $0x100, %esi
	mov $0x10, 0x8(%esi)

	mov $0x100, 0x30(%eax)
	
	// turn on the controller
	sub %edi, %eax

	mov (%eax), %edi
	and $0xff, %edi
	add %edi, %eax

	mov $0x1, (%eax)

	sub %edi, %eax

	// enable device context
	mov $0x10000, %esi
	mov $0x2400, 0xc(%esi)

	mov %esi, 0x100(%esi)
	mov $0x1822, 0x10c(%esi)

	//input context setup

	mov $0x50000, %edi
	mov $0x3, 0x4(%edi)

	mov $0x8000000, 0x20(%edi)
	mov $0x10000, 0x24(%edi)

	mov $0x400026, 0x44(%edi)
	mov $0x70001, 0x48(%edi)
	
	// output context setup

	mov $0x100000, %edi
	mov $0x80000, 0x8(%edi)

	// set address command trb
	
	mov $0x10000, %edi
	mov $0x50000, 0x10(%edi)
	mov $0x1002c00, 0x1c(%edi)
	
	mov 0x14(%eax), %edi
	add %edi, %eax
	mov $0x0, (%eax)
	
	sub %edi, %eax

	// setup stage
	mov $0x70000, %edi
	mov $0x10900, (%edi)
	mov $0x8, 0x8(%edi)
	mov $0x30841, 0xc(%edi)

	add $0x10, %edi

	// data stage
	/*mov $0x90000, (%edi)
	mov $0x8, 0x8(%edi)
	mov $0x10c01, 0xc(%edi)

	add $0x10, %edi*/

	// status stage
	mov $0x1021, 0xc(%edi)

	mov 0x14(%eax), %edi
	add %edi, %eax
	movl $0x1, 0x4(%eax)

	sub %edi, %eax

	mov $0x10000, %edi
	mov $0x50000, 0x20(%edi)
	mov $0x1003000,0x2c(%edi)

	mov 0x14(%eax), %edi
	add %edi, %eax
	movl $0x0, (%eax)

	// setup stage
	mov $0x70020, %edi
	mov $0x2000680, (%edi)
	mov $0x200000, 0x4(%edi)
	mov $0x8, 0x8(%edi)
	mov $0x30841, 0xc(%edi)

	add $0x10, %edi

	// data stage
	mov $0x90000, (%edi)
	mov $0x20, 0x8(%edi)
	mov $0x10c01, 0xc(%edi)

	add $0x10, %edi

	// status stage
	mov $0x1021, 0xc(%edi)

	mov 0x14(%eax), %edi
	add %edi, %eax
	movl $0x1, 0x4(%eax)
	
	break:
	jmp break

	add $0x8, %rsp

	ret

	
