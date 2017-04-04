#include "cache.h"

void
cache_op_l1d_all(enum cache_op op)
{
    /* csselr = 0 */
    __asm__ __volatile__ ("mcr p15, 2, %0, c0, c0, 0" : : "r"(0));
    uint32_t reg;
    __asm__ __volatile__ ("mrc p15, 1, %0, c0, c0, 0" :"=r"(reg));

    int num_set = (((reg>>13)&0x7fff)+1);
    int num_way = ((reg>>3)&0x3ff)+1;
    int line_size = (reg&0x7) * 32;
    int total = num_set * num_way * line_size;

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