.code16

top:

## fat12 headers ##

jmp _start
nop

# BPB (BIOS Parameter Block)
bpb_oem: .ascii "MSWIN4.1"  # oem identifier
bpb_bps: .word 512          # bytes per sector
bpb_spc: .byte 1            # sectors per cluster
bpb_rss: .word 1            # reserved sectors
bpb_ftc: .byte 2            # file allocation table count
bpb_drc: .word 0xe0         # directory entry count
bpb_tsc: .word 2880         # total sector count
bpb_mdt: .byte 0xf0         # media descriptor type
bpb_spf: .word 9            # sectors per fat (file allocation table)
bpb_spt: .word 18           # sectors per track
bpb_thc: .word 2            # total head count
bpb_hsc: .word 0            # hidden sector count
bpb_lsc: .word 0            # large sector count

# EBR (Extended Boot Record)
drn: .byte 0x00             # drive number
wnt: .byte 0                # windows nt flags
sig: .byte 0x29             # signature
vsn: .ascii "    "          # volumeid serial number
vls: .ascii "NBOS       "   # volume label string
sid: .ascii "FAT12   "      # system identifier string

## end headers ##

.global _start
_start:
    cli
    # Reset registers
    xor %ax, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    # Set stack pointer to entry point
    mov $0x7c00, %sp
    sti

    mov %dl, [drn]
    mov $1, %ax
    mov $1, %cl
    mov $0x7e00, %bx
    call disk_read


/* Converts Logical Block Address to Cylinder Head Sector Address
 *
 * ax: LBA in
 *
 * OUT
 * cx [bits 0-5]    : sector number
 * cx [bits 6-15]   : cylinder
 * dh               : head
*/
lba_to_chs:

    push %ax
    push %dx

    xor %dx, %dx
    div [bpb_spt]

    inc %dx
    mov %dx, %cx

    xor %dx, %dx
    div [bpb_thc]

    mov %dl, %dh
    mov %al, %ch
    shl $6, %ah
    or %cl, %ah

    pop %ax
    mov %al, %dl
    pop %ax
    ret


/* Read sectors from disk
 *
 * ax: LBA in
 *
 * OUT
 * cl       : number of sectors [<=128]
 * dl       : drive number
 * es:bx    : memory address where to store data
*/
disk_read:

    push %ax
    push %bx
    push %cx
    push %dx
    push %di

    push %cx
    call lba_to_chs
    pop %ax

    mov $0x2, %ah

    mov $3, %di

.retry:
    pusha
    stc
    int $0x13
    jnc .done

    popa
    call disk_reset

    dec %di
    cmp $0, %di
    je .retry

.fail:
    mov $read_error, %si
    jmp print

.done:
    popa

    pop %di
    pop %dx
    pop %cx
    pop %bx
    pop %ax

    mov $read_success, %si
    jmp print

disk_reset:
    pusha
    mov 0, %ah
    stc
    int $0x13
    jc .fail
    popa
    ret

print:
    lodsb
    cmp $0, %al
    je hang
    mov $0x0e, %ah
    int $0x10
    jmp print

hang:
	cli
	hlt
	jmp hang

read_success:
	.string "Success reading disk! Halting."
	.byte 0

read_error:
    .string "Error reading disk, halting cpu."
    .byte 0

. = . + (510 - (.-top))  # Pad the bootsector to 512

bps: .word 0xaa55       # bootable partition signature
