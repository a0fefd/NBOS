#include "include/paging.h"

#define PAGE_PRESENT 0x1
#define PAGE_RW      0x2
#define PAGE_PS      0x80   // PDE 4MB page size
#define PAGE_PWT     0x8    // Page-level write-through
#define PAGE_PCD     0x10   // Page-level cache disable

#define MAX_DYNAMIC_PTS 256
uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));
uint32_t dynamic_pts[MAX_DYNAMIC_PTS][1024] __attribute__((aligned(4096)));

#define KERNEL_PHYS_BASE 0x100000
#define KERNEL_SIZE      0x080000
#define NEW_STACK_TOP    (0x200000)
#define FB_VIRT          0xE0000000

static inline void memset32(void *dst, uint32_t val, uint32_t words) {
    uint32_t *p = (uint32_t*)dst;
    for (uint32_t i=0;i<words;i++) p[i]=val;
}


uint32_t __esp = 0;
extern void set_esp(uint32_t esp)
{
    __esp = esp;
}
extern void __attribute__((cdecl)) get_esp();

void enable_full_paging(uint32_t fb_phys, uint32_t fb_size_bytes, void (*__func_printf)(const char* str, ...)) {


    __func_printf("page_directory @ %x\n", (uint32_t)page_directory);
    __func_printf("first_page_table @ %x\n", (uint32_t)first_page_table);
    __func_printf("dynamic_pts[0] @ %x\n", (uint32_t)&dynamic_pts[0]);
    __func_printf("dynamic_pts[last] @ %x\n", (uint32_t)&dynamic_pts[MAX_DYNAMIC_PTS-1]);
    __func_printf("NEW_STACK_TOP=%x FB_VIRT=%x\n", (uint32_t)NEW_STACK_TOP, (uint32_t)FB_VIRT);
    get_esp();
    __func_printf("before paging: esp=%x pdir=%x ptab=%x fb_phys=%x fb_size=%u\n",
       __esp, (uint32_t)page_directory, (uint32_t)first_page_table,
       fb_phys, fb_size_bytes);
    __func_printf("fb_phys=%x fb_size=%u pages=%u pdes_needed=%u\n",
              fb_phys, (uint32_t)fb_size_bytes, (uint32_t)((fb_size_bytes+0xFFF)/0x1000),
              (uint32_t)((fb_size_bytes + 0x3FFFFF)/0x400000));

    asm volatile("cli");

    memset32(page_directory, 0, 1024);
    memset32(first_page_table, 0, 1024);
    for (int i = 0; i < MAX_DYNAMIC_PTS; ++i) memset32(dynamic_pts[i], 0, 1024);

    // Identity-map first 4 MB (kernel + stack)
    // Map first 8 MB instead of 4 MB
    for (uint32_t i = 0; i < 2048; ++i)
        first_page_table[i % 1024] = ((i * 0x1000) | PAGE_PRESENT | PAGE_RW);

    page_directory[0] = ((uint32_t)first_page_table) | PAGE_PRESENT | PAGE_RW;

    // Add a second PDE for 4–8 MB
    page_directory[1] = ((uint32_t)&dynamic_pts[0]) | PAGE_PRESENT | PAGE_RW;

    size_t fb_start = (size_t)fb_phys;
    size_t fb_end = fb_start + fb_size_bytes;
    size_t cur_phys = fb_start;
    size_t cur_virt = (size_t)FB_VIRT;

    // HEAD: map until we reach a 4MB-aligned physical address or until done
    while (cur_phys < fb_end && (cur_phys & 0x3FFFFFULL)) {
        uint32_t pd_index = (cur_virt >> 22) & 0x3FF;
        uint32_t pt_index = (cur_virt >> 12) & 0x3FF;

        if ((page_directory[pd_index] & PAGE_PRESENT) == 0) {
            // allocate a dynamic PT
            static uint32_t next_pt = 0;
            if (next_pt >= MAX_DYNAMIC_PTS) while (1) asm volatile("hlt");
            page_directory[pd_index] = ((uint32_t)&dynamic_pts[next_pt]) | PAGE_PRESENT | PAGE_RW;
            next_pt++;
        }
        uint32_t *pt = (uint32_t*)(page_directory[pd_index] & 0xFFFFF000);
        pt[pt_index] = ((uint32_t)(cur_phys & 0xFFFFF000ULL)) | PAGE_PRESENT | PAGE_RW | PAGE_PCD | PAGE_PWT;

        cur_phys += 0x1000;
        cur_virt += 0x1000;
    }

    // MIDDLE: map 4MB PDEs where possible (phys and virt must be 4MB aligned)
    while (cur_phys + 0x400000ULL <= fb_end) {
        // ensure both physical and virtual are 4MB aligned before using PDE
        if ((cur_phys & 0x3FFFFFULL) != 0 || (cur_virt & 0x3FFFFFULL) != 0) break;

        uint32_t pd_index = (cur_virt >> 22) & 0x3FF;
        // set PDE with PS bit, include cache-disable
        page_directory[pd_index] = (uint32_t)(cur_phys & 0xFFC00000ULL) | PAGE_PRESENT | PAGE_RW | PAGE_PS | PAGE_PCD | PAGE_PWT;

        cur_phys += 0x400000ULL;
        cur_virt += 0x400000ULL;
    }

    // TAIL: remaining <4MB using PTEs
    while (cur_phys < fb_end) {
        uint32_t pd_index = (cur_virt >> 22) & 0x3FF;
        uint32_t pt_index = (cur_virt >> 12) & 0x3FF;

        if ((page_directory[pd_index] & PAGE_PRESENT) == 0) {
            static uint32_t next_pt2 = 0;
            if (next_pt2 >= MAX_DYNAMIC_PTS) while (1) asm volatile("hlt");
            page_directory[pd_index] = ((uint32_t)&dynamic_pts[next_pt2]) | PAGE_PRESENT | PAGE_RW;
            next_pt2++;
        }
        uint32_t *pt = (uint32_t*)(page_directory[pd_index] & 0xFFFFF000);
        pt[pt_index] = ((uint32_t)(cur_phys & 0xFFFFF000ULL)) | PAGE_PRESENT | PAGE_RW | PAGE_PCD | PAGE_PWT;

        cur_phys += 0x1000;
        cur_virt += 0x1000;
    }

    // Move stack to safe identity-mapped area
    asm volatile("mov %0, %%esp" :: "r"(NEW_STACK_TOP));

    // Load CR3 and enable paging
    asm volatile("mov %0, %%cr3" :: "r"(page_directory));
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000U;
    asm volatile("mov %0, %%cr0" :: "r"(cr0));

    asm volatile("sti");
}