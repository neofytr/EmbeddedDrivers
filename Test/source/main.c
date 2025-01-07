#include <stdint.h>
#include <stdbool.h>
#include "STM32F401.h"

// Bit manipulation macros
#define SET_BIT(reg, bit) ((reg) |= ((1UL) << (bit)))
#define CLEAR_BIT(reg, bit) ((reg) &= ~((1UL) << (bit)))
#define TOGGLE_BIT(reg, bit) ((reg) ^= ((1UL) << (bit)))

unsigned fact(unsigned n)
{
    if (!n)
    {
        return 1;
    }
    else
    {
        return n * fact(n - 1);
    }
}

void delay(void)
{
    for (unsigned volatile counter = 0; counter < 100000; counter++)
    {
        __asm("nop");
    }
}

int main(void)
{
    unsigned num = 0;
    while (true)
    {
        fact(num++);
        delay();
    }

    return 0;
}