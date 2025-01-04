CC = arm-none-eabi-gcc
CDUMP = arm-none-eabi-objdump
CFLAGS = -mcpu=cortex-m4 \
		 -mthumb \
		 -mfloat-abi=hard \
		 -mfpu=fpv4-sp-d16 \
		 -std=c11 \
		 -pedantic \
		 -O0

# cortex-m4 only supports thumb instructions
# cortex-m4 has a floating-point unit with 16 single-precision registers

all: $1

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

%.asm: %.c 
	$(CC) $(CFLAGS) -S $^ -o $@


