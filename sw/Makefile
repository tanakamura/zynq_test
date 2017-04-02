all: main.elf

CC=arm-xilinx-eabi-gcc
AS=arm-xilinx-eabi-gcc -c

OBJS=start.o main.o
main.elf: $(OBJS) link.lds
	$(CC) -v -Wl,-T,link.lds -Wl,-Map,main.map -nostdlib -o $@ $(OBJS)

clean:
	rm -f *~ *.o main.elf main.map