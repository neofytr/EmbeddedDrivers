#include <stdint.h>

#define SRAM_START 0x20000000U
#define SRAM_SIZE (96 * 1024) // 96 KBs
#define SRAM_END ((SRAM_START) + (SRAM_SIZE))
#define STACK_START (SRAM_END)

void reset_handler(void);
void default_handler(void);

extern uint32_t _etext, _sdata, _edata, __bss_start__, __bss_end__, _sidata;
extern int main(void);

#define VECTOR_TABLE_LEN 84

// System handlers
void NMI_handler(void) __attribute__((weak, alias("default_handler")));
void HardFault_handler(void) __attribute__((weak, alias("default_handler")));
void MemManage_handler(void) __attribute__((weak, alias("default_handler")));
void BusFault_handler(void) __attribute__((weak, alias("default_handler")));
void UsageFault_handler(void) __attribute__((weak, alias("default_handler")));
void SVCall_handler(void) __attribute__((weak, alias("default_handler")));
void DebugMonitor_handler(void) __attribute__((weak, alias("default_handler")));
void PendSV_handler(void) __attribute__((weak, alias("default_handler")));
void SysTick_handler(void) __attribute__((weak, alias("default_handler")));

// Peripheral Interrupts
void WWDG_handler(void) __attribute__((weak, alias("default_handler")));
void PVD_EXTI16_handler(void) __attribute__((weak, alias("default_handler")));
void TAMP_STAMP_EXTI21_handler(void) __attribute__((weak, alias("default_handler")));
void RTC_WKUP_EXTI22_handler(void) __attribute__((weak, alias("default_handler")));
void FLASH_handler(void) __attribute__((weak, alias("default_handler")));
void RCC_handler(void) __attribute__((weak, alias("default_handler")));
void EXTI0_handler(void) __attribute__((weak, alias("default_handler")));
void EXTI1_handler(void) __attribute__((weak, alias("default_handler")));
void EXTI2_handler(void) __attribute__((weak, alias("default_handler")));
void EXTI3_handler(void) __attribute__((weak, alias("default_handler")));
void EXTI4_handler(void) __attribute__((weak, alias("default_handler")));
void DMA1_Stream0_handler(void) __attribute__((weak, alias("default_handler")));
void DMA1_Stream1_handler(void) __attribute__((weak, alias("default_handler")));
void DMA1_Stream2_handler(void) __attribute__((weak, alias("default_handler")));
void DMA1_Stream3_handler(void) __attribute__((weak, alias("default_handler")));
void DMA1_Stream4_handler(void) __attribute__((weak, alias("default_handler")));
void DMA1_Stream5_handler(void) __attribute__((weak, alias("default_handler")));
void DMA1_Stream6_handler(void) __attribute__((weak, alias("default_handler")));
void ADC_handler(void) __attribute__((weak, alias("default_handler")));
void EXTI9_5_handler(void) __attribute__((weak, alias("default_handler")));
void TIM1_BRK_TIM9_handler(void) __attribute__((weak, alias("default_handler")));
void TIM1_UP_TIM10_handler(void) __attribute__((weak, alias("default_handler")));
void TIM1_TRG_COM_TIM11_handler(void) __attribute__((weak, alias("default_handler")));
void TIM1_CC_handler(void) __attribute__((weak, alias("default_handler")));
void TIM2_handler(void) __attribute__((weak, alias("default_handler")));
void TIM3_handler(void) __attribute__((weak, alias("default_handler")));
void TIM4_handler(void) __attribute__((weak, alias("default_handler")));
void I2C1_EV_handler(void) __attribute__((weak, alias("default_handler")));
void I2C1_ER_handler(void) __attribute__((weak, alias("default_handler")));
void I2C2_EV_handler(void) __attribute__((weak, alias("default_handler")));
void I2C2_ER_handler(void) __attribute__((weak, alias("default_handler")));
void SPI1_handler(void) __attribute__((weak, alias("default_handler")));
void SPI2_handler(void) __attribute__((weak, alias("default_handler")));
void USART1_handler(void) __attribute__((weak, alias("default_handler")));
void USART2_handler(void) __attribute__((weak, alias("default_handler")));
void EXTI15_10_handler(void) __attribute__((weak, alias("default_handler")));
void RTC_Alarm_EXTI17_handler(void) __attribute__((weak, alias("default_handler")));
void OTG_FS_WKUP_EXTI18_handler(void) __attribute__((weak, alias("default_handler")));
void DMA1_Stream7_handler(void) __attribute__((weak, alias("default_handler")));
void SDIO_handler(void) __attribute__((weak, alias("default_handler")));
void TIM5_handler(void) __attribute__((weak, alias("default_handler")));
void SPI3_handler(void) __attribute__((weak, alias("default_handler")));
void DMA2_Stream0_handler(void) __attribute__((weak, alias("default_handler")));
void DMA2_Stream1_handler(void) __attribute__((weak, alias("default_handler")));
void DMA2_Stream2_handler(void) __attribute__((weak, alias("default_handler")));
void DMA2_Stream3_handler(void) __attribute__((weak, alias("default_handler")));
void DMA2_Stream4_handler(void) __attribute__((weak, alias("default_handler")));
void OTG_FS_handler(void) __attribute__((weak, alias("default_handler")));
void DMA2_Stream5_handler(void) __attribute__((weak, alias("default_handler")));
void DMA2_Stream6_handler(void) __attribute__((weak, alias("default_handler")));
void DMA2_Stream7_handler(void) __attribute__((weak, alias("default_handler")));
void USART6_handler(void) __attribute__((weak, alias("default_handler")));
void I2C3_EV_handler(void) __attribute__((weak, alias("default_handler")));
void I2C3_ER_handler(void) __attribute__((weak, alias("default_handler")));
void FPU_handler(void) __attribute__((weak, alias("default_handler")));
void SPI4_handler(void) __attribute__((weak, alias("default_handler")));

volatile uint32_t vectors[VECTOR_TABLE_LEN] __attribute__((section(".isr_vector"))) = {
    (uint32_t)STACK_START,                 // Initial stack pointer; this value is put into the stack pointer after reset
    (uint32_t)&reset_handler,              // Reset handler
    (uint32_t)&NMI_handler,                // NMI handler
    (uint32_t)&HardFault_handler,          // HardFault handler
    (uint32_t)&MemManage_handler,          // MemManage handler
    (uint32_t)&BusFault_handler,           // BusFault handler
    (uint32_t)&UsageFault_handler,         // UsageFault handler
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    (uint32_t)&SVCall_handler,             // SVCall handler
    (uint32_t)&DebugMonitor_handler,       // Debug Monitor handler
    0,                                     // Reserved
    (uint32_t)&PendSV_handler,             // PendSV handler
    (uint32_t)&SysTick_handler,            // SysTick handler
    (uint32_t)&WWDG_handler,               // WWDG handler
    (uint32_t)&PVD_EXTI16_handler,         // PVD/EXTI16 handler
    (uint32_t)&TAMP_STAMP_EXTI21_handler,  // TAMP_STAMP/EXTI21 handler
    (uint32_t)&RTC_WKUP_EXTI22_handler,    // RTC_WKUP/EXTI22 handler
    (uint32_t)&FLASH_handler,              // FLASH handler
    (uint32_t)&RCC_handler,                // RCC handler
    (uint32_t)&EXTI0_handler,              // EXTI0 handler
    (uint32_t)&EXTI1_handler,              // EXTI1 handler
    (uint32_t)&EXTI2_handler,              // EXTI2 handler
    (uint32_t)&EXTI3_handler,              // EXTI3 handler
    (uint32_t)&EXTI4_handler,              // EXTI4 handler
    (uint32_t)&DMA1_Stream0_handler,       // DMA1_Stream0 handler
    (uint32_t)&DMA1_Stream1_handler,       // DMA1_Stream1 handler
    (uint32_t)&DMA1_Stream2_handler,       // DMA1_Stream2 handler
    (uint32_t)&DMA1_Stream3_handler,       // DMA1_Stream3 handler
    (uint32_t)&DMA1_Stream4_handler,       // DMA1_Stream4 handler
    (uint32_t)&DMA1_Stream5_handler,       // DMA1_Stream5 handler
    (uint32_t)&DMA1_Stream6_handler,       // DMA1_Stream6 handler
    (uint32_t)&ADC_handler,                // ADC handler
    (uint32_t)&EXTI9_5_handler,            // EXTI9_5 handler
    (uint32_t)&TIM1_BRK_TIM9_handler,      // TIM1_BRK_TIM9 handler
    (uint32_t)&TIM1_UP_TIM10_handler,      // TIM1_UP_TIM10 handler
    (uint32_t)&TIM1_TRG_COM_TIM11_handler, // TIM1_TRG_COM_TIM11 handler
    (uint32_t)&TIM1_CC_handler,            // TIM1_CC handler
    (uint32_t)&TIM2_handler,               // TIM2 handler
    (uint32_t)&TIM3_handler,               // TIM3 handler
    (uint32_t)&TIM4_handler,               // TIM4 handler
    (uint32_t)&I2C1_EV_handler,            // I2C1_EV handler
    (uint32_t)&I2C1_ER_handler,            // I2C1_ER handler
    (uint32_t)&I2C2_EV_handler,            // I2C2_EV handler
    (uint32_t)&I2C2_ER_handler,            // I2C2_ER handler
    (uint32_t)&SPI1_handler,               // SPI1 handler
    (uint32_t)&SPI2_handler,               // SPI2 handler
    (uint32_t)&USART1_handler,             // USART1 handler
    (uint32_t)&USART2_handler,             // USART2 handler
    (uint32_t)&EXTI15_10_handler,          // EXTI15_10 handler
    (uint32_t)&RTC_Alarm_EXTI17_handler,   // RTC_Alarm/EXTI17 handler
    (uint32_t)&OTG_FS_WKUP_EXTI18_handler, // OTG_FS_WKUP/EXTI18 handler
    (uint32_t)&DMA1_Stream7_handler,       // DMA1_Stream7 handler
    (uint32_t)&SDIO_handler,               // SDIO handler
    (uint32_t)&TIM5_handler,               // TIM5 handler
    (uint32_t)&SPI3_handler,               // SPI3 handler
    (uint32_t)&DMA2_Stream0_handler,       // DMA2_Stream0 handler
    (uint32_t)&DMA2_Stream1_handler,       // DMA2_Stream1 handler
    (uint32_t)&DMA2_Stream2_handler,       // DMA2_Stream2 handler
    (uint32_t)&DMA2_Stream3_handler,       // DMA2_Stream3 handler
    (uint32_t)&DMA2_Stream4_handler,       // DMA2_Stream4 handler
    (uint32_t)&OTG_FS_handler,             // OTG_FS handler
    (uint32_t)&DMA2_Stream5_handler,       // DMA2_Stream5 handler
    (uint32_t)&DMA2_Stream6_handler,       // DMA2_Stream6 handler
    (uint32_t)&DMA2_Stream7_handler,       // DMA2_Stream7 handler
    (uint32_t)&USART6_handler,             // USART6 handler
    (uint32_t)&I2C3_EV_handler,            // I2C3_EV handler
    (uint32_t)&I2C3_ER_handler,            // I2C3_ER handler
    (uint32_t)&FPU_handler,                // FPU handler
    (uint32_t)&SPI4_handler,               // SPI4 handler
};

__attribute__((used)) void reset_handler(void)
{
    // copy .data section to SRAM
    uint32_t size = (uint32_t)&_edata - (uint32_t)&_sdata; // using _edata or _sdata directly in the C program will
                                                           // automatically dereference these symbols

    uint8_t *dst_ptr = (uint8_t *)&_sdata;
    uint8_t *src_ptr = (uint8_t *)&_sidata;

    for (uint32_t counter = 0; counter < size; counter++)
    {
        *dst_ptr++ = *src_ptr++;
    }

    // initialize the .bss section to zero in SRAM

    size = (uint32_t)&__bss_end__ - (uint32_t)&__bss_start__;
    dst_ptr = (uint8_t *)&__bss_start__;

    for (uint32_t counter = 0; counter < size; counter++)
    {
        *dst_ptr++ = 0;
    }

    // since we are not linking the C standard library with the project, there is no
    // need to call the C lib init constructor function __libc_init_array()
    // call main()
    main();

    while (1)
        ;
}

__attribute__((used)) void default_handler(void)
{
    while (1)
        ;
}
