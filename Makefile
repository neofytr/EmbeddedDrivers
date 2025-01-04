CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
CDUMP = arm-none-eabi-objdump
CFLAGS = -mcpu=cortex-m4 \
		 -mthumb \
		 -mfloat-abi=hard \
		 -mfpu=fpv4-sp-d16 \
		 -std=c11 \
		 -pedantic \
		 -O0 \
		 -Wall \
		 -Wextra
LINKER_SCRIPT = linker_script.ld
LDFLAGS = -Map=main.map

# cortex-m4 only supports thumb instructions (16-bit instructions)
# cortex-m4 has a floating-point unit with 16 single-precision registers

all: main.o startup.o
	$(LD) $(LDFLAGS) -T $(LINKER_SCRIPT) $^ -o main.out 

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

%.asm: %.c 
	$(CC) $(CFLAGS) -S $^ -o $@

dump:
	$(CDUMP) -D main.out > dump.txt

clean:
	rm -rf *.o *.asm *.out *.txt

