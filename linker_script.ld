ENTRY(reset_handler)

MEMORY 
{
    FLASH(rx): ORIGIN = 0x08000000, LENGTH = 512K
    SRAM(rwx): ORIGIN = 0x20000000, LENGTH = 96K
}

/* Define stack and heap sizes */
__max_heap_size = 0x400;
__max_stack_size = 0x200;

SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))   
        . = ALIGN(4);
    } > FLASH

    .text : 
    {
        . = ALIGN(4);
        *(.text)           
        *(.rodata)         /* Typically include rodata with text */
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    _sidata = LOADADDR(.data);  /* load address for .data initialization */

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        . = ALIGN(4);
        _edata = .;
    } >SRAM AT> FLASH

    /*

    >SRAM AT> FLASH

    This indicates that the section will execute in SRAM. The linker places the section at the
    specified address in SRAM for the program to access and use at runtime.

    The AT> part specifies the load address of the section, which is in the flash memory in this 
    case. The section's initial data is stored in flash and must be copied to SRAM during the
    microcontroller's startup routine

    */

    .bss :
    {
        . = ALIGN(4);
        _bss_start = .;    
        *(.bss)
        *(COMMON)
        . = ALIGN(4);
        _bss_end = .;     
    } > SRAM

    /* Add stack and heap sections */
    .heap :
    {
        . = ALIGN(8);
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        . = . + __max_heap_size;
        . = ALIGN(8);
    } > SRAM

    .stack :
    {
        . = ALIGN(8);
        . = . + __max_stack_size;
        . = ALIGN(8);
    } > SRAM
}