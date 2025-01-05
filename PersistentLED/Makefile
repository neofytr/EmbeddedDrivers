CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
GDB = arm-none-eabi-gdb
OBJDUMP = arm-none-eabi-objdump
OBJCOPY = arm-none-eabi-objcopy

INC_DIR = includes
SRC_DIR = source
CORESYS_DIR = ../coresys
CORE_INC_DIR = ../coresys/includes
STARTUP_DIR = $(CORESYS_DIR)/startup
SYSCALL_DIR = $(CORESYS_DIR)/syscalls
LINKER_DIR = $(CORESYS_DIR)/linker_script
OUTPUT_DIR = binaries

GDB_CMDS_FILE = gdbcmds.txt

TARGET = output

$(shell mkdir -p $(OUTPUT_DIR))

CFLAGS = -mcpu=cortex-m4 \
         -mthumb \
         -mfloat-abi=hard \
         -mfpu=fpv4-sp-d16 \
         -std=c11 \
         -O0 \
         -Wall \
         -Wextra \
         -g3 \
         -I$(INC_DIR) \
         -I$(CORE_INC_DIR)

LDFLAGS = -T$(LINKER_DIR)/linker_script.ld \
 -Wl,-Map=$(OUTPUT_DIR)/$(TARGET).map \
 -mcpu=cortex-m4 \
 -mthumb \
 -mfloat-abi=hard \
 -mfpu=fpv4-sp-d16 \
 --specs=nano.specs

# Added -mcpu=cortex-m4 to specify the processor
# Added -mthumb for Thumb instruction set
# This ensures consistent ABI settings throughout both compilation and linking stages, since we're linking with the nano C library

# --specs=nano.specs instructs the linker to use the nano libc; 
# --specs=nosys.specs instructs the linker to not use any systems calls; we don't need this
# since we've implemented the syscalls

SRCS = $(wildcard $(SRC_DIR)/*.c)
STARTUP_SRC = $(STARTUP_DIR)/startup.c
SYSCALL_SRC = $(SYSCALL_DIR)/syscall.c

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OUTPUT_DIR)/%.o)
STARTUP_OBJ = $(OUTPUT_DIR)/startup.o
SYSCALL_OBJ = $(OUTPUT_DIR)/syscall.o

all: $(OUTPUT_DIR)/$(TARGET).bin $(OUTPUT_DIR)/$(TARGET).asm 

$(OUTPUT_DIR)/$(TARGET).bin: $(OUTPUT_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(OUTPUT_DIR)/$(TARGET).asm: $(OUTPUT_DIR)/$(TARGET).elf
	$(OBJDUMP) -D $< > $@

$(OUTPUT_DIR)/$(TARGET).elf: $(OBJS) $(STARTUP_OBJ) $(SYSCALL_OBJ)
	$(LD) $(OBJS) $(STARTUP_OBJ) $(SYSCALL_OBJ) $(LDFLAGS) -o $@

$(OUTPUT_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTPUT_DIR)/startup.o: $(STARTUP_DIR)/startup.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTPUT_DIR)/syscall.o: $(SYSCALL_DIR)/syscall.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OUTPUT_DIR)

reset:
	st-flash reset 

erase:
	st-flash erase

rcnt_erase:
	st-flash --connect-under-reset erase

gdb: $(OUTPUT_DIR)/$(TARGET).elf
	st-util &
	$(GDB) -q -x $(GDB_CMDS_FILE) ./binaries/output.elf


# only .bin files should be flashed as they are the raw binaries without any headers and all, unlike .elf
# debugging should be done using -g .elfs

flash: $(OUTPUT_DIR)/$(TARGET).bin
	st-flash write $< 0x08000000
	st-flash reset

.PHONY: all clean flash

gdb: $(OUTPUT_DIR)/$(TARGET).elf
	$(GDB) -q -x $(GDB_CMDS_FILE) ./binaries/output.elf
