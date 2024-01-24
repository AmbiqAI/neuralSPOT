#!/usr/bin/env python3

import argparse
import os.path

#******************************************************************************
#
# Read in the binary files and output the merged binary
#
#******************************************************************************
def process(loaderfile, appfile, installaddress, flagsval, outfile, loaderaddress, chiptype):

    # Open the file, and read it into an array of integers.
    loader_binarray = bytearray(os.path.getsize(loaderfile))
    with open(loaderfile, mode = 'rb') as f:
        f.readinto(loader_binarray)
        f.close()

    # Open the file, and read it into an array of integers.
    with open(appfile, mode = 'rb') as f_app:
        app_binarray = f_app.read()
        f_app.close()

    loader_length = len(loader_binarray)
    print("loader size ",hex(loader_length), "(", loader_length, ")")

    app_length  = len(app_binarray)
    print("App size ",hex(app_length), "(", app_length, ")")

    install_address = int(installaddress, 16)
    print("install_address ",hex(install_address))

    flags = int(flagsval, 16)
    print("flags ",hex(flags))

    # Determine padding length
    # We pad the binary to 4 byte
    pad_length = ((loader_length + 3) & ~0x3) - loader_length

    # generate mutable byte array
    pad_binarray = bytearray([0]*pad_length);

    # Patch offset - immediately following the vector table
    if chiptype == 'apollo3p':
        patch_offset = 200
    elif chiptype == 'apollo3':
        patch_offset = 192
    else:
        print("please input supported chip type: apollo3 or apollo3p")
        return

    # Make sure the binary was generated with patch info
    if chiptype == 'apollo3p':
        patch_size = 56
    elif chiptype == 'apollo3':
        patch_size = 64
    else:
        patch_size = 64
        print("please input supported chip type: apollo3 or apollo3p")
        return

    # Make sure the binary was generated with patch info
    for x in range (0, patch_size):
#        print (hex(loader_binarray[patch_offset + x]))
        if (loader_binarray[patch_offset + x] != 0):
            print("Loader binary not generated with Patch Table - Aborting script")
            return


    # patch the loader binary with app binary information
    bin_addr = ((loader_length + 3) & ~0x3) + int(loaderaddress,16)
    print("Patch[0] ",hex(bin_addr), "(",bin_addr,")")
    loader_binarray[patch_offset + 0]  = (bin_addr >>  0) & 0x000000ff;
    loader_binarray[patch_offset + 1]  = (bin_addr >>  8) & 0x000000ff;
    loader_binarray[patch_offset + 2]  = (bin_addr >> 16) & 0x000000ff;
    loader_binarray[patch_offset + 3]  = (bin_addr >> 24) & 0x000000ff;

    print("Patch[1] ",hex(install_address), "(",install_address,")")
    loader_binarray[patch_offset + 4]  = (install_address >>  0) & 0x000000ff;
    loader_binarray[patch_offset + 5]  = (install_address >>  8) & 0x000000ff;
    loader_binarray[patch_offset + 6]  = (install_address >> 16) & 0x000000ff;
    loader_binarray[patch_offset + 7]  = (install_address >> 24) & 0x000000ff;

    print("Patch[2] ",hex(app_length), "(",app_length,")")
    loader_binarray[patch_offset + 8]  = (app_length >>  0) & 0x000000ff;
    loader_binarray[patch_offset + 9]  = (app_length >>  8) & 0x000000ff;
    loader_binarray[patch_offset + 10] = (app_length >> 16) & 0x000000ff;
    loader_binarray[patch_offset + 11] = (app_length >> 24) & 0x000000ff;

    print("Patch[3] ",hex(flags), "(",flags,")")
    loader_binarray[patch_offset + 12]  = (flags >>  0) & 0x000000ff;
    loader_binarray[patch_offset + 13]  = (flags >>  8) & 0x000000ff;
    loader_binarray[patch_offset + 14] = (flags >> 16) & 0x000000ff;
    loader_binarray[patch_offset + 15] = (flags >> 24) & 0x000000ff;

    print("Writing output file ", outfile + '.bin')
    # now output all three binary arrays in the proper order
    with open(outfile + '.bin', mode = 'wb') as out:
        out.write(loader_binarray)
        out.write(pad_binarray)
        out.write(app_binarray)

def parse_arguments():
    parser = argparse.ArgumentParser(description =
                     'Combine External Flash Loader & external flash image in to a single blob for downloading to internal flash.')

    parser.add_argument('--loaderbin', dest='loaderbin',
                        help='External Flash loader binary file (loader.bin)')

    parser.add_argument('--appbin', dest='appbin',
                        help='External Flash Application binary file (app.bin)')

    parser.add_argument('--install-address', dest='installaddress', default='0x04000000',
                        help='Install address of the application. Default = 0x04000000 (start of MSPI XIP region)')

    parser.add_argument('--flags', dest='flags', default='0x0',
                        help = 'Customization Params bitmask ('
                        '0x1 = Scrambling enabled, '
                        '0x2 = Run binary after loading '
                        ')'
                        )

    parser.add_argument('--outbin', dest='outfile',
                        help = 'Output binary file for internal flash (out.bin)')

    parser.add_argument('--loader-address', dest='loaderaddress', default='0x0000C000',
                        help='Link address of the loader program. Default = 0x0000C000 (start of loader program in flash)')

    parser.add_argument('--chipType', dest='chiptype', default='apollo3p',
                        help = 'selection of chip type')

    args = parser.parse_args()

    return args

#******************************************************************************
#
# Main function.
#
#******************************************************************************
def main():
    # Read the arguments.
    args = parse_arguments()

    process(args.loaderbin, args.appbin, args.installaddress, args.flags, args.outfile, args.loaderaddress, args.chiptype)

if __name__ == '__main__':
    main()
