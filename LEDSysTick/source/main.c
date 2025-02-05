#include <stdint.h>
#include <stdbool.h>
#include "STM32F401.h"
#include "core_cm4.h"

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
#define GPIOA_EN (0U)

// Pin 5 definitions
#define PIN5 (5U) // Bit 5 in ODR register for PA5
// For output pin: MODER is 01

// Bit manipulation macros
#define SET_BIT(reg, bit) ((reg) |= ((1UL) << (bit)))
#define CLEAR_BIT(reg, bit) ((reg) &= ~((1UL) << (bit)))
#define TOGGLE_BIT(reg, bit) ((reg) ^= ((1UL) << (bit)))

void delay(void);
void setup_systick(void);

void delay(void)
{
    while (true)
    {
        __asm("nop");
    }
}

#define SYS_CLOCK 16000000 // 16MHz is the default system clock without any configuration

void setup_systick(void)
{
#define COUNTER_ENABLE (0)
    SET_BIT(SysTick->CTRL, COUNTER_ENABLE); // enable counter
#define CLOCK_SOURCE (2)
    SET_BIT(SysTick->CTRL, CLOCK_SOURCE); // select the processor clock
#define SYSTICK_INTERRUPT (1)
    SET_BIT(SysTick->CTRL, SYSTICK_INTERRUPT); // enables the system interrupt request

    NVIC_EnableIRQ(SysTick_IRQn); // enable systick interrupts; i think they are also enabled by default?

    SysTick->LOAD = 0x00FFFFFFU & (unsigned int)(1 * SYS_CLOCK - 1); // timer with a period of SYS_CLOCK processor clock cycles
    // systick counts from zero so we subtract a one
}

void SysTick_handler(void)
{
    // these enable and disable interrupt functions are inlined and are single assembly instructions long and typically run
    // in a single CPU cycle thus avoiding function call and instruction overhead
    __disable_irq();           // disable interrupts (atomically) before entering a critical section
    GPIOA->ODR ^= 1UL << PIN5; // a load-modify-store sequence on the GPIOA output data register that can
                               // lead to race conditions if some other interrupt handler also modifies this register between the read and write of this
                               // register in this handler
    __enable_irq();            // enable interrupts (atomically) after leaving a critical section
    // any interrupt that came in between the critical section will not be lost and will execute
    // after the interrupts are enabled again
}

int main(void)
{
    // 1. Enable clock access to GPIOA via AHB1 bus
    SET_BIT(RCC->AHB1ENR, GPIOA_EN);

    // 2. Configure PA5 as general purpose output (01)
    // First clear both mode bits, then set the lower bit
    SET_BIT(GPIOA->MODER, PIN5 * 2);
    CLEAR_BIT(GPIOA->MODER, PIN5 * 2 + 1);
    setup_systick();

    while (true)
        ;
    return 0;
}