#!/usr/bin/env python3

import os
import ctypes
import logging
import argparse

#ctypes stream to struct
def stream2struct(string, stype):
    if not issubclass(stype, ctypes.Structure):
        raise ValueError('The type of the struct is not a ctypes.Structure')
    length      = ctypes.sizeof(stype)
    stream      = (ctypes.c_char * length)()
    stream.raw  = string
    p           = ctypes.cast(stream, ctypes.POINTER(stype))
    return p.contents

#ctypes struct to stream
def struct2stream(s):
    length  = ctypes.sizeof(s)
    p       = ctypes.cast(ctypes.pointer(s), ctypes.POINTER(ctypes.c_char * length))
    return p.contents.raw

#head table struct
class head(ctypes.LittleEndianStructure):
        _fields_ = [
            ('version',                ctypes.c_uint32 ),
            ('tables_count',           ctypes.c_uint16 ),
            ('font_size',              ctypes.c_uint16 ),
            ('ascent',                 ctypes.c_uint16 ),
            ('descent',                ctypes.c_int16),
            ('typo_ascent',            ctypes.c_uint16 ),
            ('typo_descent',           ctypes.c_int16),
            ('typo_line_gap',          ctypes.c_uint16 ),
            ('min_y',                  ctypes.c_int16),
            ('max_y',                  ctypes.c_int16),
            ('default_advance_width',  ctypes.c_uint16 ),
            ('kerning_scale',          ctypes.c_uint16 ),
            ('index_to_loc_format',    ctypes.c_uint8 ),
            ('glyph_id_format',        ctypes.c_uint8 ),
            ('advance_width_format',   ctypes.c_uint8 ),
            ('bits_per_pixel',         ctypes.c_uint8 ),
            ('xy_bits',                ctypes.c_uint8 ),
            ('wh_bits',                ctypes.c_uint8 ),
            ('advance_width_bits',     ctypes.c_uint8 ),
            ('compression_id',         ctypes.c_uint8 ),
            ('subpixels_mode',         ctypes.c_uint8 ),
            ('padding',                ctypes.c_uint8 ),
            ('underline_position',     ctypes.c_int16 ),
            ('underline_thickness',    ctypes.c_uint16 ),
        ]

def read_section(file, name, offset=0):
    with open(file, "rb") as binary_file:
        binary_file.seek(offset, 0)  # Go to beginning of the file
        section_length = int.from_bytes(binary_file.read(4), byteorder='little', signed=False)
        section_name = binary_file.read(4)
        if section_name == name.encode('utf-8') :
            return section_length, section_name,  binary_file.read(section_length - 8)
        else:
            return 0, None, None

def write_section(file, name, content, length=0):
    if (name == None) or (content == None) :
        return

    #get length
    if length == 0 :
        if(type(content) == list):
            for part in content :
                if(type(part) == bytes):
                    length = length + len(part)
                else:
                    raise TypeError("content must be bytes or list of bytes!")
        elif(type(content) == bytes) :
            length = length + len(content)
        else:
            raise TypeError("content must be bytes or list of bytes!")

        # length takes 4 bytes and name tasks 4 bytes
        length = length + 8

    with open(file, "ab+") as binary_file:
        #write length
        section_length_bytes = length.to_bytes(4, byteorder='little', signed=False)
        binary_file.write(section_length_bytes)

        #write name
        if type(name) == str :
            name = name.encode('utf-8')
        binary_file.write(name)

        #write content
        if(type(content) == list):
            for part in content :
                if(type(part) == bytes):
                    binary_file.write(part)
                else:
                    raise TypeError("content must be bytes or list of bytes!")
        elif(type(content) == bytes) :
            binary_file.write(content)
        else:
            raise TypeError("content must be bytes or list of bytes!")

#read bits from buffer
class bit_iter():
    def __init__(self, raw_data):
        self.byte_pos = 0
        self.bit_pos = -1
        self.byte_value = ctypes.c_uint8(0)
        self.raw_data = raw_data

    def read_bits(self, n_bits):
        if n_bits > 32:
            raise ValueError("n_bits should less than 32!")

        value = 0
        while(n_bits):
            n_bits = n_bits -1
            self.byte_value = ctypes.c_uint8(self.byte_value.value << 1)
            self.bit_pos = self.bit_pos - 1

            if self.bit_pos < 0:
                self.bit_pos = 7
                self.byte_value = ctypes.c_uint8(self.raw_data[self.byte_pos])
                self.byte_pos = self.byte_pos + 1

            if (self.byte_value.value & 0x80) :
                bit = 0x1
            else:
                bit = 0x0

            value = value | (bit << n_bits)

        return value

#write bits to buffer
class bit_iter_write():
    def __init__(self):
        self.bit_pos = 7
        self.byte_value = ctypes.c_uint8(0)
        self.write_buffer = []

    def write_bits(self, bits, n_bits):
        if n_bits > 32:
            raise ValueError("n_bits should less than 32!")

        while(n_bits):
            n_bits = n_bits -1

            if bits & (0x1 << n_bits) :
                bit = 0x1
            else:
                bit = 0x0

            self.byte_value.value = self.byte_value.value | (bit << self.bit_pos)

            self.bit_pos = self.bit_pos - 1

            if self.bit_pos < 0:
                self.write_buffer.append(self.byte_value.value)
                self.bit_pos = 7
                self.byte_value = ctypes.c_uint8(0)

    def flush_bits(self):
        if(self.bit_pos != 7) :
            self.write_buffer.append(self.byte_value.value)
            self.bit_pos = 7
            self.byte_value = ctypes.c_uint8(0)

    def get_result(self):
        self.flush_bits()
        buffer_bytes = bytes(self.write_buffer)
        self.write_buffer = []
        return buffer_bytes

def main(file_path, output_path):

    #Fetch each section
    file_offset = 0
    head_length, head_name, head_raw = read_section(file_path, "head", file_offset)
    file_offset = file_offset + head_length
    cmap_length, cmap_name, cmap_raw = read_section(file_path, "cmap", file_offset)
    file_offset = file_offset + cmap_length
    loca_length, loca_name, loca_raw = read_section(file_path, "loca", file_offset)
    file_offset = file_offset + loca_length
    glyf_length, glyf_name, glyf_raw = read_section(file_path, "glyf", file_offset)
    file_offset = file_offset + glyf_length
    kern_length, kern_name, kern_raw = read_section(file_path, "kern", file_offset)

    #Check file integrity
    if head_length == 0:
        logging.error("head table is missing!")
        return
    if loca_length == 0 :
        logging.error("loca table is missing!")
        return 
    if glyf_length == 0 :
        logging.error("glyf table is missing!")
        return

    font_header = stream2struct(head_raw, head)

    #Check file header
    if font_header.compression_id != 0:
        logging.error("glyf compression is not supported!")
        return

    if font_header.subpixels_mode != 0:
        logging.error("Subpixel rendering is not supported!")
        return

    if font_header.bits_per_pixel == 3:
        logging.error("bpp=3 is not supported!")
        return

    loca_count = int.from_bytes(loca_raw[0:4], byteorder='little', signed=False)

    if font_header.index_to_loc_format == 0 :
        loca_array = (ctypes.c_uint16 * loca_count).from_buffer_copy(loca_raw[4:])
    elif(font_header.index_to_loc_format == 1):
        loca_array = (ctypes.c_uint32 * loca_count).from_buffer_copy(loca_raw[4:])
    else:
        logging.error("Unknown index_to_loc_format: %d." , font_header.index_to_loc_format)

    #Use this list to hold glyf bitmap
    glyf_list = []

    #Use this value to update loca array
    glyf_offset = 8

    for i in range(loca_count):
        print(loca_array[i])

    for i in range(loca_count):
        if i < loca_count-1 :
            glyph_info = glyf_raw[loca_array[i] - 8 : loca_array[i+1] - 8]
        else:
            glyph_info = glyf_raw[loca_array[i] - 8: ]

        if len(glyph_info) == 0 :
            continue

        bits_it_read = bit_iter(glyph_info)
        bits_it_write = bit_iter_write()

        # read advance bits
        if font_header.advance_width_bits != 0 :
            adv_w = bits_it_read.read_bits(font_header.advance_width_bits)
            bits_it_write.write_bits(adv_w, font_header.advance_width_bits)

        # read offset x
        ofs_x = bits_it_read.read_bits(font_header.xy_bits)
        bits_it_write.write_bits(ofs_x, font_header.xy_bits)

        # read offset y
        ofs_y = bits_it_read.read_bits(font_header.xy_bits)
        bits_it_write.write_bits(ofs_y, font_header.xy_bits)

        # read bbox w
        box_w = bits_it_read.read_bits(font_header.wh_bits)
        bits_it_write.write_bits(box_w, font_header.wh_bits)

        # read bbox h
        box_h = bits_it_read.read_bits(font_header.wh_bits)
        bits_it_write.write_bits(box_h, font_header.wh_bits)

        # Align to bytes to speed the font load operation
        bits_it_write.flush_bits()

        # read glyf bitmap and adjust the alignment
        for row in range(box_h):
            for col in range(box_w):
                #read pixel
                pixel = bits_it_read.read_bits(font_header.bits_per_pixel)
                bits_it_write.write_bits(pixel, font_header.bits_per_pixel)

            #Start address of horizontal line should align to byte, this is required by NemaGFX
            bits_it_write.flush_bits()

        glyf_content = bits_it_write.get_result()

        #Recode the aligned glyf
        glyf_list.append( glyf_content)

        # Update the loca array
        if font_header.index_to_loc_format == 0 :
            loca_array[i] = ctypes.c_uint16(glyf_offset)
        else:
            loca_array[i] = ctypes.c_uint32(glyf_offset)

        #Updata offset
        glyf_offset = glyf_offset + len(glyf_content)

    #Write back head
    write_section(output_path, head_name, head_raw, length=head_length)
    #Write back cmap
    write_section(output_path, cmap_name, cmap_raw, length=cmap_length)
    #Write back loca
    write_section(output_path, loca_name, [loca_raw[0:4], struct2stream(loca_array)])
    #Write back glyf
    write_section(output_path, glyf_name, glyf_list)
    #Write back kern
    write_section(output_path, kern_name, kern_raw, length=kern_length)



if __name__ == '__main__':

    parser = argparse.ArgumentParser(description = 'Parse the lvgl font file and adjust the file format to satisfy requirements from NemaGFX')
    #parser.add_argument('binfile', default='./cour.bin', help = 'path to lvgl font file, support bin format only. ')
    parser.add_argument('-i', dest='binfile', default='./cour.bin', help = 'path to lvgl font file, support bin format only. ')
    parser.add_argument('-o', dest='output_path',
                        help = 'output file path (default is ./ambiq_xxx.bin, xxx stands the input file name)')

    args = parser.parse_args()

    if args.output_path == None :
        filename = os.path.basename(args.binfile)
        filename_split = os.path.splitext(filename)
        output_name = 'ambiq_' + filename_split[0] + '.bin'
        args.output_path = os.path.join(os.path.dirname( os.path.abspath(__file__) ) ,
                                        output_name)

    if os.path.exists(args.output_path):
        os.remove(args.output_path)

    print("Input:" + os.path.abspath(args.binfile))
    print("Output:" + args.output_path)

    main(args.binfile, args.output_path)

    print("Done")