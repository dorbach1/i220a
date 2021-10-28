	.text
	.globl get_parity
#edi contains n	
get_parity:
	xorl %eax, %eax 	#setting eax to 0 
	testl %edi, %edi	#setting parity flag
	jpe .C2			#IF parity is odd jump to c2 and add 1 to eax		

.C1:
	ret

.C2:
	add $1, %eax
	jmp .C1	
