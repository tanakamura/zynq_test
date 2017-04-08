#include "cache.h"
#include "regs.h"
#include "io.h"

void
cache_op_d_all(enum cache_op op)
{
    cache_op_l1d_all(op);
    cache_op_l2_all(op);
}


void
cache_op_l1d_all(enum cache_op op)
{
    /* csselr = 0 */
    __asm__ __volatile__ ("mcr p15, 2, %0, c0, c0, 0" : : "r"(0));
    uint32_t reg;
    __asm__ __volatile__ ("mrc p15, 1, %0, c0, c0, 0" :"=r"(reg));

    int num_set = (((reg>>13)&0x7fff)+1);
    int num_way = ((reg>>3)&0x3ff)+1;

    for (int si=0; si<num_set; si++) {
        for (int wi=0; wi<num_way; wi++) {
            uint32_t reg = (wi<<30) | (si<<5);

            switch (op) {
            case CACHE_INVALIDATE:
                __asm__ __volatile__("mcr p15, 0, %0, c7, c6, 2" : : "r"(reg));
                break;
            case CACHE_WRITEBACK:
                __asm__ __volatile__("mcr p15, 0, %0, c7, c10, 2" : : "r"(reg));
                break;
            case CACHE_WRITEBACK_INVALIDATE:
                __asm__ __volatile__("mcr p15, 0, %0, c7, c14, 2" : : "r"(reg));
                break;
            }
        }
    }
}

void
cache_op_l2_all(enum cache_op op)
{
    uintptr_t reg = 0;

    switch (op) {
    case CACHE_INVALIDATE:
        reg = PL310_BASE + PL310_INV_WAY;
        break;

    case CACHE_WRITEBACK:
        reg = PL310_BASE + PL310_CLEAN_WAY;
        break;

    case CACHE_WRITEBACK_INVALIDATE:
        reg = PL310_BASE + PL310_CLEAN_INV_WAY;
        break;
    }

    io_write32(reg, 0xffff);

    while (1) {
        if (io_read32(reg) == 0)
            break;
    }

    io_write32(PL310_BASE + PL310_SYNC, 0);
}

void
cache_op_va(enum cache_op op,
            uintptr_t va, size_t length)
{
    uintptr_t off = 0;

    switch (op) {
    case CACHE_INVALIDATE:
        for (off=0; off<length; off+=32) {
            asm volatile("mcr p15, 0, %0, c7, c6, 1": :"r"(va + off));
            io_write32(PL310_BASE + PL310_INV_PA, (va + off));
        }
        break;

    case CACHE_WRITEBACK:
        for (off=0; off<length; off+=32) {
            asm volatile("mcr p15, 0, %0, c7, c10, 1": :"r"(va + off));
            io_write32(PL310_BASE + PL310_CLEAN_PA, (va + off));
        }
        break;

    case CACHE_WRITEBACK_INVALIDATE:
        for (off=0; off<length; off+=32) {
            asm volatile("mcr p15, 0, %0, c7, c14, 1": :"r"(va + off));
            io_write32(PL310_BASE + PL310_CLEAN_INV_PA, (va + off));
        }
        break;
    }
}
