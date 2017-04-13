#ifndef REGS_H
#define REGS_H

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

#define PL310_BASE 0xF8F02000

#define PL310_SYNC 0x730

#define PL310_INV_PA 0x770

#define PL310_INV_WAY 0x77c

#define PL310_CLEAN_PA 0x7b0
#define PL310_CLEAN_INDEX_WAY 0x7b8
#define PL310_CLEAN_WAY 0x7bc

#define PL310_CLEAN_INV_PA 0x7f0
#define PL310_CLEAN_INV_INDEX_WAY 0x7f8
#define PL310_CLEAN_INV_WAY 0x7fc

#define PL310_REG1_CONTROL 0x100
#define PL310_AUX_CONTROL 0x104
#define PL310_TAG_RAM_CONTROL 0x108
#define PL310_DATA_RAM_CONTROL 0x10c

#define GPIO_BASE 0xe000a000

#define GPIO_DATA(N) (0x00000040 + (N)*4)
#define GPIO_DATA_READ(N) (0x00000060 + (N)*4)
#define GPIO_DIRM(N) (0x00000204 + (N)*0x40)
#define GPIO_OEN(N) (0x00000208 + (N)*0x40)

#define GPIO_INT_MASK(N) (0x0000020c + (N*0x40))
#define GPIO_INT_EN(N) (0x00000210 + (N*0x40))
#define GPIO_INT_DIS(N) (0x00000214 + (N*0x40))
#define GPIO_INT_STAT(N) (0x00000218 + (N*0x40))
#define GPIO_INT_TYPE(N) (0x0000021c + (N*0x40))
#define GPIO_INT_POLARITY(N) (0x00000220 + (N*0x40))
#define GPIO_INT_ANY(N) (0x00000224 + (N*0x40))

#define GPIO_BANK(MIO) ((MIO)/32U)
#define GPIO_BIT(MIO) ((MIO)%32U)

#define SCU_BASE 0xf8f00000

#define ICCICR 0xf8f00100
#define ICCPMR 0xf8f00104
#define ICCIAR 0xf8f0010C
#define ICCEOIR 0xf8f00110
#define ICCRPR 0xf8f00114
#define ICCHPIR 0xf8f00118

#define ICDDCR 0xf8f01000
#define ICDICTR 0xf8f01004
#define ICDIIDR 0xf8f01008

#define spi_status(n) (0xf8f01d04 + (n)*4)

#define ICDISER(N) (0xf8f01100 + (N)*4)
#define ICDIPR(N) (0xf8f01400 + (N)*4)
#define ICDIPTR(N) (0xf8f01800 + (N)*4)
#define ICDICFR(N) (0xf8f01C00 + (N)*4)
#define ICDABR(N) (0xf8f01300 + (N)*4)

#define IRQ_GPIO 52
#define IRQ_USB0 53
#define IRQ_ENET0 54
#define IRQ_ENET0_WAKEUP 55
#define IRQ_I2C0 57
#define IRQ_SPI0 58
#define IRQ_UART0 59
#define IRQ_CAN0 60
#define IRQ_PL(N) (61 + N)
#define IRQ_TTC1 (69)

#define GTC_REG0 0xf8f00200
#define GTC_REG1 0xf8f00204
#define GTC_CONTROL 0xf8f00208

#define GTC_FREQ (APU_FREQ/2)

static inline uint32_t
read_gtc_32(void)
{
    return *(volatile uint32_t*)GTC_REG0;
}

static inline uint64_t
read_gtc_64(void)
{
    uint32_t lo, hi, hi2;

retry:
    hi = *(volatile uint32_t*)GTC_REG1;
    lo = *(volatile uint32_t*)GTC_REG0;
    hi2 = *(volatile uint32_t*)GTC_REG1;

    if (hi != hi2) {
        goto retry;
    }

    return ((uint64_t)hi<<32) | lo;
}

#endif
