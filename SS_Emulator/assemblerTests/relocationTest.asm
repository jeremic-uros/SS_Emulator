.global a,b
.extern c

.data
	.skip 10
a:	.word a-s 
	.byte k
.equ k, 0xFEFE
	
.text

b:	mov r1, 2
	mov r1, c
	mov r1, $b
s:	mov r1, b
	mov r1, a
	mov r1, $a
	mov r1, $c
	
.end
	
