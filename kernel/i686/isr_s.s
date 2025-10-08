[bits 32]

extern i686_isr_handler

%macro ISR_NOERR 1

global i686_isr%1:

i686_isr%1:
    push 0 ; dummy err
    push %1
    jmp isr_common

%endmacro

%macro ISR_ERR 1
global i686_isr%1

i686_isr%1:
    push %1
    jmp isr_common

%endmacro

%include "kernel/i686/isrs.inc"

isr_common:
    pusha
    
    xor eax, eax
    mov ax, ds
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call i686_isr_handler
    add esp, 4

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa
    add esp, 8
    
    iret