#Currently get_cpuid(int *eax, int *ebx, int *ecx, int *edx).
#Modify to get_cpuid(int cpuid_op, int *eax, int *ebx, int *ecx, int *edx).	
	.text
	.globl get_cpuid
get_cpuid:
	pushq   %rcx
	pushq   %rdx
	xorl 	%eax, %eax	#setup cpuid opcode to 0
	movl    %edi, %eax	#moving opcode into correct place
	cpuid
	#largest param in %eax
	#12-char manufacturer string in ebx, edx, ecx.
	movl	%eax, (%rsi)	#store eax cpuid result
	popq    %rax
	movl	%ebx, (%rax)	#store ebx cpuid result
	popq	%rax		#pop address for edxP
	movl    %ecx, (%rax)    #store ecx cpuid result previous pointer ecx
	movl    %edx, (%r8)     #Store edx result in r8
	ret
	
