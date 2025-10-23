[org 0x8000]

%define endl 0xd, 0xa

jmp _start

%include "FAT.inc"
%include "disk_utils.inc"

; GDT (Global Descriptor Table)
gdt_start:
    dq 0x0                  ; Null descriptor (required)

gdt_code:
    dw 0xffff               ; Limit
    dw 0x0                  ; Base low
    db 0x0                  ; Base mid
    db 10011010b            ; Access byte (code segment)
    db 11001111b            ; Granularity and flags
    db 0x0                  ; Base high

gdt_data:
    dw 0xffff               ; Limit
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


; CONSTANTS
MEM_VIDEO equ 0xb8000
SCREEN_SIZE equ 2000

[bits 16]
section .bss
vesa_mode_info:
    resb 256
section .text

_start:

load_kernel:
    ; read something from floppy disk
    ; BIOS should set DL to drive number
    mov [ebr_drive_number], dl

    ; read drive parameters (sectors per track and head count),
    ; instead of relying on data on formatted disk
    push es
    mov ah, 08h
    int 13h
    jc floppy_error
    pop es

    and cl, 0x3F                        ; remove top 2 bits
    xor ch, ch
    mov [bdb_sectors_per_track], cx     ; sector count

    inc dh
    mov [bdb_heads], dh                 ; head count

    ; compute LBA of root directory = reserved + fats * sectors_per_fat
    ; note: this section can be hardcoded
    mov ax, [bdb_sectors_per_fat]
    mov bl, [bdb_fat_count]
    xor bh, bh
    mul bx                              ; ax = (fats * sectors_per_fat)
    add ax, [bdb_reserved_sectors]      ; ax = LBA of root directory
    push ax

    ; compute size of root directory = (32 * number_of_entries) / bytes_per_sector
    mov ax, [bdb_dir_entries_count]
    shl ax, 5                           ; ax *= 32
    xor dx, dx                          ; dx = 0
    div word [bdb_bytes_per_sector]     ; number of sectors we need to read

    test dx, dx                         ; if dx != 0, add 1
    jz .root_dir_after
    inc ax                              ; division remainder != 0, add 1
                                        ; this means we have a sector only partially filled with entries
.root_dir_after:

    ; read root directory
    mov cl, al                          ; cl = number of sectors to read = size of root directory
    pop ax                              ; ax = LBA of root directory
    mov dl, [ebr_drive_number]          ; dl = drive number (we saved it previously)
    mov bx, buffer                      ; es:bx = buffer
    call disk_read

    ; search for kernel.bin
    xor bx, bx
    mov di, buffer

.search_kernel:
    mov si, kernel_file_name
    mov cx, 11                          ; compare up to 11 characters
    push di
    repe cmpsb
    pop di
    je .found_kernel

    add di, 32
    inc bx
    cmp bx, [bdb_dir_entries_count]
    jl .search_kernel

    ; kernel not found
    jmp kernel_not_found_error

.found_kernel:

    ; di should have the address to the entry
    mov ax, [di + 26]                   ; first logical cluster field (offset 26)
    mov [kernel_cluster], ax

    ; load FAT from disk into memory
    mov ax, [bdb_reserved_sectors]
    mov bx, buffer
    mov cl, [bdb_sectors_per_fat]
    mov dl, [ebr_drive_number]
    call disk_read

    ; read kernel and process FAT chain
    mov bx, KERNEL_LOAD_SEGMENT
    mov es, bx
    mov bx, KERNEL_LOAD_OFFSET

.load_kernel_loop:
    
    ; Read next cluster
    mov ax, [kernel_cluster]
    
    ; not nice :( hardcoded value
    add ax, 31                          ; first cluster = (kernel_cluster - 2) * sectors_per_cluster + start_sector
                                        ; start sector = reserved + fats + root directory size = 1 + 18 + 134 = 33
    mov cl, 1
    mov dl, [ebr_drive_number]
    call disk_read

    add bx, [bdb_bytes_per_sector]

    ; compute location of next cluster
    mov ax, [kernel_cluster]
    mov cx, 3
    mul cx
    mov cx, 2
    div cx                              ; ax = index of entry in FAT, dx = cluster mod 2

    mov si, buffer
    add si, ax
    mov ax, [ds:si]                     ; read entry from FAT table at index ax

    or dx, dx
    jz .even

.odd:
    shr ax, 4
    jmp .next_cluster_after

.even:
    and ax, 0x0FFF

.next_cluster_after:
    cmp ax, 0x0FF8                      ; end of chain
    jae .read_finish

    mov [kernel_cluster], ax
    jmp .load_kernel_loop

.read_finish:

    ; ; switch to 320:300 VGA Graphics
    ; mov ah, 0x00
    ; mov al, 0x13
    ; int 0x10 

    mov ax, 0x4F01
    ; mov cx, 0x4118    
    mov cx, 0x4118
    mov di, vesa_mode_info
    push es
    mov ax, 0
    mov es, ax
    int 0x10
    pop es
    cmp ah, 0x1
    je .hang

    mov ax, 0x4F02       ; VESA set mode
    ; mov bx, 0x4118    
    mov bx, 0x4118
    int 0x10
    cmp ax, 0x004f
    jne .hang

    cli

    lgdt [gdt_descriptor]
    mov eax, cr0
    or al, 1
    mov cr0, eax

    jmp CODE_SEG:unreal_start

.hang:
    mov si, msg_hang
    call puts
    cli
    hlt
    jmp .hang

msg_hang: db "Hanging...", endl, 0
kernel_file_name:        db "KERNEL  BIN"
kernel_cluster:         dw 0

KERNEL_LOAD_SEGMENT     equ 0xFFFF
KERNEL_LOAD_OFFSET      equ 0x10

kernel_not_found_error:
    hlt

floppy_error:
    hlt

[bits 32]
clrscr:
    mov edi, MEM_VIDEO
    mov ecx, SCREEN_SIZE
    mov ax, 0xf20
    rep stosw
    ret

unreal_start:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    call clrscr

    mov ebx, vesa_mode_info
    jmp 0x100000 ; jump to kernel

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

times 2048 - ($-$$) db 0 ; pad to 4 sectors


buffer: