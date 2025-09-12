.code16
.global _start
_start:
    cli

    # Reset registers
    xor %ax, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss

    mov $0x7c00, %sp

    sti
    
    // mov $startup_message, %si



    jmp proceed

proceed:
    call loader_start

print:
    lodsb
    cmp $0, %al
    je done
    mov $0x0e, %ah
    int $0x10
    jmp print

done:
    jmp proceed

startup_message:
    .byte 0xa
    .ascii "John Sigma Boot"
    .byte 0x0

. = . + (510 - (.-_start))  # Pad the bootsector to 512
.word 0xaa55
