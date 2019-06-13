.global main
.data
b:	.word 100

.text
main:
	mov r1, b
	mov a, r1
	mov *0xff00, a
	halt
.section .gogo
	.align 20
.bss
a:	.skip 100


.end