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

void setup_systick(void);
void LED_setup(void);

#define SYS_CLOCK 16000000U // 16MHz is the default system clock without any configuration
#define SMALLEST_UNIT_OF_TIME_MEASURED (1000U)

volatile uint32_t tick_count = 0; // global tick counter; each tick measures 1 milisecond passed

void setup_systick(void)
{
#define COUNTER_ENABLE (0)
    SET_BIT(SysTick->CTRL, COUNTER_ENABLE); // enable counter
#define CLOCK_SOURCE (2)
    SET_BIT(SysTick->CTRL, CLOCK_SOURCE); // select the processor clock
#define SYSTICK_INTERRUPT (1)
    SET_BIT(SysTick->CTRL, SYSTICK_INTERRUPT); // enables the system interrupt request

    NVIC_EnableIRQ(SysTick_IRQn); // enable systick interrupts; i think they are also enabled by default?

    SysTick->LOAD = 0x00FFFFFFU & (unsigned int)(1 * SYS_CLOCK / SMALLEST_UNIT_OF_TIME_MEASURED - 1); // timer with a period of SYS_CLOCK processor clock cycles
    // so the systick interrupt goes off roughly every milisecond
    // systick counts from zero so we subtract a one
}

void SysTick_handler(void)
{
    tick_count++; // there is no other function (except the main function, which will disable interrupts
    // before accessing the tick_count) that accesses this global variable so we don't need to put disable and
    // enable irq before and after accessing this variable here
}

void LED_setup(void)
{
    // 1. Enable clock access to GPIOA via AHB1 bus
    SET_BIT(RCC->AHB1ENR, GPIOA_EN);

    // 2. Configure PA5 as general purpose output (01)
    // First clear both mode bits, then set the lower bit
    SET_BIT(GPIOA->MODER, PIN5 * 2);
    CLEAR_BIT(GPIOA->MODER, PIN5 * 2 + 1);
}

int main(void)
{

    setup_systick();

    while (true)
        ;
    return 0;
}