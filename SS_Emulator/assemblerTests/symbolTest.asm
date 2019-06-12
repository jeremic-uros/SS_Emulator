.global a,b,c,d
.extern f,g
.global h,p
.extern j,s

.data
a: .skip 0xA
b: .word 2
c: .byte 1
.align 4
.skip 100
h:
.equ ABS, 1111

.text
	mov r1,1
	mov r2,2
p:
	xchg r1,[r2]


.bss
d: .skip 256


.end