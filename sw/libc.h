#ifndef LIBC_H
#define LIBC_H

#include <stddef.h>

void putc0(int c);
void putchar(int c);
int getchar(void);
void printf(const char *p, ...);
void puts(const char *p);
void memcpy(void *dst, const void *src, size_t sz);
int strcmp(const char *s1, const char *s2);

#endif