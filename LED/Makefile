CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
OBJDUMP = arm-none-eabi-objdump
OBJCOPY = arm-none-eabi-objcopy

INC_DIR = includes
SRC_DIR = source
CORESYS_DIR = ../coresys
CORE_INC_DIR = ../coresys/includes
STARTUP_DIR = $(CORESYS_DIR)/startup
LINKER_DIR = $(CORESYS_DIR)/linker_script
OUTPUT_DIR = binaries

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
         -nostdlib \
         -nostartfiles \
         -I$(INC_DIR) \
         -I$(CORE_INC_DIR)

LDFLAGS = -T$(LINKER_DIR)/linker_script.ld \
          -Wl,-Map=$(OUTPUT_DIR)/$(TARGET).map \
          --specs=nano.specs \
          --specs=nosys.specs \
          -nostdlib

SRCS = $(wildcard $(SRC_DIR)/*.c)
STARTUP_SRC = $(STARTUP_DIR)/startup.c

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OUTPUT_DIR)/%.o)
STARTUP_OBJ = $(OUTPUT_DIR)/startup.o

all: $(OUTPUT_DIR)/$(TARGET).bin $(OUTPUT_DIR)/$(TARGET).asm

$(OUTPUT_DIR)/$(TARGET).bin: $(OUTPUT_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(OUTPUT_DIR)/$(TARGET).asm: $(OUTPUT_DIR)/$(TARGET).elf
	$(OBJDUMP) -D $< > $@

$(OUTPUT_DIR)/$(TARGET).elf: $(OBJS) $(STARTUP_OBJ)
	$(LD) $(OBJS) $(STARTUP_OBJ) $(LDFLAGS) -o $@

$(OUTPUT_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTPUT_DIR)/startup.o: $(STARTUP_DIR)/startup.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OUTPUT_DIR)

# only .bin files should be flashed as they are the raw binaries without any headers and all, unlike .elf
# debugging should be done using -g .elfs

flash: $(OUTPUT_DIR)/$(TARGET).bin
	st-flash write $< 0x08000000
	st-flash reset

.PHONY: all clean flash