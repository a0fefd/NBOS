.code16


.align 16
.global _start

# ================================
# --- BIOS Parameter Block (BPB) ---
# ================================

jmp _start
nop

bdb_oem:                    .ascii "MSWIN4.1"
bdb_bytes_per_sector:       .word 512
bdb_sectors_per_cluster:    .byte 1
bdb_reserved_sectors:       .word 1
bdb_fat_count:              .byte 2
bdb_dir_entries_count:      .word 0x0E0
bdb_total_sectors:          .word 2880
bdb_media_descriptor_type:  .byte 0xF0
bdb_sectors_per_fat:        .word 9
bdb_sectors_per_track:      .word 18
bdb_heads:                  .word 2
bdb_hidden_sectors:         .long 0
bdb_large_sector_count:     .long 0

# --- Extended Boot Record (EBR) ---
ebr_drive_number:           .byte 0
ebr_reserved:               .byte 0
ebr_signature:              .byte 0x29
ebr_volume_id:              .byte 0x12,0x34,0x56,0x78
ebr_volume_label:           .ascii "NBOS       "
ebr_volume_label_pad:       .byte 0
ebr_system_id:              .ascii "FAT12   "

_start:
    cli
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es

    movw %ax, %ss
    movw $0x7c00, %sp
    sti

    # save boot drive
    movb %dl, ebr_drive_number

    pushw %es
    pushw $after
    retf

after:
    movw $msg_loading, %si
    call puts

    # lba of root = reserved + fatcount*sectorsperfat
    #             = 0 + 2*9
    #             = 18
    # size of root = 32*numberofentries/bytespersector
    #              = 32*0xe0 / 512
    #              = 0xe0 / 16
    #              = 0xe0 >> 4
    movb bdb_dir_entries_count, %cl
    shrb $4, %cl

    # read root
    // movb %al, %cl
    // popw %ax
    movw $18, %ax
    // movw $ebr_drive_number, %dx
    movw $BUFFER_SEG, %bx
    movw %bx, %es
    xorw %bx, %bx
    call disk_read

    movw $BUFFER_SEG, %di

_sk:# search for kernel
    movw $file_kernel_bin, %si
    movw $11, %cx
    push %di
    repe cmpsb
    pop %di
    // je +8 # found kernel
    je _fk

    addw $32, %di
    incw %bx
    cmpw bdb_dir_entries_count, %bx
    jl _sk

    # kernel not found
    movw $msg_kernel_not_found, %si
    call puts
    jmp hang

_fk:# found kernel
    movw 26(%di,1), %ax
    movw %ax, kernel_cluster

    # load fat from disk into mem
    movw bdb_reserved_sectors, %ax
    movw $BUFFER_SEG, %bx
    movb bdb_sectors_per_fat, %cl
    movb ebr_drive_number, %dl
    call disk_read

    movw KERNEL_LOAD_SEG, %bx
    movw %bx, %es
    xorw %bx, %bx

_lk:# load kernel loop

    # read next cluster
    movw kernel_cluster, %ax
    add $31, %ax

    movb $1, %cl
    movb ebr_drive_number, %dl
    call disk_read

    addw bdb_bytes_per_sector, %bx

    # compute loc of next cluster
    movw $kernel_cluster, %ax
    movw $3, %cx
    mulw %cx
    movw $2, %cx
    divw %cx

    movw $BUFFER_SEG, %si
    addw %ax, %si
    movw %ds:(%si), %ax

    orw %dx, %dx
    jz .even

.odd:
    shr $4, %ax
    jmp .next_cluster

.even:
    andw $0x0FFF, %ax

.next_cluster:
    cmpw $0x0FF8, %ax
    jae .read_finish

    mov %ax, kernel_cluster
    jmp _lk

.read_finish:
    movb ebr_drive_number, %dl

to_kernel:
    movw $KERNEL_LOAD_SEG, %ax
    movw %ax, %es
    xorw %bx, %bx

    ljmp $KERNEL_LOAD_SEG, $KERNEL_LOAD_OFS

hang:
    cli
.hlt:
    hlt
    jmp .hlt


# --------------------------------
# Print zero-terminated string at DS:SI
# --------------------------------
puts:
    pushw %si
    pushw %ax
    pushw %bx
.puts_loop:
    lodsb
    orb %al, %al
    jz .puts_done
    movb $0x0E, %ah
    movb $0x00, %bh
    int $0x10
    jmp .puts_loop
.puts_done:
    popw %bx
    popw %ax
    popw %si
    ret

# --------------------------------
# disk_read
# in: AX = LBA, ES:BX = buffer, CL = sector count, DL = drive
# --------------------------------
disk_read:
    pusha
    pushw %ds

    # convert LBA in AX → CHS
    pushw %ax
    call lba_to_chs         # CH=cyl, DH=head, CL=sector
    popw %ax                # restore LBA in AX (not needed further)

    movb $0x02, %ah         # BIOS read
    movb $1, %al            # read 1 sector (for simplicity)
    int $0x13
    jc disk_error

    popw %ds
    popa
    ret

disk_error:
    movw $msg_read_failed, %si
    call puts
    jmp hang

# --------------------------------
# Convert LBA (AX) to CHS (CH,CL,DH)
# --------------------------------
lba_to_chs:
    xorw %dx, %dx
    divw bdb_sectors_per_track   # AX / SPT
    movw %dx, %cx                # remainder
    incb %cl                     # sector = remainder + 1

    xorw %dx, %dx
    divw bdb_heads               # AX / heads
    movb %dl, %dh                # head = remainder
    movb %al, %ch                # cylinder low 8 bits

    # put high 2 bits of cylinder into CL bits 6..7
    shrw $8, %ax
    andb $0x03, %al
    shlb $6, %al
    orb  %al, %cl

    ret


# ================================
# --- Strings / Variables ---
# ================================

# \15: carrige return   0xD (\r)
# \12: newline          0xA (\n)

msg_loading:                .ascii "Loading...\15\12\0" 
                            // .byte 0
msg_read_failed:            .ascii "Disk read failed!\15\12\0"
msg_kernel_not_found:       .ascii "KERNEL.BIN not found!\15\12\0"
file_kernel_bin:            .ascii "KERNEL  BIN"

kernel_cluster:             .word 0

# ================================
# --- Constants ---
# ================================
    .equ KERNEL_LOAD_SEG, 0x2000
    .equ KERNEL_LOAD_OFS, 0x0000
    .equ BUFFER_SEG, 0x7e0        # temp buffer at 0000:0x7e00 / 0x07e0:0000


# ================================
# --- Boot Signature ---
# ================================
    .org 510
    .word 0xAA55
