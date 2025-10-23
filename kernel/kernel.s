[section .text]

[bits 32]

global _start
extern kernel_main

_start:
    mov esp, 0x200000
    mov ebp, esp

    push ebx
    call kernel_main
    
;
; functions
;

hang:
    cli
    hlt
    jmp hang
