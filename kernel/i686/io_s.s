[bits 32]

global i686_inb
global i686_outb

global i686_sti
global i686_cli

i686_inb:
    mov dx, [esp + 4]
    xor eax, eax
    in al, dx
    ret

i686_outb:
    mov dx, [esp + 4]
    mov al, [esp + 8]
    out dx, al
    ret

i686_sti:
    sti
    ret

i686_cli:
    cli
    ret
