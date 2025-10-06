[org 0x8000]

jmp _start


; GDT (Global Descriptor Table)
gdt_start:
    dq 0x0                  ; Null descriptor (required)

gdt_code:
    dw 0xFFFF               ; Limit
    dw 0x0                  ; Base low
    db 0x0                  ; Base mid
    db 10011010b            ; Access byte (code segment)
    db 11001111b            ; Granularity and flags
    db 0x0                  ; Base high

gdt_data:
    dw 0xFFFF               ; Limit
    dw 0x0
    db 0x0
    db 10010010b            ; Access byte (data segment)
    db 11001111b
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; GDT size - 1
    dd gdt_start              ; GDT base address

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

[bits 16]
_start:
    cli

    lgdt [gdt_descriptor]
    mov eax, cr0
    or al, 1
    mov cr0, eax

    jmp CODE_SEG:protected_start

.hang:
    cli
    hlt
    jmp .hang

[bits 32]


protected_start:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ss, ax
    mov esp, 0x8000

    


.hang:
    cli
    hlt
    jmp .hang


times 2048 - ($-$$) db 0 ; pad to 4 sectors
