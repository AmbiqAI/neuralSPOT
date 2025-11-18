# *****************************************************************************
#
#    iar_link.py
#
#    @brief Generate the IAR link control file.
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


def generate_link_script(config):
    D = dict()
    projName = config['proj_name']
    memSections = config['MemorySections']

    if projName == 'apollo5a/b':
        itcmBase = format_hex(memSections['MCU_ITCM']['start'])
        itcmLen = format_hex(memSections['MCU_ITCM']['length'])
        D['itcm_region'] = f'\ndefine region MCU_ITCM    = mem:[from {itcmBase} to {itcmLen}];'
        D['tcm_place'] = f'\nplace at start of MCU_ITCM {{ section .textrw }};'
    else:
        D['itcm_region'] = ''
        D['tcm_place'] = f'\nplace at start of MCU_TCM {{ section .textrw }};'

    D['ro_base'] = format_hex(memSections['MCU_MRAM']['start'])
    D['ro_end'] = format_hex(memSections['MCU_MRAM']['end'])

    D['rw_base'] = format_hex(memSections['MCU_TCM']['start'])
    D['rw_end'] = format_hex(memSections['MCU_TCM']['end'])

    D['shared_base'] = format_hex(memSections['SHARED_SRAM']['start'])
    D['shared_end'] = format_hex(memSections['SHARED_SRAM']['end'])

    D['stack_base'] = format_hex(memSections['STACK']['start'])
    D['stack_end'] = format_hex(memSections['STACK']['end']) 

    D['heap_base'] = format_hex(memSections['HEAP']['start'])
    D['heap_end'] = format_hex(memSections['HEAP']['end']) 

    D['stack_length'] = memSections['STACK']['length']
    D['heap_length']  = memSections['HEAP']['length']

    D['additional_sections'] = ''

    return link_script_template.substitute(**D)


def format_hex(n):
    return '0x{:08X}'.format(n)


link_script_template = Template('''\
//*****************************************************************************
//
// linker_script.icf
//
// IAR linker Configuration File
//
//*****************************************************************************

//
// Define a memory section that covers the entire 4 GB addressable space of the
// processor. (32-bit can address up to 4GB)
//
define memory mem with size = 4G;

//
// Define regions for the various types of internal memory.
//${itcm_region}
define region MCU_MRAM    = mem:[from ${ro_base} to ${ro_end}];
define region MCU_TCM     = mem:[from ${rw_base} to ${rw_end}];
define region SHARED_SRAM = mem:[from ${shared_base} to ${shared_end}];
define region STACK       = mem:[from ${stack_base} to ${stack_end}];
define region HEAP        = mem:[from ${heap_base} to ${heap_end}];

//
// Define blocks for logical groups of data.
//
define block HEAP with alignment = 0x8, size = ${heap_length}
{
    section .heap
};
define block CSTACK with alignment = 0x8, size = ${stack_length} 
{
    section .stack
};

define block FLASHBASE with fixed order
{
    readonly section .intvec,
    readonly section .patch
};

//
// Set section properties.
//
initialize by copy { readwrite };
initialize by copy { section SHARED_RW };
do not initialize { section .noinit, section .stack , section .heap};
//
// Place code sections in memory regions.
//
place at start of MCU_MRAM { block FLASHBASE };
place in MCU_MRAM { readonly };${tcm_place}
place at end of STACK { block CSTACK};
place in HEAP { block HEAP};
place in MCU_TCM { section .noinit, readwrite};
place at start of SHARED_SRAM { section RESOURCE_TABLE };
place in SHARED_SRAM { section SHARED_RW };${additional_sections}
''')
