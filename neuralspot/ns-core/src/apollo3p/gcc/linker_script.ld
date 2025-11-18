/******************************************************************************
 *
 * linker_script.ld - Linker script for applications using startup_gnu.c
 *
 *****************************************************************************/
ENTRY(Reset_Handler)
 
MEMORY
{
    ROMEM   (rx)  : ORIGIN = 0x0000C000, LENGTH = 2048000
    RWMEM   (rwx) : ORIGIN = 0x10015400, LENGTH = 711680
    TCM     (rwx) : ORIGIN = 0x10000000, LENGTH = 65536
    STACKMEM(rwx) : ORIGIN = 0x10010000, LENGTH = 21504
}
 
SECTIONS
{
    /* deterministic ordering overall (pair with -Wl,--sort-section=name) */
    .text :
    {
    . = ALIGN(4);
    KEEP(*(.isr_vector .isr_vector.*))
    KEEP(*(.patch .patch.*))
    *(.text)
    *(SORT_BY_NAME(.text.*) .gnu.linkonce.t.*)
    *(.rodata)
    *(SORT_BY_NAME(.rodata.*) .gnu.linkonce.r.*)
    . = ALIGN(4);
    _etext = .;
    } > ROMEM

    /* FIX: extab pattern typo */
    .ARM.extab :
    {
    KEEP(*(.ARM.extab* .gnu.linkonce.armextab.*))
    } > ROMEM

    /* Keep exidx bounds inside the section */
    .ARM.exidx :
    {
    __exidx_start = .;
    KEEP(*(.ARM.exidx* .gnu.linkonce.armexidx.*))
    __exidx_end = .;
    } > ROMEM

 
    .preinit_array :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        /* rarely used, but deterministic if present */
        KEEP(*(SORT_BY_INIT_PRIORITY(.preinit_array.*)))
        KEEP(*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } > ROMEM
 
    .init_array :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
        KEEP(*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
    } > ROMEM
 
    .fini_array :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*)))
        KEEP(*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } > ROMEM
 
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(SORT_BY_NAME(.data.*))
        . = ALIGN(4);
        _edata = .;
    } > RWMEM AT>ROMEM
 
    /* used by startup to initialize data */
    _init_data = LOADADDR(.data);
 
    .tcm :
    {
        . = ALIGN(4);
        _stcm = .;
        *(.tcm)
        *(SORT_BY_NAME(.tcm.*))
        . = ALIGN(4);
        _etcm = .;
    } > TCM AT>ROMEM
 
    /* used by startup to initialize tcm */
    _init_tcm = LOADADDR(.tcm);
 
    /* Shared stack/heap region: stack first, heap after; top = end of .heap */
    .stack (NOLOAD) :
    {
    . = ALIGN(8);
    KEEP(*(.stack .stack.*))   /* g_pui32Stack lives here */
    . = ALIGN(8);
    } > STACKMEM

    .heap (NOLOAD) :
    {
    __heap_start__ = .;
    KEEP(*(.heap .heap.*))     /* g_pui32Heap lives here */
    __heap_end__ = .;
    __HeapLimit = __heap_end__;
    . = ALIGN(8);
    } > STACKMEM
    /* Traditional heap symbols expected by newlib/nosys sbrk */
    __heap_start__ = ADDR(.heap);
    __heap_end__   = ADDR(.heap) + SIZEOF(.heap);  /* you already had this inside .heap; ok to keep both */

    __HeapLimit = __heap_end__;   /* sbrk upper bound used by some ports */

    /* aliases used by various C libraries / ports */
    PROVIDE(end   = __heap_start__);   /* matches your error: wants 'end' */
    PROVIDE(_end  = __heap_start__);
    PROVIDE(__end = __heap_start__);

    /* Derived symbols for convenience / diagnostics */
    __StackBase = ORIGIN(STACKMEM);
    /* SharedTop == end_of_stack + sizeof(heap) == end_of .heap */
    __SharedTop = ORIGIN(STACKMEM) + SIZEOF(.stack) + SIZEOF(.heap);
    PROVIDE(__StackTop = __SharedTop);
    PROVIDE(_estack    = __SharedTop);

    /* Sanity: both sections must FIT inside STACKMEM */
    ASSERT(SIZEOF(.stack) + SIZEOF(.heap) <= LENGTH(STACKMEM), "Heap/Stack exceed STACKMEM")


    /* Absolute stack symbols for startup (down-growing) */
    __StackLimit = ORIGIN(STACKMEM);
    __StackTop   = ORIGIN(STACKMEM) + LENGTH(STACKMEM);
    PROVIDE(_estack = __StackTop);
 
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(SORT_BY_NAME(.bss.*))
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RWMEM
 
    .ARM.attributes 0 : { *(.ARM.attributes) }
 
    /* Safety checks (fail link if violated) */
    /* .stack then .heap; ensure they are contiguous (no gap) */
    ASSERT(ADDR(.heap) == ADDR(.stack) + SIZEOF(.stack), "STACKMEM: .heap must immediately follow .stack")

    /* Both sections together must fit in STACKMEM (after ALIGNs) */
    ASSERT(SIZEOF(.stack) + SIZEOF(.heap) <= LENGTH(STACKMEM), "STACKMEM: stack+heap exceed region")
}