.global prog
.extern a,main
.data
b: .skip 15
c: 	.byte 85 
	.byte 110 
	.byte 101
	.byte 115
	.byte 105
	.byte 116
	.byte 101
	.byte 32
	.byte 105
	.byte 109
	.byte 101
	.byte 58
	.byte 32
c1:

.text
prog: 
	mov r0, &c
	movb *0xff00, [r0]
	add r0,1
ispis1:	movb r3, *0xff01
	cmp r3,0
	jeq $ispis1
	movb *0xff00, [r0]
	add r0,1
	cmp r0, &c1
	jne $ispis1

	mov r0, &b
	mov r3, 0
	mov r2, 0
	mov r4, 4
loopR:	movb r3, *0xff03
	cmp r3, 0
	jeq $loopR	
	movb r1, *0xff02
	cmp r1, 10
	jeq $skip
	movb [r0], r1
	add r0, 1
	jmp $loopR

skip:	mov r2, r0
	mov r0, &a
loopW:  movb r3, *0xff01
	cmp r3, 0
	jeq $loopW
	movb *0xff00, [r0]
	add r0, 1
	cmp r0, r2
	jeq $end
	jmp $loopW

end: 
	mov r4, 3000
wait:	sub r4, 1
	cmp r4, 0
	jne wait
	ret
.end
