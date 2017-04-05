#ifndef IO_H
#define IO_H

#include <stdint.h>

static inline uint32_t io_read32(uintptr_t p) {
    return *(volatile uint32_t*)p;
}
static inline void io_write32(uintptr_t p, uint32_t v) {
    *(volatile uint32_t*)p = v;
}
static inline void dsb(void) {
    __asm__ __volatile__ ("dsb");
}
static inline void dmb(void) {
    __asm__ __volatile__ ("dmb");
}
static inline void isb(void) {
    __asm__ __volatile__ ("isb");
}
static inline void invalidate_tlb(uintptr_t va) {
    __asm__ __volatile__ ("dsb\n\t"
                          "mcr 15, 0, %[va], c8, c7, 3\n\t"
                          :
                          :[va]"r"(va)
                          :"memory");
}

static inline void slcr_unlock() {
    io_write32(0xf800000c, 0xdf0d);
    dmb();
}
static inline void slcr_lock() {
    io_write32(0xf800000c, 0x767b);
    dmb();
}

#endif