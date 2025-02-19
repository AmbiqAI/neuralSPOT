/******************************************************************************
 *
 * linker_script.ld - Linker script for applications using startup_gnu.c
 *
 *****************************************************************************/
ENTRY(Reset_Handler)

MEMORY
{
    MCU_ITCM     (rwx) : ORIGIN = 0x00000000, LENGTH = 262144
    MCU_MRAM     (rx)  : ORIGIN = 0x00410000, LENGTH = 4128768
    MCU_TCM      (rwx) : ORIGIN = 0x20000000, LENGTH = 507904
    SHARED_SRAM  (rwx) : ORIGIN = 0x20080000, LENGTH = 3145728
    /* STACK        (rw)  : ORIGIN = 0x2007D000, LENGTH = 12288
    HEAP         (rw)  : ORIGIN = 0x2007C000, LENGTH = 4096 */
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
    
    .itcm_text :
    {
        . = ALIGN(4);
        _s_itcm_text = .;
        *(.itcm_text)
        *(.itcm_text*)
        . = ALIGN(4);
        _e_itcm_text = .;
    } > MCU_ITCM AT>MCU_MRAM

    /* used by startup to initialize the functions located in ITCM */
    _init_itcm_text = LOADADDR(.itcm_text);

    /* User stack section initialized by startup code. */
    .stack (NOLOAD):
    {
        . = ALIGN(8);
        *(.stack)
        *(.stack*)
        . = ALIGN(8);
    } > MCU_TCM
    /* } > STACK */

    .heap (NOLOAD): 
    {
        __heap_start__ = .;
        end = __heap_start__;
        _end = end;
        __end = end;
        KEEP(*(.heap))
        __heap_end__ = .;
        __HeapLimit = __heap_end__;
    }  > MCU_TCM
    /* } > HEAP */

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