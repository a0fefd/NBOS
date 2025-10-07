[bits 32]
global idt_load

idt_load:
    mov eax, [esp + 4]  ; pointer to IDT descriptor (passed as argument)
    lidt [eax]
    ret
