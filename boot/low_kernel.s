.code16

.global _start
_start:
    sti

	movw $welcome_message, %si
	call puts
	
	cli
    hlt

// .size kernel, . - kernel

puts:
    pushw %si
    pushw %ax
    pushw %bx
.puts_loop:
    lodsb
    orb %al, %al
    jz .puts_done
    movb $0x0E, %ah
    movb $0x00, %bh
    int $0x10
    jmp .puts_loop
.puts_done:
    popw %bx
    popw %ax
    popw %si
    ret

welcome_message:    .ascii "Welcome to NBOS Kernel!\15\12\0"