ENTRY(reset_handler)

MEMORY 
{
    FLASH(rx): ORIGIN = 0x08000000, LENGTH = 512K
    SRAM(xrw): ORIGIN = 0x20000000, LENGTH = 96K
}

/* Define stack and heap sizes */
__max_heap_size = 0x400;  /* 1KB heap */
__max_stack_size = 0x200; /* 512B stack */

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
        *(.text*)          /* Add .text* to catch all text sections */
        *(.rodata)         
        *(.rodata*)        /* Add .rodata* to catch all rodata sections */
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    _sidata = LOADADDR(.data);

    .data :
    {
        . = ALIGN(4);
        _sdata = .;        /* Start of .data section */
        *(.data)           
        *(.data*)          /* Add .data* to catch all data sections */
        . = ALIGN(4);
        _edata = .;        /* End of .data section */
    } > SRAM AT > FLASH

    .bss :
    {
        . = ALIGN(4);
        _bss_start = .;    /* Fixed symbol name and added underscore */
        *(.bss)
        *(.bss*)           /* Add .bss* to catch all bss sections */
        *(COMMON)
        . = ALIGN(4);
        _bss_end = .;      /* Fixed symbol name and added underscore */
    } > SRAM

    /* Stack section - place at start of SRAM */
    .stack (NOLOAD) :
    {
        . = ALIGN(8);
        _stack_start = .;
        . = . + __max_stack_size;
        . = ALIGN(8);
        _stack_end = .;
    } > SRAM

    /* Heap section - place after BSS */
    .heap (NOLOAD) :
    {
        . = ALIGN(8);
        _heap_start = .;
        . = . + __max_heap_size;
        . = ALIGN(8);
        _heap_end = .;
        end = .;           /* For C standard library */
    } > SRAM

    /* Discard unused sections */
    /DISCARD/ :
    {
        *(.comment)
        *(.ARM.attributes)
        *(.note.GNU-stack)
    }
}

/* Stack top for initial SP */
_estack = ORIGIN(SRAM) + LENGTH(SRAM);