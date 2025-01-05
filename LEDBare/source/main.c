#include <stdint.h>
#include <stdbool.h>

#define RCC_BASE (0x40023800UL)
#define GPIOA_BASE (0x40020000UL)

#define RCC_AHB1ENR_OFFSET (0x30UL)
#define RCC_ANB1ENR (RCC_BASE + RCC_AHB1ENR_OFFSET)

#define RCC_AHB1ENR_GPIOA_BITNO (0)

#define PIN5 (5)

#define GPIOA_MODER_OFFSET (0)
#define GPIOA_MODER (GPIOA_BASE + GPIOA_MODER_OFFSET)

#define GPIOA_ODR_OFFSET (0x14UL)
#define GPIOA_ODR (GPIOA_BASE + GPIOA_ODR_OFFSET)

int main()
{
    *((volatile uint32_t *)(RCC_ANB1ENR)) |= (1UL << RCC_AHB1ENR_GPIOA_BITNO);

    *((volatile uint32_t *)(GPIOA_MODER)) |= (1UL << (PIN5 * 2));
    *((volatile uint32_t *)(GPIOA_MODER)) &= (~(1UL << (PIN5 * 2 + 1)));

    while (true)
    {
        *((volatile uint32_t *)(GPIOA_ODR)) ^= (1UL << (PIN5));

        for (volatile int i = 0; i < 100000; i++)
            ;
    }
}