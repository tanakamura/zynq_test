#ifndef REGS_H
#define REGS_H

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

#define GPIO_BANK(MIO) ((MIO)/32U)
#define GPIO_BIT(MIO) ((MIO)%32U)

#endif