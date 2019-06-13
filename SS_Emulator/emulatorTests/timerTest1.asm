.global main
.extern timerintr
.text
main:
	mov *0xff10, 3
	mov *4, &timerintr
	
loop: mov r1, 0
	  jmp $loop
.end


