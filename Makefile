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
LDFLAGS = -nostdlib

# cortex-m4 only supports thumb instructions (16-bit instructions)
# cortex-m4 has a floating-point unit with 16 single-precision registers

all: main.o startup.o final.out

%.out: %.o 
	$(LD) $(LDFLAGS) -T $(LINKER_SCRIPT) $^ -o $@ 

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

%.asm: %.c 
	$(CC) $(CFLAGS) -S $^ -o $@

dump:
	$(CDUMP) -D startup.o > dump.txt

clean:
	rm -rf *.o *.asm *.out *.txt

