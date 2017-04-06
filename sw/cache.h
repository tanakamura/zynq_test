#ifndef CACHE_H
#define CACHE_H

#include <stddef.h>
#include <stdint.h>

enum cache_op {
    CACHE_INVALIDATE,
    CACHE_WRITEBACK,
    CACHE_WRITEBACK_INVALIDATE
};

void cache_op_l1d_all(enum cache_op op);
void cache_op_l1i_all(enum cache_op op);
void cache_op_l2d_all(enum cache_op op);
void cache_op_d_all(enum cache_op op);

void cache_op_l1i_va(enum cache_op op, 
                     uintptr_t va,
                     size_t length);
void cache_op_l1d_va(enum cache_op op, 
                     uintptr_t va,
                     size_t length);
void cache_op_l2_pa(enum cache_op op, 
                    uintptr_t pa,
                    size_t length);

/* va & length should align to cacheline(32byte) */
void cache_op_va(enum cache_op op,
                 uintptr_t va, size_t length);

#endif