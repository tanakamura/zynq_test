#include <stdint.h>
#include "page.h"
#include "libc.h"
#include "regs.h"
#include "io.h"
#include "cache.h"

static uint32_t lv1_pgtable[4096] __attribute__((aligned(16384)));
extern unsigned char _loaded[];
struct page pages[NUM_PAGE];

uint16_t texremap_table[] = {
    (0<<12) | (0<<3) | (0<<2),  /* PAGE_TYPE_UC */
    (0<<12) | (0<<3) | (1<<2),  /* PAGE_TYPE_WC */
    (0<<12) | (1<<3) | (0<<2),  /* PAGE_TYPE_C */
};

/*
 * 0 = nc
 * 1 = wb,wa
 * 2 = wt,no-wa
 * 3 = wb,no-wa
 */

#define TABLE8(v0,v1,v2,v3,v4,v5,v6,v7)              \
    ((v0<<0) | (v1<<2) | (v2<<4) | (v3<<6) |         \
     (v4<<8) | (v5<<10) | (v6<<12) | (v7<<14))


#define NMRR_TABLE(v0,v1,v2,v3,v4,v5,v6,v7)     \
    TABLE8(v0,v1,v2,v3,v4,v5,v6,v7)             \


#define NMRR_VAL (                              \
        (NMRR_TABLE(0,0,1,0,0,0,0,0)<<16) |     \
        (NMRR_TABLE(0,0,1,0,0,0,0,0))           \
        )

/* 0 strong order
 * 1 device
 * 2 normal memory
 * 3 reserved
 */

#define REMAP_TABLE(v0,v1,v2,v3,v4,v5,v6,v7)    \
    TABLE8(v0,v1,v2,v3,v4,v5,v6,v7)             \

#define PRRR_VAL (                                                      \
        ((0xff) << 24) | /* all shareable */                            \
        ((0x5) << 16) | /* shareable = 0101 */                          \
        (REMAP_TABLE(1,2,2,0, 0,0,0,0)  << 0)  /* */                    \
        )

void
init_mmu(void)
{
    uintptr_t pa = 0;
    uintptr_t elf_end = (uintptr_t)_loaded;

    /*   addr |NS| 0|nG| S|AP2|TEX[3]|AP[2]| 0|domain|XN C B|1|0|
     * |31  20|19|18|17|16| 15|14  12|11 10| 9|8    5|4  3 2|1|0|
     */

    for (int i=0; i<4096; i++) {
        uint32_t val = pa;

        if (pa < elf_end) {
            val |= 0x2;                         /* section */
            val |= (3<<10);                     /* enable access */
            val |= (1<<16);                     /* shareable */
            //val |= (0x4<<12) | (0<<3) | (1<<2); /* cachable, wb alloc */
            val |= texremap_table[PAGE_TYPE_C];
            pages[i].flags = PAGE_ENABLED;
            pages[i].type = PAGE_TYPE_C;
        } else {
            pages[i].flags = 0;
            pages[i].type = 0;
        }
        lv1_pgtable[i] = val;

        pa += 1<<20;
    }

    /* IRGN[0] NOS  RGN  NOS IMP  IRGN[1]
     *   6      5   4 3   2   1     0
     *   1      0   1 1   0   0     1
     *        5          9
     */

    slcr_unlock();
    io_write32(0xf8000a1c, 0x020202);
    slcr_lock();

    /* from xilinx sdk */
    io_write32(PL310_BASE + PL310_TAG_RAM_CONTROL, 0x111);
    io_write32(PL310_BASE + PL310_DATA_RAM_CONTROL, 0x121);
    io_write32(PL310_BASE + PL310_AUX_CONTROL, 0x72360000);

    cache_op_l1d_all(CACHE_INVALIDATE);
    cache_op_l2d_all(CACHE_INVALIDATE);

    __asm__ __volatile__ ("mcr p15, 0, %0, c10, c2, 0" : : "r"(PRRR_VAL));
    __asm__ __volatile__ ("mcr p15, 0, %0, c10, c2, 1" : : "r"(NMRR_VAL));

    uint32_t ttbr0_val = (uint32_t)lv1_pgtable;
    ttbr0_val |= 0x59;

    /* enable mmu */
    __asm__ __volatile__(//"1: b 1b\n\t"
                         "mcr 15, 0, %[zero], c8, c7, 0\n\t" /* invalidate tlb */
                         "mcr 15, 0, %[zero], c7, c5, 0\n\t" /* invalidate icache */
                         "mcr 15, 0, %[zero], c7, c5, 6\n\t" /* invalidate btb */
                         //"mcr 15, 0, %[zero], c7, c6, 0\n\t" /* invalidate dcache */
                         "mcr 15, 0, %[ttbr0], c2, c0, 0\n\t"
                         "mcr 15, 0, %[domains], c3, c0, 0\n\t"
                         "mcr 15, 0, %[sctlr], c1, c0, 0\n\t"
                         "dsb\n\t"
                         "isb\n\t"
                         :
                         :[ttbr0]"r"(ttbr0_val),
                          [domains]"r"(-1),
                          [zero]"r"(0),
                          [sctlr]"r"((1<<28) | (1<<12) | 0x7) /* enable tex remap, mmu, i cache,d cache */
                         :"memory"
        );

    enable_page_as_io(UART1_BASE, 4096);
    enable_page_as_io(PL310_BASE, 4096);

    io_write32(PL310_BASE + PL310_REG1_CONTROL, 1); /* enable l2 */

    puts("enable tlb");
}

void enable_page_as_io(uintptr_t pa,
                       size_t length)
{
    uintptr_t pa_start = pa & ~(PAGE_SIZE-1);
    uintptr_t pa_end = pa + length;
    pa_end = (pa_end + (PAGE_SIZE-1)) & ~(PAGE_SIZE-1);

    uintptr_t num_page = (pa_end - pa_start)/PAGE_SIZE;
    uintptr_t pfn_start = pa_start / PAGE_SIZE;

    for (uintptr_t pi=0; pi<num_page; pi++) {
        struct page *p = &pages[pi + pfn_start];
        if (p->flags & PAGE_ENABLED) {
            if (p->type != PAGE_TYPE_UC) {
                puts("fatal : mismatch page type");
                while(1);
            }
        } else {
            p->flags |= PAGE_ENABLED;
            p->type = PAGE_TYPE_UC;

            uintptr_t pa = pa_start + pi * PAGE_SIZE;
            uint32_t val = pa;
            val |= 0x2;                       /* section */
            val |= (3<<10);                   /* enable access */
            val |= (1<<16);                   /* shareable */
            val |= texremap_table[PAGE_TYPE_UC];

            uint32_t *entry_ptr = &lv1_pgtable[(pi + pfn_start)];
            *entry_ptr = val;
            invalidate_tlb(pa);
        }
    }
}   
