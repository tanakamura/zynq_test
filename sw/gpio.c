#include "io.h"
#include "regs.h"
#include "page.h"

void
gpio_init(void)
{
    enable_page_as_io(GPIO_BASE, 4096);

    io_write32(GPIO_BASE + GPIO_INT_TYPE(0), ~0);
    io_write32(GPIO_BASE + GPIO_INT_TYPE(1), ~0);
    io_write32(GPIO_BASE + GPIO_INT_TYPE(2), ~0);
    io_write32(GPIO_BASE + GPIO_INT_TYPE(3), ~0);

    io_write32(GPIO_BASE + GPIO_INT_DIS(0), ~0);
    io_write32(GPIO_BASE + GPIO_INT_DIS(1), ~0);
    io_write32(GPIO_BASE + GPIO_INT_DIS(2), ~0);
    io_write32(GPIO_BASE + GPIO_INT_DIS(3), ~0);

    io_write32(GPIO_BASE + GPIO_INT_STAT(0), ~0);
    io_write32(GPIO_BASE + GPIO_INT_STAT(1), ~0);
    io_write32(GPIO_BASE + GPIO_INT_STAT(2), ~0);
    io_write32(GPIO_BASE + GPIO_INT_STAT(3), ~0);
}