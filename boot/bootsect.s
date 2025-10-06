[bits 16]
[org 0x7c00]

%define endl 0xd, 0xa

jmp short _start
nop

%include "boot/FAT.inc"

_start:
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    push es
    push word .0
    retf

.0:
    mov [ebr_drive_number], dl

    mov si, msg_load
    call puts

    push es
    mov ah, 08h
    int 0x13
    jc disk_err
    pop es

    and cl, 0x3f
    xor ch, ch
    mov [bdb_sectors_per_track], cx

    inc dh
    mov [bdb_heads], dh

    mov ax, 19
    push ax
    
    mov ax, [bdb_dir_entries_count]
    shl ax, 5
    xor dx, dx
    div word [bdb_bytes_per_sector]

    test dx, dx
    jz .root_dir_after
    inc ax

.root_dir_after:
    mov cl, al
    pop ax
    mov dl, [ebr_drive_number]
    mov bx, buffer
    call disk_read

    xor bx, bx
    mov di, buffer

.find_stage2:
    mov si, stage2_file_name
    mov cx, 11
    push di
    repe cmpsb
    pop di
    je .found_stage2

    add di, 32
    inc bx
    cmp bx, [bdb_dir_entries_count]
    jl .find_stage2

    jmp stage2_nfound_err
.found_stage2:
    mov ax, [di + 26]
    mov [stage2_clust], ax

    mov ax, [bdb_reserved_sectors]
    mov bx, buffer
    mov cl, [bdb_sectors_per_fat]
    mov dl, [ebr_drive_number]
    call disk_read

    mov bx, stage2_LOAD_SEGM
    mov es, bx
    mov bx, stage2_LOAD_OFFS
.load_stage2_loop:
    mov ax, [stage2_clust]
    add ax, 31
    mov cl, 1
    mov dl, [ebr_drive_number]
    call disk_read

    add bx, [bdb_bytes_per_sector]
    mov ax, [stage2_clust]
    mov cx, 3
    mul cx
    mov cx, 2
    div cx

    mov si, buffer
    add si, ax
    mov ax, [ds:si]

    or dx, dx
    jz .even
.odd:
    shr ax, 4
    jmp .next_clust_after
.even:
    and ax, 0xfff
.next_clust_after:
    cmp ax, 0xff8
    jae .read_finished
    mov [stage2_clust], ax
    jmp .load_stage2_loop
.read_finished:

    mov dl, [ebr_drive_number]

    mov ax, stage2_LOAD_SEGM
    mov ds, ax
    mov es, ax

    jmp stage2_LOAD_SEGM:stage2_LOAD_OFFS

    jmp hang

lba_to_chs:
    push ax
    push dx

    xor dx, dx
    div word [bdb_sectors_per_track]

    inc dx
    mov cx, dx

    xor dx, dx
    div word [bdb_heads]

    mov dh, dl
    mov ch, al
    shl ah, 6
    or cl, ah

    pop ax
    mov dl, al
    pop ax
    ret

disk_read:
    pusha
    push cx
    
    call lba_to_chs
    pop ax

    mov ah, 0x2
    mov di, 3
.retry:
    pusha
    stc
    int 0x13
    jnc .done

    popa
    call disk_reset

    dec di
    test di, di
    jnz .retry
.fail:
    jmp disk_err
.done:
    popa
    popa

    ret

disk_reset:
    pusha
    mov ah, 0
    stc
    int 0x13
    jc disk_err
    popa
    ret

disk_err:
    mov si, msg_read_fail
    call puts
    jmp hang

stage2_nfound_err:
    mov si, msg_stage2_nfound
    call puts
    jmp hang

hang:
    cli
    hlt
    jmp $-2

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

msg_load: db "Loading...", endl, 0
msg_read_fail: db "Read failed.", endl, 0
msg_stage2_nfound: db "Stage2 NOT FOUND", endl, 0
stage2_file_name: db "STAGE2  BIN"
stage2_clust: dw 0

stage2_LOAD_SEGM equ 0x0000
stage2_LOAD_OFFS equ 0x8000

times 510 - ($-$$) db 0

dw 0xAA55

buffer: