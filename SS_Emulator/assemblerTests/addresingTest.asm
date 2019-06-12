.data
	.skip 10
a:
.text
	movb r1, 2
	mov r1, &a
	mov [sp], 2
	mov r1[0xf], 2
	mov r1[a], 2
	mov r1[0xfff], 2
	mov r1, r1h
	mov r1, $a
	mov r1, *0xA

.end