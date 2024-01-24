#            <TextAddressRange></TextAddressRange>
#            <DataAddressRange></DataAddressRange>
import argparse
import yaml
from . import iar_link
from . import gcc_link
from . import keil_link
from . import keil6_link

# Linker file locations.
IAR_LD_SCRIPT = "iar/linker_script.icf"
KEIL_LD_SCRIPT = "keil/linker_script.sct"
KEIL_DEBUG_FILE = "keil/Dbg_RAM.ini"
KEIL6_LD_SCRIPT = "keil6/linker_script.sct"
KEIL6_DEBUG_FILE = "keil6/Dbg_RAM.ini"
GCC_LD_SCRIPT = "gcc/linker_script.ld"

KEIL_STARTUP_FILE = "keil/startup_keil.s"
KEIL6_STARTUP_FILE = "keil6/startup_keil6.s"
IAR_STARTUP_FILE = "iar/startup_iar.c"
GCC_STARTUP_FILE = "gcc/startup_gcc.c"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('config_file', default='default_config_apollo3.yaml')
    parser.add_argument('-i', dest='iar', action='store_true')
    parser.add_argument('-k', dest='keil', action='store_true')
    parser.add_argument('-s', dest='keil6', action='store_true')
    parser.add_argument('-g', dest='gcc', action='store_true')

    args = parser.parse_args()

    # Read the configuration file.
    config = read_configuration(args.config_file)

    # Figure out what we want to build. If no specific toolchains were
    # specified, just build everything. Otherwise, only build what was
    # specified.
    build_all = True

    if args.keil:
        write_keil_linker_scripts(config)
        build_all = False
        
    if args.keil6:
        write_keil6_linker_scripts(config)
        build_all = False

    if args.iar:
        write_iar_linker_scripts(config)
        build_all = False

    if args.gcc:
        write_gcc_linker_scripts(config)
        build_all = False

    if build_all:
        write_keil_linker_scripts(config)
        write_keil6_linker_scripts(config)
        write_iar_linker_scripts(config)
        write_gcc_linker_scripts(config)

    # Print the memory map
    print_memory_map(config)


def generate_files(config_file, toolchains):
    config = read_configuration(config_file)

    print_memory_map(config)

    if 'keil' in toolchains:
        write_keil_linker_scripts(config)
        
    if 'keil6' in toolchains:
        write_keil6_linker_scripts(config)

    if 'iar' in toolchains:
        write_iar_linker_scripts(config)

    if 'gcc' in toolchains:
        write_gcc_linker_scripts(config)


def read_configuration(config_file):
    """Read a configuration YAML files and return a dictionary of memory sections"""

    # Read the YAML configuration file as is.
    with open(config_file) as file_object:
        config_string = file_object.read()
        config = yaml.load(config_string, Loader=yaml.FullLoader)

        memory_sections = config['MemorySections']

        # Search through the memory sections...
        for name in memory_sections.keys():

            # Find the start value, and convert it if necessary.
            start = memory_sections[name]['start']
            memory_sections[name]['start'] = convert_number(start)

            # If we find a size value, use it.
            if 'size' in memory_sections[name]:
                size = memory_sections[name]['size']
                memory_sections[name]['size'] = convert_number(size)

            # It not, try to use an "end" value.
            elif 'end' in memory_sections[name]:
                end = memory_sections[name]['end']
                memory_sections[name]['size'] = convert_number(end) - convert_number(start)

        stack_size = config['StackOptions']['size']
        config['StackOptions']['size'] = convert_number(stack_size)

        # Create a memory section for the stack.
        memory_sections['STACK'] = dict()

        # Create a section for the stack. To do this, we'll either need to
        # carve space out of TCM or RWMEM.
        if config['StackOptions']['place_in_tcm']:
            if config['StackOptions']['size'] > memory_sections['TCM']['size']:
                raise LinkerConfigError("Stack ({} B) doesn't fit in TCM ({} B)".format(
                    config['StackOptions']['size'],
                    memory_sections['TCM']['size']))

            memory_sections['STACK']['start'] = memory_sections['TCM']['start']

            memory_sections['STACK']['size'] = config['StackOptions']['size']

            memory_sections['TCM']['start'] = (memory_sections['STACK']['start'] +
                                               config['StackOptions']['size'])

            memory_sections['TCM']['size'] = (memory_sections['TCM']['size'] -
                                              config['StackOptions']['size'])

        else:
            if config['StackOptions']['size'] > memory_sections['RWMEM']['size']:
                raise LinkerConfigError("Stack ({} B) doesn't fit in RWMEM ({} B)".format(
                    config['StackOptions']['size'],
                    memory_sections['RWMEM']['size']))

            memory_sections['STACK']['start'] = memory_sections['RWMEM']['start']

            memory_sections['STACK']['size'] = config['StackOptions']['size']

            memory_sections['RWMEM']['start'] = (memory_sections['STACK']['start'] +
                                                 config['StackOptions']['size'])

            memory_sections['RWMEM']['size'] = (memory_sections['RWMEM']['size'] -
                                                config['StackOptions']['size'])

        return memory_sections


def write_keil_linker_scripts(config):
    try:
        linker_file_data, debug_file_data = keil_link.generate_link_script(config)

        # Specify the newline parameter to output with Unix line endings.
        with open(KEIL_LD_SCRIPT, 'w', newline='\n') as linker_file:
            linker_file.write(linker_file_data)

        with open(KEIL_DEBUG_FILE, 'w', newline='\n') as debug_file:
            debug_file.write(debug_file_data)

        keil_link.fix_startup_file(config, KEIL_STARTUP_FILE)

    except FileNotFoundError:
        pass

def write_keil6_linker_scripts(config):
    try:
        linker_file_data, debug_file_data = keil6_link.generate_link_script(config)

        # Specify the newline parameter to output with Unix line endings.
        with open(KEIL6_LD_SCRIPT, 'w', newline='\n') as linker_file:
            linker_file.write(linker_file_data)

        with open(KEIL6_DEBUG_FILE, 'w', newline='\n') as debug_file:
            debug_file.write(debug_file_data)

        keil6_link.fix_startup_file(config, KEIL6_STARTUP_FILE)

    except FileNotFoundError:
        pass

def write_iar_linker_scripts(config):
    try:
        # Specify the newline parameter to output with Unix line endings.
        with open(IAR_LD_SCRIPT, 'w', newline='\n') as linker_file:
            linker_file.write(iar_link.generate_link_script(config))

        iar_link.fix_startup_file(config, IAR_STARTUP_FILE)

    except FileNotFoundError:
        pass


def write_gcc_linker_scripts(config):
    try:
        # Specify the newline parameter to output with Unix line endings.
        with open(GCC_LD_SCRIPT, 'w', newline='\n') as linker_file:
            linker_file.write(gcc_link.generate_link_script(config))

        gcc_link.fix_startup_file(config, GCC_STARTUP_FILE)

    except FileNotFoundError:
        pass


def convert_number(N):
    """Take in an integer or a numerical string ending in 'K', and convert it to an int"""
    if isinstance(N, int):
        return N
    elif isinstance(N, str):
        if N.endswith('K'):
            return int(N[:-1]) * 1024
        else:
            raise LinkerConfigError('"{}" not recognized as a number'.format(N))
    else:
        raise LinkerConfigError('"{}" not recognized as a number'.format(N))


def print_memory_map(memory_sections):
    """Show the memory map in a human readable format"""
    # Sort the section names by their starting address.
    section_names = sorted(memory_sections.keys(), key=lambda x: memory_sections[x]['start'])

    # Search for the longest section name, and record its length.
    max_name_length = max(len(x) for x in section_names)

    # This is a roundabout way to copy the maximum name length into a format
    # string, so we can make the output string look pretty.
    name_format = '{{:{}}}'.format(max_name_length + 1)

    for name in section_names:
        section = memory_sections[name]
        mapping = {
            'name':  name_format.format(name + ':'),
            'start': '0x{:08X}'.format(section['start']),
            'end':   '0x{:08X}'.format(section['start'] + section['size']),
            'size':  section['size'],
        }

        print('{name} {start:10} - {end:10} ({size} bytes)'.format(**mapping))


# Custom error for linker configuration problems.
class LinkerConfigError(Exception):
    pass


if __name__ == '__main__':
    main()
