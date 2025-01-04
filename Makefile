CC = arm-none-eabi-gcc
CDUMP = arm-none-eabi-objdump
CFLAGS = -mcpu=cortex-m4 -mthumb

# cortex-m4 only supports thumb instructions

all:
	$(CC) $(CFLAGS) -c main.c -o main.o

dis:
	$(CDUMP) -d main.o
