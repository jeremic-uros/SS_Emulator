.global c
.extern a,b,k

.data
c:	.word 100+c-b
	.skip 8
.text 
	mov *0xff00, 84
p:	mov *0xff00, 50
	mov *0xff00, 58
	mov *0xff00, $p
	mov *0xff00, 32
	mov *0xff00, k
	halt
.end