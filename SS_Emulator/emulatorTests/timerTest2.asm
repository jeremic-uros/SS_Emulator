.global timerintr
.data
a:	.byte 6
.text
timerintr:
	mov r0, a
	sub r0, 1
	cmp r0, 0
	jeq $end
	mov a, r0
	add r0, 48
	mov *0xff00, r0
	iret
end: 
	halt
.end
