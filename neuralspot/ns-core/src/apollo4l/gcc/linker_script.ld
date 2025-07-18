/******************************************************************************
 *
 * linker_script.ld - Linker script for applications using startup_gnu.c
 *
 *****************************************************************************/
ENTRY(Reset_Handler)

MEMORY
{
    MCU_MRAM     (rx)  : ORIGIN = 0x00018000, LENGTH = 1998848
    MCU_TCM      (rwx) : ORIGIN = 0x10000000, LENGTH = 393216
    SHARED_SRAM  (rwx) : ORIGIN = 0x10060000, LENGTH = 1048576
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

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > MCU_MRAM

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > MCU_MRAM
    __exidx_end = .;

    .preinit_array :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } > MCU_MRAM

    .init_array :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
    } > MCU_MRAM

    .fini_array :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } > MCU_MRAM

    /* User stack section initialized by startup code. */
    .stack (NOLOAD):
    {
        . = ALIGN(8);
        *(.stack)
        *(.stack*)
        . = ALIGN(8);
    } > MCU_TCM

    .heap (NOLOAD): {
        __heap_start__ = .;
        end = __heap_start__;
        _end = end;
        __end = end;
        KEEP(*(.heap))
        __heap_end__ = .;
        __HeapLimit = __heap_end__;
    } > MCU_TCM

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

    /*--------------------------------------------------------------------------
     * Uninitialized data in Shared SRAM
     * Place with __attribute__((section(".sram_bss")))
     *--------------------------------------------------------------------------*/
    .sram_bss (NOLOAD):
    {
        . = ALIGN(4);
        /* Marks start/end of SRAM BSS */
        _ssram_bss = .;
        *(.sram_bss)
        *(.sram_bss*)
        . = ALIGN(4);
        _esram_bss = .;
    } > SHARED_SRAM
    
   .shared :
    {
        . = ALIGN(4);
        KEEP(*(.resource_table))
        . = ALIGN(4);

        _ssdata = .;
        KEEP(*(.shared))
        . = ALIGN(4);
        _sedata = .;
    } > SHARED_SRAM AT>MCU_MRAM
    _init_data_sram = LOADADDR(.shared);
}
