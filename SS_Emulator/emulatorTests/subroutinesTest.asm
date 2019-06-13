.global main

.text
main:
	mov *0xff10, 7
loopR:
	movb r3, *0xff03
	cmp r3, 0
	jeq $loopR
	movb r1, *0xff02
	sub r1, 48
	mov r0,0
	call $sum
	add r0, 48
	mov *0xff00, r0
	halt
sum:
	cmp r1, 0
	jeq $skip
	push r1
	sub r1,1
	call $sum
	pop r2
	add r0,r2
skip:
	ret
.end