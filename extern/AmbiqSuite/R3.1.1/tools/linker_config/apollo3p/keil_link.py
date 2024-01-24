from string import Template
import textwrap
import re


def fix_startup_file(config, filename):

    # Get the stack size from the config file. Note that the startup file
    # measures in words, not bytes.
    stack_size = config['STACK']['size']

    # Read in every line of the given file
    input_lines = None
    with open(filename) as f:
        input_lines = [line for line in f]

    # Perform a search-and-replace with the new stack size.
    def fix_line(line):
        return re.sub(r'(Stack   EQU     )0x[0-9a-fA-F]*',
                      r'\g<1>' + '0x{:08X}'.format(stack_size),
                      line)

    output_lines = map(fix_line, input_lines)
    output_text = ''.join(output_lines)

    # Write the new file back to the input
    # Specify the newline parameter to output with Unix line endings.
    with open(filename, 'w', newline='\n') as f:
        f.write(output_text)


def generate_link_script(config):

    mapping = dict()
    mapping['ro_base'] = format_number(config['ROMEM']['start'])
    mapping['ro_size'] = format_number(config['ROMEM']['size'])
    mapping['rw_base'] = format_number(config['RWMEM']['start'])
    mapping['rw_size'] = format_number(config['RWMEM']['size'])
    mapping['stack_base'] = format_number(config['STACK']['start'])
    mapping['stack_size'] = format_number(config['STACK']['size'])
    mapping['tcm_base'] = format_number(config['TCM']['start'])
    mapping['tcm_size'] = format_number(config['TCM']['size'])
    mapping['additional_sections'] = generate_sections(config)

    link_script = link_script_template.substitute(**mapping)
    debug_file = debug_file_template.substitute(**mapping)

    return (link_script, debug_file)


def generate_sections(config):
    # If there aren't any custom sections in the config file, we don't need to
    # add anything to the linker scripts.
    if 'custom_sections' not in config:
        return ''
    elif not config['custom_sections']:
        return ''

    L = []
    for mem_section in config['custom_sections']:
        D = dict()
        D['name'] = mem_section['blockname']
        D['start'] = format_number(mem_section['start'])
        D['length'] = format_number(mem_section['length'])
        D['sections'] = '\n'.join('    * ({})'.format(x) for x in mem_section['sections'])

        S = extra_section_template.substitute(**D)
        L.append(textwrap.indent(S, 4 * ' '))

    return '\n' + '\n'.join(L)


def format_number(n):
    return '0x{:08X}'.format(n)


link_script_template = Template('''\
;******************************************************************************
;
; Scatter file for Keil linker configuration.
;
;******************************************************************************
LR_1 ${ro_base}
{
    ROMEM ${ro_base} ${ro_size}
    {
        *.o (RESET, +First)
        * (+RO)
    }

    RWMEM ${rw_base} ${rw_size}
    {
        * (+RW, +ZI)
    }

    TCM ${tcm_base} ${tcm_size}
    {
        * (.tcm)
    }

    STACKMEM ${stack_base} ${stack_size}
    {
        startup_keil.o (STACK)
    }
}
''')

extra_section_template = Template('''\
${name} ${start} ${length}
{
${sections}
}
''')

debug_file_template = Template('''\
/*----------------------------------------------------------------------------
 * Name:    Dbg_RAM.ini
 * Purpose: RAM Debug Initialization File
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2008-2013 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
  TraceSetup()  Turn on ITM clocks, etc.
 *----------------------------------------------------------------------------*/
FUNC void TraceSetup (void)
{
    // turn on the ITM/TPIU clock
    //_WDWORD(0x40020250, 0x00000201);    // TPIU clock enabled at 3MHz
}

/*----------------------------------------------------------------------------
  Setup()  configure PC & SP for RAM Debug
 *----------------------------------------------------------------------------*/
FUNC void Setup (void) {
    SP = _RDWORD(${ro_base}+0x0);        // Setup Stack Pointer
    PC = _RDWORD(${ro_base}+0x4);        // Setup Program Counter
    _WDWORD(0xE000ED08, ${ro_base}+0x0); // Setup Vector Table Offset Register (done in system file)
}

/*----------------------------------------------------------------------------
  OnResetExec() executed after reset via uVision's 'Reset'-button
 *----------------------------------------------------------------------------*/
FUNC void OnResetExec (void)
{
}

LOAD %L INCREMENTAL                     // load the application
Setup();                                // Setup for Running

BS main
g

/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
''')

extra_section_template = Template('''\
${name} ${start} ${length}
{
${sections}
}
''')
