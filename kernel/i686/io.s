global i686_inb
global i686_outb

global i686_sti
global i686_cli
global i686_hang_s

i686_inb:
    [bits 32]
    mov dx, [esp + 4]
    xor eax, eax
    in al, dx
    ret

i686_outb:
    [bits 32]
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

i686_hang_s:
    call i686_cli
    hlt
