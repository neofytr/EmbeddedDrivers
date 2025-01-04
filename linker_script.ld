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
        KEEP(*(.isr_vector))    /* KEEP directive added */
        . = ALIGN(4);
    } > FLASH

    .text : 
    {
        . = ALIGN(4);
        *(.text)           
        *(.text*)          /* Added .text* to include all text sections */
        *(.rodata)         /* Typically include rodata with text */
        *(.rodata*)        /* Added .rodata* to include all rodata sections */
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    _sidata = LOADADDR(.data);  /* Added load address for .data initialization */

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)          /* Added .data* to include all data sections */
        . = ALIGN(4);
        _edata = .;
    } > SRAM AT> FLASH

    .bss :
    {
        . = ALIGN(4);
        _bss_start = .;    /* Changed to consistent naming */
        *(.bss)
        *(.bss*)           /* Added .bss* to include all bss sections */
        *(COMMON)          /* Added COMMON sections */
        . = ALIGN(4);
        _bss_end = .;      /* Changed to consistent naming */
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