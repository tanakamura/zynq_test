#include <stdint.h>
#include "ps7_init.h"

#define UART1_BASE 0xe0001000
#define UART_CONTROL 0x0
#define UART_MODE 0x4
#define UART_INT_EN 0x8
#define UART_INT_DIS 0xc
#define UART_BAUD_RATE_GEN 0x18
#define UART_CHNL_STS 0x2c
#define UART_FIFO 0x30
#define UART_BAUD_RATE_DIV 0x34

static inline uint32_t io_read32(uintptr_t p) {
    return *(volatile uint32_t*)p;
}
static inline void io_write32(uintptr_t p, uint32_t v) {
    return *(volatile uint32_t*)p = v;
}

static void
putc(char c) {
    while (1) {
        uint32_t st = io_read32(UART1_BASE + UART_CHNL_STS);
        if (st & (1<<4)) {      /* TFUL */
            continue;
        }

        io_write32(UART1_BASE + UART_FIFO, c);
        break;
    }
}

static void
puts(const char *p) {
    while (*p) {
        putc(*p);
        p++;
    }

    putc('\r');
    putc('\n');
}

int main()
{
    uint32_t mode = 0;
    mode |= 0x1;                /* div 8 */
    mode |= 0<<1;               /* 8 */
    mode |= 0x4<<3;             /* N */
    mode |= 0x0<<6;             /* 1 */
    io_write32(UART1_BASE + UART_MODE, mode);

    uint32_t ctrl = io_read32(UART1_BASE + UART_CONTROL);
    ctrl |= (1<<2) | (1<<4);    /* enable rx/tx */
    io_write32(UART1_BASE + UART_CONTROL, ctrl);

    /* uart_freq == 100MHz */
    /* clock = 100MHz/8 = 12.5MHz */

    /* CD = 18, BDIV = 6, baudrate = 115200Hz */

    io_write32(UART1_BASE + UART_BAUD_RATE_GEN, 18);
    io_write32(UART1_BASE + UART_BAUD_RATE_DIV, 6-1);

    puts("Hello, world!");

    while (1)
        ;
    
}