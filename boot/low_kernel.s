bits 16
org 0x0

%define endl 0xd, 0xa

jmp _start

gdt:
;gdt_null: 
    dd 0
    dd 0
;gdt_code:
    dw 0xFFFF
    dw 0
    db 0
    db 0b10011010
    db 0b11001111
    db 0
;gdt_data:
    dw 0xFFFF
    dw 0
    db 0
    db 0b10010010
    db 0b11001111
    db 0
gdt_end:
gdt_desc:
    dw gdt_end - gdt - 1
    dd gdt

_start:
    sti

    mov si, msg_welcome

    call puts

    cli
;    xor ax, ax
;    mov ds, ax
    pusha
    lgdt [gdt_desc]
    popa

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x8:clear_pipe

.hang:
    cli
    hlt
    jmp .hang


puts:
    push si
    push ax
    push bx
.loop:
    lodsb
    or al, al
    jz .done

    mov ah, 0xe
    mov bh, 0
    int 0x10
    
    jmp .loop
.done:
    pop bx
    pop ax
    pop si
    ret

msg_welcome: db "Welcome to NBOS.", endl, 0


bits 32
clear_pipe:
    mov ax, 0x8
    mov ds, ax
    mov ss, ax
    mov es, ax

    mov esp, 0x90000
    mov byte [0xb8000], 'P'
    mov word [0xb8001], 0x1b

.hang:
    cli
    hlt
    jmp .hang
