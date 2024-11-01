/* Entry Point */
ENTRY(Reset_Handler)

/* Memory Configuration */
MEMORY
{
  RAM    (xrw)    : ORIGIN = 0x20000000, LENGTH = 96K
  FLASH    (rx)    : ORIGIN = 0x08000000, LENGTH = 512K
}

/* Stack and Heap Configuration */
__Min_Heap_Size  = 0x200;    /* 512 bytes minimum heap  */
__Min_Stack_Size = 0x400;    /* 1KB minimum stack */

/* Calculate end of RAM address */
__RAM_END = ORIGIN(RAM) + LENGTH(RAM);

/* Define stack addresses */
__stack_limit = __RAM_END - __Min_Stack_Size;
_estack = __RAM_END;

SECTIONS
{
    /* Vector Table */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } >FLASH

    /* Program Code */
    .text :
    {
        . = ALIGN(4);
        *(.text*)
        *(.text)
        *(.glue_7)         /* ARM/Thumb glue code */
        *(.glue_7t)        /* Thumb/ARM glue code */
        *(.eh_frame)
        KEEP (*(.init))
        KEEP (*(.fini))
        . = ALIGN(4);
        _etext = .;        /* End of .text section */
    } >FLASH

    /* Read-only Data */
    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } >FLASH

    /* Initialized Data */
    _sidata = LOADADDR(.data);
    .data : AT ( _sidata )
    {
        . = ALIGN(4);
        _sdata = .;        /* Start of .data section */
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;        /* End of .data section */
    } >RAM

    /* Uninitialized Data */
    .bss :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } >RAM

    /* Heap and Stack */
    ._user_heap_stack :
    {
        . = ALIGN(8);
        . = . + __Min_Heap_Size;
        . = . + __Min_Stack_Size;
        . = ALIGN(8);
    } >RAM

    /* Remove Information from Standard Libraries */
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }

    .ARM.attributes 0 : { *(.ARM.attributes) }
}

/* Heap end pointers */
__heap_base = __bss_end__;
__heap_limit = __stack_limit;