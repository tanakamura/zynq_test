#include <stdint.h>
#include <stddef.h>
#include "io.h"
#include "uart.h"
#include "ps7_init.h"
#include "libc.h"
#include "page.h"

extern unsigned char _ocm_start[], _ocm_end[];
extern unsigned char _heap[];

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

    init_mmu();

    while (1)
        ;
    
}