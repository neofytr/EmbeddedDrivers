#include <stdint.h>

#define SRAM_START 0x20000000U
#define SRAM_SIZE (96 * 1024) // 96 KBs
#define SRAM_END ((SRAM_START) + (SRAM_SIZE))

void ResetHandler(void);

uint32_t vectors[] = {
    (uint32_t)0,
    (uint32_t)&ResetHandler,
};

void ResetHandler(void)
{
}