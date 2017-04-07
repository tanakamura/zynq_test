#include <stdint.h>
#include <stddef.h>
#include "io.h"
#include "regs.h"
#include "ps7_init.h"
#include "libc.h"
#include "page.h"
#include "cache.h"
#include "zybo.h"

extern unsigned char _ocm_start[], _ocm_end[];
extern unsigned char _heap[];
extern unsigned char _secondary_start[];
extern unsigned char exc_vector[];
extern unsigned char _exc_stack[];
extern unsigned char _exc_stack2[];

struct exception_regs {
    uint32_t r[15];
};

void
fatal(void) {
    puts("fatal : unhandled exception");
    while (1);
}

void
data_abort(struct exception_regs *regs) {
    uint32_t data_addr;
    uint32_t inst_addr = regs->r[14]-8;

    __asm__ __volatile__ ("mrc p15, 0, %0, c6, c0, 0" :"=r"(data_addr));
    
    printf("data abort : pc=%x, addr=%x\n", inst_addr, data_addr);
    while (1);
}

void
irq_handler(void) {
    puts("irq");
    while (1);
}
void
fiq_handler(void) {
    puts("fiq");
    while (1);
}

static void
set_exc_stack(uint32_t mode, void *addr)
{
    uint32_t mask = 0xffffffe0;
    uint32_t cur_cpsr;
    __asm__ __volatile__("mrs %0, cpsr\n\t" : "=r"(cur_cpsr));
    uint32_t new_cpsr = (cur_cpsr & mask) | mode;

    __asm__ __volatile__("msr cpsr, %[new_cpsr]\n\t"
                         "mov r13, %[stack]\n\t"
                         "msr cpsr, %[cur_cpsr]\n\t"
                         :
                         :[new_cpsr]"r"(new_cpsr),
                          [cur_cpsr]"r"(cur_cpsr),
                          [stack]"r"(addr)
                         :"r13");
}

int
main()
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

    set_exc_stack(0x11, _exc_stack);        /* fiq */
    set_exc_stack(0x12, _exc_stack);        /* irq */
    set_exc_stack(0x13, _exc_stack);        /* svc */
    set_exc_stack(0x17, _exc_stack);        /* abt */
    set_exc_stack(0x1b, _exc_stack);        /* und */

    __asm__ __volatile__ ("mcr p15, 0, %0, c12, c0, 0" : : "r"(exc_vector));

    /* uart_freq == 100MHz */
    /* clock = 100MHz/8 = 12.5MHz */

    /* CD = 18, BDIV = 6, baudrate = 115200Hz */

    io_write32(UART1_BASE + UART_BAUD_RATE_GEN, 18);
    io_write32(UART1_BASE + UART_BAUD_RATE_DIV, 6-1);

    puts("Hello, world!");

    uintptr_t scu_base;
    __asm__ __volatile__ ("mrc p15, 4, %0, c15, c0, 0":"=r"(scu_base));
    printf("%p\n", scu_base);

    uint32_t scu_ctrl = io_read32(scu_base + 0);
    io_write32(scu_base + 0, scu_ctrl | 1); /* enable scu */
    io_write32(scu_base + 4, 0xffff);
    printf("num_core = %x\n", (io_read32(scu_base + 4)&3) + 1);

    /* csselr = 0 */
    __asm__ __volatile__ ("mcr p15, 2, %0, c0, c0, 0" : : "r"(0));
    uint32_t reg;
    __asm__ __volatile__ ("mrc p15, 1, %0, c0, c0, 0" :"=r"(reg));

    int num_set = (((reg>>13)&0x7fff)+1);
    int num_way = ((reg>>3)&0x3ff)+1;
    int line_size = (reg&0x7) * 32;
    int total = num_set * num_way * line_size;

    printf("CACHE : numset=%d, numway=%x, linesize=%d, total=%d\n",
           num_set,
           num_way,
           line_size,
           total);

    init_mmu(0);
    puts("enable mmu");
    disable_page(0);            /* disable access to null pointer */

    enable_page_as_io(GPIO_BASE, 4096);
    uint32_t led_bank = GPIO_BANK(GPIO_LED4);
    uint32_t led_bit = GPIO_BIT(GPIO_LED4);

    gpio_set_direction(GPIO_LED4, 1);
    gpio_set_direction(GPIO_BTN4, 0);
    gpio_set_direction(GPIO_BTN5, 0);

    io_write32(GPIO_BASE + GPIO_OEN(led_bank), 1<<led_bit);
    io_write32(GPIO_BASE + GPIO_DATA(led_bank), 1<<led_bit);

    while (1) {
        putchar('>');
        putchar(' ');

        char buffer[1024];
        int i;

        for (i=0; i<1024; i++) {
            buffer[i] = getchar();
            if (buffer[i] == '\r') {
                buffer[i] = '\0';
                putchar('\n');
                break;
            }
            putchar(buffer[i]);
            if (buffer[i] == '\b') {
                i-=2;
                if (i < 0) {
                    i = 0;
                }
            }
        }

        if (strcmp(buffer,"sboot") == 0) {
            uintptr_t tmp_page = alloc_single_page();
            map_address(tmp_page, -PAGE_SIZE, PAGE_TYPE_UC, 1);

            volatile uint32_t *secondary_ptr;
            secondary_ptr = (volatile uint32_t*)(PFN_TO_VA(tmp_page) + PAGE_SIZE - 16);
            *secondary_ptr= (uint32_t)_secondary_start;
            __asm__ __volatile__ ("dmb");
            __asm__ __volatile__ ("sev");

            free_page(tmp_page);
        } else if (strcmp(buffer,"r_btn") == 0) {
            printf("btn[4] = %d\n", gpio_read(GPIO_BTN4));
            printf("btn[5] = %d\n", gpio_read(GPIO_BTN5));
        }
    }

    return 0;
}

int
main2()
{
    set_exc_stack(0x11, _exc_stack2);        /* fiq */
    set_exc_stack(0x12, _exc_stack2);        /* irq */
    set_exc_stack(0x13, _exc_stack2);        /* svc */
    set_exc_stack(0x17, _exc_stack2);        /* abt */
    set_exc_stack(0x1b, _exc_stack2);        /* und */

    __asm__ __volatile__ ("mcr p15, 0, %0, c12, c0, 0" : : "r"(exc_vector));

    init_mmu(1);
    printf("Hello World! (from cpu=1)\n");

    while (1);

    return 0;
}