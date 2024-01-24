from string import Template
import re


def fix_startup_file(config, filename):

    # Get the stack size from the config file. Note that the startup file
    # measures in words, not bytes.
    stack_size = config['STACK']['size'] // 4

    # Read in every line of the given file
    input_lines = None
    with open(filename) as f:
        input_lines = [line for line in f]

    # Perform a search-and-replace with the new stack size.
    def fix_line(line):
        return re.sub(r'(g_pui32Stack\[)([x0-9a-fA-F]*)',
                      r'\g<1>' + str(stack_size),
                      line)

    output_lines = map(fix_line, input_lines)
    output_text = ''.join(output_lines)

    # Write the new file back to the input
    # Specify the newline parameter to output with Unix line endings.
    with open(filename, 'w', newline='\n') as f:
        f.write(output_text)


def generate_link_script(config):
    D = dict()
    D['ro_base'] = format_hex(config['ROMEM']['start'])
    D['ro_size'] = config['ROMEM']['size']

    D['rw_base'] = format_hex(config['RWMEM']['start'])
    D['rw_size'] = config['RWMEM']['size']

    D['stack_base'] = format_hex(config['STACK']['start'])
    D['stack_size'] = config['STACK']['size']

    D['tcm_base'] = format_hex(config['TCM']['start'])
    D['tcm_size'] = config['TCM']['size']

    D['section_definitions'] = format_section_definitions(config)
    D['additional_sections'] = format_sections(config)

    return link_script_template.substitute(**D)


def format_section_definitions(config):

    def format_section_definition(section):
        mapping = {
            'name': section,
            'permissions': config[section]['perm'],
            'base': format_hex(config[section]['start']),
            'length': format_hex(config[section]['size']),
        }

        return section_definition_template.substitute(**mapping)

    extra_sections = get_extra_sections(config)
    return ''.join(map(format_section_definition, extra_sections))


def format_sections(config):

    def format_section(section):
        mapping = {
            'name': section,
        }

        return section_template.substitute(**mapping)

    extra_sections = get_extra_sections(config)
    return ''.join(map(format_section, extra_sections))


def get_extra_sections(config):
    def section_not_required(x):
        return x not in ['ROMEM', 'RWMEM', 'STACK', 'TCM']

    return list(filter(section_not_required, config))


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
{
    ROMEM (rx) : ORIGIN = ${ro_base}, LENGTH = ${ro_size}
    RWMEM (rwx) : ORIGIN = ${rw_base}, LENGTH = ${rw_size}
    TCM (rwx) : ORIGIN = ${tcm_base}, LENGTH = ${tcm_size}
    STACKMEM (rwx) : ORIGIN = ${stack_base}, LENGTH = ${stack_size}
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
    } > ROMEM

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
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
        *(.tcm*)
        . = ALIGN(4);
        _etcm = .;
    } > TCM AT>ROMEM

    /* used by startup to initialize tcm */
    _init_tcm = LOADADDR(.tcm);

    /* User stack section initialized by startup code. */
    .stack (NOLOAD):
    {
        . = ALIGN(8);
        *(.stack)
        *(.stack*)
        . = ALIGN(8);
    } > STACKMEM

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RWMEM

    .ARM.attributes 0 : { *(.ARM.attributes) }
}\
''')

section_definition_template = Template('''\
    ${name} (${permissions}) : ORIGIN = ${base}, LENGTH = ${length}
''')

section_template = Template('''\
    ${name} :
    {
        *(${name})
    } > ${name}

''')
