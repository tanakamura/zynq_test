#ifndef LIBC_H
#define LIBC_H

#include <stddef.h>

void putc(char c);
void printf(const char *p, ...);
void puts(const char *p);
void memcpy(void *dst, const void *src, size_t sz);

#endif