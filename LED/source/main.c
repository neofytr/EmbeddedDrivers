#include <stdint.h>
#include "STM32F401.h"
#include <stdlib.h>

#define GPIOA_EN (1U << 0)
#define LED_PIN (1U << 5)
#define LED_MODE (1U << 10)

int main(void)
{
    // Enable GPIOA clock
    RCC->AHB1ENR |= GPIOA_EN;

    // Clear the mode bits for PA5
    GPIOA->MODER &= ~(3U << 10); // Clear bits 10-11
    // Set PA5 as output
    GPIOA->MODER |= (1U << 10); // Set bit 10

    // Make sure pin is push-pull (default, but let's be explicit)
    GPIOA->OTYPER &= ~(1U << 5);

    // Set to low speed
    GPIOA->OSPEEDR &= ~(3U << 10);

    // No pull-up or pull-down
    GPIOA->PUPDR &= ~(3U << 10);

    // void *ptr = malloc(10);

    while (1)
    {
        GPIOA->BSRR = (1U << 5);

        // Delay
        for (volatile uint32_t i = 0; i < 10000000; i++)
            ;

        GPIOA->BSRR = (1U << 21);

        // Delay
        for (volatile uint32_t i = 0; i < 10000000; i++)
            ;
    }
}