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
#define GPIOA_EN        (1UL << 0)

// Pin 5 definitions
#define PIN5            (1UL << 5)    // Bit 5 in ODR register for PA5
#define PIN5_MODE_BITS  (10U)         // Starting bit position for PA5 mode in MODER
#define PIN5_OUT_MODE   (1UL << 10)   // Sets bits 10-11 to 01 (output mode)

// Bit manipulation macros
#define SET_BIT(reg, mask)   ((reg) |= (mask))
#define CLEAR_BIT(reg, mask) ((reg) &= ~(mask))
#define TOGGLE_BIT(reg, mask) ((reg) ^= (mask))

int main(void)
{
    // 1. Enable clock access to GPIOA via AHB1 bus
    SET_BIT(RCC->AHB1ENR, GPIOA_EN);
    
    // 2. Configure PA5 as general purpose output (01)
    // First clear both mode bits, then set the lower bit
    CLEAR_BIT(GPIOA->MODER, (3UL << PIN5_MODE_BITS));  // Clear bits 10-11
    SET_BIT(GPIOA->MODER, PIN5_OUT_MODE);              // Set bit 10
    
    // 3. Main loop - toggle LED with delay
    while (true)
    {
        // Toggle PA5 output state
        TOGGLE_BIT(GPIOA->ODR, PIN5);
        
        // Simple delay loop
        // Note: In production code, use a proper timer instead
        for (volatile uint32_t i = 0; i < 100000; i++)
        {
        }
    }
    
    return 0; // Never reached
}