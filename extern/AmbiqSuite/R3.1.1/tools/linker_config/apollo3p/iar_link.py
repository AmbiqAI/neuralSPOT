from string import Template
import re

required_sections = ['ROMEM', 'RWMEM', 'STACK']


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
        return re.sub(r'(pui32Stack\[)([x0-9a-fA-F]*)',
                      r'\g<1>' + str(stack_size),
                      line)

    output_lines = map(fix_line, input_lines)
    output_text = ''.join(output_lines)

    # Write the new file back to the input
    # Specify the newline parameter to output with Unix line endings.
    with open(filename, 'w', newline='\n') as f:
        f.write(output_text)


def generate_link_script(config):
    robase = config['ROMEM']['start']
    roend = robase + config['ROMEM']['size']
    rwbase = config['RWMEM']['start']
    rwend = rwbase + config['RWMEM']['size']
    tcmbase = config['TCM']['start']
    tcmend = tcmbase + config['TCM']['size']
    stackbase = config['STACK']['start']
    stackend = stackbase + config['STACK']['size']

    D = dict()
    D['robase'] = format_hex(robase)
    D['roend'] = format_hex(roend)
    D['rwbase'] = format_hex(rwbase)
    D['rwend'] = format_hex(rwend)
    D['tcmbase'] = format_hex(tcmbase)
    D['tcmend'] = format_hex(tcmend)

    D['stackbase'] = format_hex(stackbase)
    D['stackend'] = format_hex(stackend)

    D['stack_size'] = config['STACK']['size']

    #D['section_defs'] = format_sections(section_definition, config)
    #D['section_blocks'] = format_sections(section_block, config)
    #D['section_placements'] = format_sections(section_placement, config)

    return link_script_template.substitute(**D)


def format_sections(template, config):

    def fill_section_template(section):
        mapping = {
            'section_name': section,
            'block_name': section + '_BLOCK',
            'mem_name': section + '_MEM',
            'align': 4,
            'start': format_hex(config[section]['start']),
            'size': format_hex(config[section]['size']),
            'end': format_hex(config[section]['start'] +
                              config[section]['size']),

            'permissions': convert_permissions(config[section]['perm']),
        }

        return template.substitute(**mapping)

    def is_extra_section(section):
        return section not in required_sections

    sections = filter(is_extra_section, config)

    return ''.join(map(fill_section_template, sections))


def convert_permissions(perm_string):
    if 'r' in perm_string and 'w' in perm_string and 'x' in perm_string:
        return 'readwrite'
    else:
        return 'readonly'


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
//
define region ROMEM = mem:[from ${robase} to ${roend}];
define region RWMEM = mem:[from ${rwbase} to ${rwend}];
define region TCM = mem:[from ${tcmbase} to ${tcmend}];
define region STACKMEM = mem:[from ${stackbase} to ${stackend}];

//
// Define blocks for logical groups of data.
//
define block HEAP with alignment = 0x8, size = 0x00000000 { };
define block CSTACK with alignment = 0x8, size = ${stack_size}
{
    section .stack
};

define block ROSTART with fixed order
{
    readonly section .intvec,
    readonly section .patch
};

//
// Set section properties.
//
initialize by copy { readwrite };
initialize by copy { section RWMEM };
do not initialize { section .noinit };
do not initialize { section .stack };

//
// Place code sections in memory regions.
//
place at start of ROMEM { block ROSTART };
place in ROMEM { readonly };
place at start of STACKMEM { block CSTACK};
place in RWMEM { block HEAP, readwrite, section .noinit };
place in TCM { section .tcm };
''')

section_definition = Template('''\
define region ${mem_name} = mem:[from ${start} to ${end}];
''')

section_block = Template('''\
define block ${block_name} with alignment = ${align}, size = ${size}
{
    ${permissions} section ${section_name}
};
''')

section_placement = Template('''\
place in ${mem_name} { block ${block_name} };
''')
