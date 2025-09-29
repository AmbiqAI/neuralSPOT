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
        . = ALIGN(4);
        _etext = .;
    } > MCU_MRAM
    
    .itcm_text :
    {
        . = ALIGN(4);
        _s_itcm_text = .;
        *(.itcm_text)
        *(.itcm_text*)
        /* KEEP(add*.o       (.text .text.* .rodata .rodata.*)) */
        KEEP(arm*.o       (.text .text.* .rodata .rodata.*))
        KEEP(fft*.o       (.text .text.* .rodata .rodata.*))
        KEEP(conv*.o      (.text .text.* .rodata .rodata.*))
        KEEP(connected*.o (.text .text.* .rodata .rodata.*))
        KEEP(quant*.o     (.text .text.* .rodata .rodata.*))
        KEEP(add*.o       (.text .text.* .rodata .rodata.*))
        KEEP(softmax*.o   (.text .text.* .rodata .rodata.*))
        KEEP(micro*.o     (.text .text.* .rodata .rodata.*))
        KEEP(poolin*.o    (.text .text.* .rodata .rodata.*))
        KEEP(call_on*.o  (.text .text.* .rodata .rodata.*))
        KEEP(var_hand*.o (.text .text.* .rodata .rodata.*))
        KEEP(strided*.o   (.text .text.* .rodata .rodata.*))
        KEEP(concat*.o    (.text .text.* .rodata .rodata.*))
        KEEP(memory_help*.o (.text .text.* .rodata .rodata.*))
        KEEP(read_var*.o  (.text .text.* .rodata .rodata.*))
        KEEP(assign_var*.o (.text .text.* .rodata .rodata.*))
        KEEP(tensor*.o    (.text .text.* .rodata .rodata.*))
        KEEP(linear*.o    (.text .text.* .rodata .rodata.*))
        KEEP(greedy*.o    (.text .text.* .rodata .rodata.*))
        KEEP(flatbuf*.o   (.text .text.* .rodata .rodata.*))
        KEEP(single_arena*.o (.text .text.* .rodata .rodata.*))
        KEEP(kernel_*.o   (.text .text.* .rodata .rodata.*))

        . = ALIGN(4);
        _e_itcm_text = .;
    } > MCU_ITCM AT>MCU_MRAM

    .text :
    {
        . = ALIGN(4);
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

    /* .itcm_text :
    {
        . = ALIGN(4);
        _s_itcm_text = .;
        *(.itcm_text)
        *(.itcm_text*)
        . = ALIGN(4);
        _e_itcm_text = .;
    } > MCU_ITCM AT>MCU_MRAM */

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
    
    .ARM.attributes 0 : { *(.ARM.attributes) }
}