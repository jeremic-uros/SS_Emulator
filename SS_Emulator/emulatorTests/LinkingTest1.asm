.global a,main
.extern b,c

.data
a:	.word 50
	.skip 8
.text
main: 
	mov r1, a
	mov r2, $b
	mov r3, &c
	
	mov *0xff00, 84
	mov *0xff00, 49
	mov *0xff00, 58
	
	mov *0xff00, r1
	mov *0xff00, 32
	mov *0xff00, r2
	mov *0xff00, 32
	mov *0xff00, [r3]
	
	mov *0xff00, 10
.end
	
	