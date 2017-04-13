#include <stdarg.h>
#include "regs.h"
#include "io.h"
#include "libc.h"

void
putc0(int c) {
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
putchar(int c) {
    if (c == '\n') {
        putc0('\r');
    }
    putc0(c);
}

int
getchar(void)
{
    while (1) {
        uint32_t st = io_read32(UART1_BASE + UART_CHNL_STS);
        if ((st & 2) == 0) {
            break;
        }
    }

    return io_read32(UART1_BASE + UART_FIFO);
}

static void
put_str(const char *p) {
    while (*p) {
        putchar(*p);
        p++;
    }
}


void
puts(const char *p) {
    put_str(p);

    putchar('\n');
}

void
printf(const char *p, ...)
{
    va_list ap;
    static const char table[]="0123456789abcdef";
    char *s;

    va_start(ap, p);

    while (*p) {
        if (*p != '%') {
            putchar(*p);
            p++;
        } else {
            char digits[32];
            int v;
            unsigned int uv;
            int pos = 0;
            char fmt = p[1];
            p+=2;

            switch (fmt) {
            case 'd':
                v = va_arg(ap, int);
                if (v < 0) {
                    putchar('-');
                    v = -v;     /* oh : ~0 */
                }

                if (v==0) {
                    putchar('0');
                } else {
                    while (v) {
                        digits[pos] = table[v%10];
                        v/=10;
                        pos++;
                    }

                    for (int i=pos-1; i>=0; i--) {
                        putchar(digits[i]);
                    }
                }
                break;

            case 'x':
            case 'p':
                uv = va_arg(ap, unsigned int);

                if (uv==0) {
                    putchar('0');
                } else {
                    while (uv) {
                        digits[pos] = table[uv&0xf];
                        uv>>=4;
                        pos++;
                    }

                    for (int i=pos-1; i>=0; i--) {
                        putchar(digits[i]);
                    }
                }
                break;

            case 's':
                s = va_arg(ap, char *);

                while (*s) {
                    putchar(*s);
                    s++;
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

int
strcmp(const char *s1, const char *s2)
{
    while (*s1 == *s2) {
        if (*s1 == '\0') {
            return 0;
        }
        s1++;
        s2++;
    }

    if (*s1 < *s2) {
        return -1;
    } else {
        return 1;
    }
}

int
isspace(int c) {
    return (c=='\t') || (c==' ') || (c=='\r') || (c == '\n') || (c == '\f') || (c == '\v');
}

long strtol(const char *nptr, char **endptr, int base)
{
    long ret = 0;
    int c;

    c = nptr[0];
    if (! (c >= '0' && c <= '9')) {
        *endptr = (char*)&nptr[0];
        return 0;
    }

    if (base == 0) {
        c = nptr[1];
        if (c == 'x' || c == 'X') {
            base = 16;
            nptr += 2;
        } else {
            base = 10;
        }
    }

    if (base == 16) {
        int i = 0;
        ret = 0;
        while (1) {
            c = nptr[i];
            if (c >= '0' && c <= '9') {
                ret = ret*16 + (c-'0');
            } else if (c >= 'a' && c <= 'f') {
                ret = ret*16 + (c-'a')+10;
            } else if (c >= 'A' && c <= 'F') {
                ret = ret*16 + (c-'A')+10;
            } else {
                *endptr = (char*)&nptr[i];
                return ret;
            }

            i++;
        }
    } else {
        int i = 0;
        ret = 0;
        while (1) {
            c = nptr[i];
            if (c >= '0' && c <= '9') {
                ret = ret*10 + (c-'0');
            } else {
                *endptr = (char*)&nptr[i];
                return ret;
            }

            i++;
        }
    }

}