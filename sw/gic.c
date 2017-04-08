#include "io.h"
#include "page.h"
#include "libc.h"

void
gic_init(void)
{
    enable_page_as_io(0xf8f00000, 0x2000);
    io_or32(ICDDCR, 3);
    io_or32(ICCICR, 3);
}

void
gic_config_irq(unsigned int gic_irq,
               int target_cpu_mask,
               int prio,
               int trigger_type)
{
    int reg8 = gic_irq/4U;
    int pos8 = gic_irq%4U;

    int reg2 = gic_irq/16U;
    int pos2 = gic_irq%16U;

    uint32_t mask8 = ~(0xff<<(pos8*8));
    uint32_t mask2 = ~(0x3<<(pos2*2));

    io_mask_or32(ICDIPR(reg8), mask8, prio << pos8);
    io_mask_or32(ICDIPTR(reg8), mask8, target_cpu_mask << pos8);
    io_mask_or32(ICDICFR(reg2), mask2, trigger_type <<pos2);
}

void
gic_enable(unsigned int gic_irq)
{
    int reg = gic_irq/32U;
    int pos = gic_irq%32U;

    io_write32(ICDISER(reg), 1 << pos);
}