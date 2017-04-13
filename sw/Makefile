all: main.elf

CC=arm-xilinx-eabi-gcc
AS=arm-xilinx-eabi-gcc -c

CFLAGS=-O2 -Wall -fno-builtin -std=gnu99 -MMD -Werror

OBJS=start.o exception.o main.o libc.o page.o cache.o gic.o gpio.o console.o
main.elf: $(OBJS) link.lds
	$(CC) -v -Wl,-T,link.lds -Wl,-Map,main.map -nostdlib -o $@ $(OBJS)

clean:
	rm -f *~ *.o main.elf main.map

-include *.d