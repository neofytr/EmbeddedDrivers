#include <stdint.h>
#include "STM32F401.h"
#include <stdlib.h>

#define GPIOA_EN (1U << 0)
#define LED_PIN (1U << 5)
#define LED_MODE (1U << 10)

int main(void)
{
    RCC->AHB1ENR |= GPIOA_EN;

    GPIOA->MODER &= ~(3U << 10);
    GPIOA->MODER |= (1U << 10);

    GPIOA->OTYPER &= ~(1U << 5);

    GPIOA->OSPEEDR &= ~(3U << 10);

    GPIOA->PUPDR &= ~(3U << 10);

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