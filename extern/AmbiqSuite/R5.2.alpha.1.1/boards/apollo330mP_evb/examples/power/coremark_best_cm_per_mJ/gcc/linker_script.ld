/******************************************************************************
 *
 * linker_script.ld - Linker script for applications using startup_gnu.c
 *
 *****************************************************************************/
ENTRY(Reset_Handler)

MEMORY
{
    MCU_MRAM     (rx)  : ORIGIN = 0x00410000, LENGTH = 2031616
    MCU_TCM      (rwx) : ORIGIN = 0x20000000, LENGTH = 122880
    SHARED_SRAM  (rwx) : ORIGIN = 0x2001E000, LENGTH = 256
    STACK        (rw)  : ORIGIN = 0x2001E200, LENGTH = 7680
    HEAP         (rw)  : ORIGIN = 0x2001E100, LENGTH = 256
}

SECTIONS
{
    .text :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        KEEP(*(.patch))
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } > MCU_MRAM

    /* .ARM.extab and .ARM.exidx are only for C++ exceptions.
    For details, please refer to
    https://developer.arm.com/documentation/ka001486/latest/
    https://blog.thea.codes/the-most-thoroughly-commented-linker-script/
    https://answers.launchpad.net/gcc-arm-embedded/+question/271294
    */
    /* .ARM.extab sections containing exception unwinding information */
    .ARM.extab : {
        . = ALIGN(4);
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > MCU_MRAM

    /* .ARM.exidx sections containing index entries for stack unwinding */
    .ARM.exidx : {
        . = ALIGN(4);
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > MCU_MRAM

    .dtcm_text :
    {
        . = ALIGN(4);
        _s_dtcm_text = .;
        *(.dtcm_text)
        *(.dtcm_text*)
        . = ALIGN(4);
        _e_dtcm_text = .;
    } > MCU_TCM AT>MCU_MRAM

    /* used by startup to initialize the functions located in TCM */
    _init_dtcm_text = LOADADDR(.dtcm_text);

    /* User stack section initialized by startup code. */
    .stack (NOLOAD):
    {
        . = ALIGN(8);
        *(.stack)
        *(.stack*)
        . = ALIGN(8);
    } > STACK

    .heap (NOLOAD):
    {
        __heap_start__ = .;
        end = __heap_start__;
        _end = end;
        __end = end;
        KEEP(*(.heap))
        __heap_end__ = .;
        __HeapLimit = __heap_end__;
    } > HEAP

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > MCU_TCM AT>MCU_MRAM

    /* used by startup to initialize data */
    _init_data = LOADADDR(.data);

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > MCU_TCM

    .shared (NOLOAD):
    {
        . = ALIGN(4);
        KEEP(*(.resource_table))
        KEEP(*(.shared))
        . = ALIGN(4);
    } > SHARED_SRAM AT>MCU_MRAM
    .ARM.attributes 0 : { *(.ARM.attributes) }
}