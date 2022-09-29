"""
Scripts to transform PCM data to WAV file.

Usage:
    pcm_to_wav.py [--input=<pcm_bin_file_name>]

Options:
    -h --help                                   Show this screen.
    --input=<pcm_bin_file_name>                 Input path of pcm data file.[default: .\record_data.bin]
"""
from docopt import docopt
import struct
import soundfile as sf
import numpy as np
import time

args = docopt(__doc__)

pcmBinFile = args['--input']

binFile = open(pcmBinFile, 'rb')

context=binFile.read()

audioStream = struct.unpack('<'+'h'*(len(context)//2), context)

audioStream = np.array(audioStream)

wavStream = np.array([0]*(len(audioStream)//2), dtype=float)

for i in range(len(wavStream)):
    # wavStream[i] = 0.5 * audioStream[2*i] + 0.5 * audioStream[2*i+1]
    wavStream[i] = audioStream[2*i]

wavStream = wavStream / 32768.

now = time.strftime("-%Y-%m-%d-%H_%M", time.localtime(time.time()))

sf.write('pcm_wavStream' + now +'.wav', wavStream, 16000)

# Normlization for human listening
wavStream = wavStream/np.max([np.max(wavStream),np.abs(np.min(wavStream))])

sf.write('pcm_wavNormStream' + now +'.wav', wavStream, 16000)
