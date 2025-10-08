[bits 32]
global i686_gdt_load

i686_gdt_load:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]
    lgdt [eax]

    mov esp, ebp
    pop ebp
    ret
