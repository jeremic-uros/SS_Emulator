.global a,main
.extern prog
.data
a:	.byte 90
 	.byte 100
	.byte 114
	.byte 97
	.byte 118
	.byte 111
	.byte 32
.text
main:
	mov *0xff10, 7
	call $prog
	halt
.end


