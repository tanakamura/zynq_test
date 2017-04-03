#ifndef PAGE_H
#define PAGE_H

#include <stddef.h>
#include <stdint.h>

#define DDR3_SIZE (512 * 1024 * 1024)
#define NUM_PAGE (DDR3_SIZE / (1024*1024))
#define PAGE_SIZE (1<<20)

#define PAGE_ENABLED (1<<0)

enum {
    PAGE_TYPE_IO,
    PAGE_TYPE_UC_MEM,
    PAGE_TYPE_WC_MEM,
    PAGE_TYPE_C_MEM,
};

struct page {
    uint8_t flags;
    uint8_t type;
};

extern struct page pages[NUM_PAGE];

void init_mmu(void);

void enable_page_as_io(uintptr_t pa, size_t length);

#endif