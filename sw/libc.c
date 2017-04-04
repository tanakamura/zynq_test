#include <stdarg.h>
#include "regs.h"
#include "io.h"
#include "libc.h"

static void
putc0(char c) {
    while (1) {
        uint32_t st = io_read32(UART1_BASE + UART_CHNL_STS);
        if (st & (1<<4)) {      /* TFUL */
            continue;
        }

        io_write32(UART1_BASE + UART_FIFO, c);
        break;
    }
}

void
putc(char c) {
    if (c == '\n') {
        putc0('\r');
    }
    putc0(c);
}

static void
put_str(const char *p) {
    while (*p) {
        putc(*p);
        p++;
    }
}


void
puts(const char *p) {
    put_str(p);

    putc('\n');
}

void
printf(const char *p, ...)
{
    va_list ap;
    static const char table[]="0123456789abcdef";

    va_start(ap, p);

    while (*p) {
        if (*p != '%') {
            putc(*p);
            p++;
        } else {
            char digits[32];
            int v;
            unsigned int uv;
            int neg = 0;
            int pos = 0;
            char fmt = p[1];
            p+=2;

            switch (fmt) {
            case 'd':
                v = va_arg(ap, int);
                if (v < 0) {
                    putc('-');
                    v = -v;     /* oh : ~0 */
                }

                if (v==0) {
                    putc('0');
                } else {
                    while (v) {
                        digits[pos] = table[v%10];
                        v/=10;
                        pos++;
                    }

                    for (int i=pos-1; i>=0; i--) {
                        putc(digits[i]);
                    }
                }
                break;

            case 'x':
            case 'p':
                uv = va_arg(ap, unsigned int);

                if (uv==0) {
                    putc('0');
                } else {
                    while (uv) {
                        digits[pos] = table[uv&0xf];
                        uv>>=4;
                        pos++;
                    }

                    for (int i=pos-1; i>=0; i--) {
                        putc(digits[i]);
                    }
                }
                break;
            }
        }
    }

    va_end(ap);
}


void memcpy(void *dst, const void *src, size_t sz)
{
    unsigned char *pd = dst;
    const unsigned char *ps = src;

    size_t i = 0;

    if (((uintptr_t)pd & 0x7) || ((uintptr_t)ps & 0x7)) {
    } else {
        for (; i<sz; i+=8) {
            uint64_t *ps8 = (uint64_t*)(ps+i);
            uint64_t *pd8 = (uint64_t*)(pd+i);

            *pd8 = *ps8;
        }
        
    }

    for (; i<sz; i++) {
        pd[i] = ps[i];
    }
}