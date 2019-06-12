.global a,b
.extern c

.data
	.word 10
a:	.word b-a
	.byte c+a
	.skip 9
b: 
	.word l+k+a

.section .tst

	mov r1, 4
l:	add pc, -5
	xchg sp, 0x1234
	mov [sp], 2
k:	jmp r2	
.end
