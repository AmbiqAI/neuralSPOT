# *****************************************************************************
#
#    gcc_link.py
#
#    @brief Generate the gcc link control file.
#
# *****************************************************************************

# *****************************************************************************
#
#    Copyright (c) 2025, Ambiq Micro, Inc.
#    All rights reserved.
#
#    Redistribution and use in source and binary forms, with or without
#    modification, are permitted provided that the following conditions are met:
#
#    1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
#    2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
#    3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
#    Third party software included in this distribution is subject to the
#    additional license terms as defined in the /docs/licenses directory.
#
#    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#    POSSIBILITY OF SUCH DAMAGE.
#
#  This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
#
# *****************************************************************************

from string import Template

def replace_device_info(template_text, config):
    D = dict()
    template = Template(template_text)

    projName = config['proj_name']
    part = config['part']
    memSections = config['MemorySections']

    D['ro_base'] = format_hex(memSections['MCU_MRAM']['start'])

    if projName == 'apollo5a/b':
        if part == 'nbl':
            D['device'] = 'AMAP54KK-KBR-nbl'
        else:
            D['device'] = 'AP510NFA-CBR'
    elif projName == 'apollo510L':
        if part == 'nbl':
            D['device'] = 'AP510L-nbl'
        else:
            D['device'] = 'AP510L'

    return template.safe_substitute(D)

def generate_link_script(config):
    D = dict()
    projName = config['proj_name']
    memSections = config['MemorySections']

    if projName == 'apollo5a/b':
        itcm_section_script = (
            '\n    .itcm_text :\n'
            '    {\n'
            '        . = ALIGN(4);\n'
            '        _s_itcm_text = .;\n'
            '        *(.itcm_text)\n'
            '        *(.itcm_text*)\n'
            '        . = ALIGN(4);\n'
            '        _e_itcm_text = .;\n'
            '    } > MCU_ITCM AT>MCU_MRAM\n'
            '\n'
            '    /* used by startup to initialize the functions located in TCM */\n'
            '    _init_itcm_text = LOADADDR(.itcm_text);\n'
        )
        itcmBase = format_hex(memSections['MCU_ITCM']['start'])
        itcmLen = memSections['MCU_ITCM']['length']
        D['tcm_memory'] = f'\n    MCU_ITCM     (rwx) : ORIGIN = {itcmBase}, LENGTH = {itcmLen}'
        D['tcm_section'] = itcm_section_script
    elif projName == 'apollo510L':
        dtcm_section_script = (
            '\n    .dtcm_text :\n'
            '    {\n'
            '        . = ALIGN(4);\n'
            '        _s_dtcm_text = .;\n'
            '        *(.dtcm_text)\n'
            '        *(.dtcm_text*)\n'
            '        . = ALIGN(4);\n'
            '        _e_dtcm_text = .;\n'
            '    } > MCU_TCM AT>MCU_MRAM\n'
            '\n'
            '    /* used by startup to initialize the functions located in TCM */\n'
            '    _init_dtcm_text = LOADADDR(.dtcm_text);\n'
        )
        D['tcm_memory'] = ''
        D['tcm_section'] = dtcm_section_script

    D['ro_base'] = format_hex(memSections['MCU_MRAM']['start'])
    D['ro_length'] = memSections['MCU_MRAM']['length']

    D['rw_base'] = format_hex(memSections['MCU_TCM']['start'])
    D['rw_length'] = memSections['MCU_TCM']['length']

    D['shared_base'] = format_hex(memSections['SHARED_SRAM']['start'])
    D['shared_length'] = memSections['SHARED_SRAM']['length']

    D['stack_base'] = format_hex(memSections['STACK']['start'])
    D['stack_length'] = memSections['STACK']['length']

    D['heap_base'] = format_hex(memSections['HEAP']['start'])
    D['heap_length'] = memSections['HEAP']['length']

    D['additional_sections'] = ''

    return link_script_template.substitute(**D)


def format_hex(n):
    return '0x{:08X}'.format(n)

link_script_template = Template('''\
/******************************************************************************
 *
 * linker_script.ld - Linker script for applications using startup_gnu.c
 *
 *****************************************************************************/
ENTRY(Reset_Handler)

MEMORY
{${tcm_memory}
    MCU_MRAM     (rx)  : ORIGIN = ${ro_base}, LENGTH = ${ro_length}
    MCU_TCM      (rwx) : ORIGIN = ${rw_base}, LENGTH = ${rw_length}
    SHARED_SRAM  (rwx) : ORIGIN = ${shared_base}, LENGTH = ${shared_length}
    STACK        (rw)  : ORIGIN = ${stack_base}, LENGTH = ${stack_length}
    HEAP         (rw)  : ORIGIN = ${heap_base}, LENGTH = ${heap_length}
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
${tcm_section}
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
${additional_sections}    .ARM.attributes 0 : { *(.ARM.attributes) }
}\
''')
