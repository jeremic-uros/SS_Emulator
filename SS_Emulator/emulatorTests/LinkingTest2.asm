.global b,k
.extern a,c

.data
b:	.word 60
	.skip 8
.equ k, 41
.text 
	mov *0xff00, 84
	mov *0xff00, 50
	mov *0xff00, 58
	mov *0xff00, k
	
	mov *0xff00, 10
.end