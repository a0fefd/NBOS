.section .text
.align 16
.global loader_start
loader_start:

	# read kernel from disk to load into memory
	
	int $0x13

hang:	
	cli
	hlt
	jmp hang
