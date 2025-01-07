#include <stdint.h>
#include <stdbool.h>
#include "STM32F401.h"

/*
 * Hardware Configuration:
 * LED is connected to PA5 (Port A, Pin 5)
 *
 * Clock Configuration:
 * GPIOA is connected to the AHB1 bus
 * Clock must be enabled via RCC->AHB1ENR before GPIOA can be used
 *
 * GPIO Configuration:
 * Each GPIO pin has 2 mode bits in the MODER register
 * For PA5, these are bits 10-11 (2 bits per pin, so pin 5 * 2)
 * Mode 01 = General Purpose Output
 */

// Clock enable bit for GPIOA in RCC_AHB1ENR register
#define GPIOA_EN (0)

// Pin 5 definitions
#define PIN5 (5) // Bit 5 in ODR register for PA5
// For output pin: MODER is 01

// Bit manipulation macros
#define SET_BIT(reg, bit) ((reg) |= ((1UL) << (bit)))
#define CLEAR_BIT(reg, bit) ((reg) &= ~((1UL) << (bit)))
#define TOGGLE_BIT(reg, bit) ((reg) ^= ((1UL) << (bit)))

void delay(void);

void delay(void)
{
    while (true)
    {
        __asm("nop");
    }
}

int main(void)
{
    // 1. Enable clock access to GPIOA via AHB1 bus
    SET_BIT(RCC->AHB1ENR, GPIOA_EN);

    // 2. Configure PA5 as general purpose output (01)
    // First clear both mode bits, then set the lower bit
    SET_BIT(GPIOA->MODER, PIN5 * 2);
    CLEAR_BIT(GPIOA->MODER, PIN5 * 2 + 1);

    GPIOA->BSRR = 1UL << PIN5;

    delay();
    return 0;
}