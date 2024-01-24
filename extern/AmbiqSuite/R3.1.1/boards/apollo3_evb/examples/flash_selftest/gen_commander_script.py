#!/usr/bin/env python3
# ******************************************************************************
#                                                                              *
# Copyright (c) 2020 Ambiq Micro.                                              *
#                                                                              *
# ******************************************************************************
#                                                                              *
# File:     gen_commander_script.py                                            *
#                                                                              *
# Title:    Customizes J-Link Commander scripts to the given .bin file.        *
#           Reads the first 2 words of a binary to get the stack pointer and   *
#           program counter used in the Commander script.                      *
#                                                                              *
# Date:     01/09/2020                                                         *
#                                                                              *
# ******************************************************************************


# ******************************************************************************
# Local "constants" (as if Python really did constants)
# ******************************************************************************

# ******************************************************************************
# Libraries
# ******************************************************************************
import sys          # For sys.argv[0]

# ******************************************************************************
# Global Data
# ******************************************************************************

# ******************************************************************************
#
# Commander script template
#
# ******************************************************************************
commander_script_template = '''\
//*****************************************************************************
//
// {devicefam} ({devpn}) selftest_commander_{TClwr}.jlink
//
// This J-Link Commander batch file can be used to run the flash_selftest
// example, which must execute from SRAM.
//
// For example, to run from a Windows Command Prompt:
//  jlink -CommanderScript selftest_commander_{TClwr}.jlink
//
// The results are stored at address 0x10030000.
//  0xFAE00000 = Pass, the flash tested good.
//  0xFAE0xxxx = Fail, where xxxx is a failure code.
//
//*****************************************************************************
//
// Connect to the Ambiq Micro {devicefam} device and halt.
//
device {devpn}
si SWD
speed 1000
halt

//
//  Load the {TCupr} binary into SRAM.
//
w4 10030000 0
loadbin {TClwr}\\bin\\flash_selftest.bin 0x10000000

//
// Set the SP and PC.
// The SP and PC values can be found as the first words in the .bin.
//  SP = Word 0
//  PC = Word 1
// {TCupr} with PRINT_RESULTS enabled:  SP = {stptr}, PC = {pgmctr}
//
wreg MSP, {stptr}
setPC {pgmctr}

//
// We're ready to go, start the test.
//
g

//
// Periodically check the result.
// For a good part Apollo3 takes about 30s, Apollo3p about 50s.
//
sleep 30000
{sleep}
mem32 0x10030000 1
sleep 1000


//
// Halt the MCU and print the final results.
//
//halt
mem32 0x10030000 1
mem32 0x10030004 2

//
// Close J-Link and quit.
//
qc
'''
# End of commander_script_template


# ******************************************************************************
# Methods
# ******************************************************************************
# **************************************
# writeFile()
# **************************************
def writeFile(outFileNm, newFileData, WindowsCRLF):
    # Write the updated file.
    # Write the file in binary format so we end up with LF (Unix) chars instead of CR/LF (DOS).
    # For Python 3 this requires casting the strings as bytes.
    # For Python 2, we have to write the strings directly as the cast to bytes is not supported.

    if WindowsCRLF:
        # Use 'w' for Windows/DOS
        fout = open(outFileNm, 'w')
    else:
        # Use 'wb' for Unix
        fout = open(outFileNm, 'wb')

    if sys.version_info[0] <= 2:    # tuple element [0] is an int representing the Major version number
        fout.write(newFileData)
    else:
        if WindowsCRLF:
            # Use for Windows/DOS style line endings
            fout.write(newFileData)
        else:
            # Use UTF-8 version for Unix style line endings
            fout.write(bytes(newFileData,'UTF-8'))

    fout.close()
    return


# ******************************************************************************
#
# main
#
# ******************************************************************************

sUsage = ""                                                                         \
    "\n"                                                                            \
    "Generate a customized J-Link Commander script based on the given .bin file.\n" \
    "\n"                                                                            \
    "Usage:\n"                                                                      \
    "    gen_commander_script example.bin gcc|iar|keil [family]\n"                  \
    "\n"                                                                            \
    "  example.bin:  the pathname of the target binary file.\n"                     \
    "  gcc | iar | keil: The target toolchain.\n"                                   \
    "  family = optional family name.  Default is Apollo3..\n"                      \
    "\n"


# Parse the command line
argv = sys.argv[1:]
argc = len(argv)

if argc >= 2:
    infilename  = argv[0]
    toolchain   = argv[1].lower()
    addl_sleep =""
    devfam=argv[2].upper()
    if argc == 3:
        if devfam == "APOLLO3":
            devicePN = "AMA3B1KK-KBR"
        elif devfam == "APOLLO3P":
            devicePN = "AMA3B2KK-KBR"
            addl_sleep ="mem32 0x10030000 1\nsleep 20000\n"
        else:
            "ERROR: Unknown family name given."
            exit(2)
    else:
        devicePN = "AMA3B1KK-KBR"
else:
    print(sUsage)
    exit(1)

if toolchain != "gcc"   and \
   toolchain != "iar"   and \
   toolchain != "keil"  and \
   toolchain != "keil6":
    print(sUsage)
    exit(1)


#
# Open and read the file.
#
fin  = open(infilename, 'rb')
infiledata = fin.read()
fin.close()

#
# Read the first 2 words from the input file.
# The first word is the SP, the second is the PC.
#
if len(infiledata) < 8:
    print("File does not contain enough data.")
    exit(3)

sp_val = int.from_bytes(infiledata[0:4], byteorder='little', signed=False)
pc_val = int.from_bytes(infiledata[4:8], byteorder='little', signed=False)

sSP = "0x%08X" % (sp_val)
sPC = "0x%08X" % (pc_val)

writeFile("selftest_commander_" + toolchain.lower() + ".jlink",
          commander_script_template.format(stptr=sSP, pgmctr=sPC,
                                           TCupr=toolchain.upper(),
                                           TClwr=toolchain.lower(),
                                           devicefam=devfam,
                                           devpn=devicePN,
                                           sleep=addl_sleep),
                                           False)

exit(0)

