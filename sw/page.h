#ifndef PAGE_H
#define PAGE_H

#include <stddef.h>
#include <stdint.h>

#define DDR3_SIZE (512 * 1024 * 1024)
#define NUM_PAGE (DDR3_SIZE / (1024*1024))
#define PAGE_SIZE (1<<20)

#define PAGE_ALLOCATED (1<<0)

enum {
    PAGE_TYPE_UC,
    PAGE_TYPE_WC,
    PAGE_TYPE_C,
    PAGE_TYPE_C_NS,             // NON SHAREABLE
};

#define PAGE_TYPE_WC_MEM PAGE_TYPE_WC_IO

extern uint16_t texremap_table[];

struct page {
    uint8_t flags;
    uint8_t type;
};

extern struct page pages[NUM_PAGE];

void init_mmu(int cpu);

/* caller should clear cache in page */
void enable_page(uintptr_t pa,size_t length,
                 int page_type, int shareable);

static inline void enable_page_as_io(uintptr_t pa, size_t length)
{
    enable_page(pa, length, PAGE_TYPE_UC, 1);
}
static inline void enable_page_as_c(uintptr_t pa, size_t length)
{
    enable_page(pa, length, PAGE_TYPE_C, 1);
}
static inline void enable_page_as_c_ns(uintptr_t pa, size_t length)
{
    enable_page(pa, length, PAGE_TYPE_C, 0);
}

uintptr_t alloc_single_page(void);
void free_page(uintptr_t pfn);
void disable_page(uintptr_t pfn);

void map_address(uintptr_t pfn,uintptr_t pa,int page_type, int shareable);

#define PFN_TO_VA(pfn) ((pfn) * PAGE_SIZE)

#endif