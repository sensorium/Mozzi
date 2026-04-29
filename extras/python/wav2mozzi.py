#!/usr/bin/env python

##@file wav2mozzi.py
#  @ingroup util
#	A script for converting .WAV sound files to wavetables for Mozzi.
#
#	Usage:
#	>>>wav2mozzi.py infile [-t tablename] [-o outfile]
#
#	@param infile		The .WAV file to convert.
#	@param -t tablename	(Optional) The name to give the table. Default: uppercase input filename.
#	@param -o outfile	(Optional) The output .h file. Default: derived from input filename.
#
#	Reads bitness, sample format, and sample rate from the WAV header automatically.
#	Supports 8-bit unsigned, 16-bit signed, 24-bit signed, and 32-bit signed PCM WAV files,
#	as well as 32-bit IEEE float WAV files (samples in -1.0..1.0 range).
#
#	All sample data is converted to signed 8-bit (-128..127).
#   If audio is stereo, only the first channel is used.
#

import sys, os, textwrap, struct, random, argparse, re

def read_wav(infile):
    """Read a WAV file, supporting both PCM and IEEE float formats.
    Returns (nchannels, sampwidth, samplerate, nframes, raw_bytes, is_float)."""
    with open(infile, 'rb') as f:
        # Parse RIFF header
        riff = f.read(4)
        if riff != b'RIFF':
            print("Error: not a valid WAV file (missing RIFF header)")
            sys.exit(1)
        f.read(4)  # file size
        wave_id = f.read(4)
        if wave_id != b'WAVE':
            print("Error: not a valid WAV file (missing WAVE identifier)")
            sys.exit(1)

        fmt_found = False
        data_raw = None
        audio_format = None
        nchannels = None
        samplerate = None
        sampwidth = None
        nframes = None

        while True:
            chunk_header = f.read(8)
            if len(chunk_header) < 8:
                break
            chunk_id = chunk_header[:4]
            chunk_size = struct.unpack('<I', chunk_header[4:8])[0]

            if chunk_id == b'fmt ':
                fmt_data = f.read(chunk_size)
                audio_format = struct.unpack('<H', fmt_data[0:2])[0]
                nchannels = struct.unpack('<H', fmt_data[2:4])[0]
                samplerate = struct.unpack('<I', fmt_data[4:8])[0]
                bits_per_sample = struct.unpack('<H', fmt_data[14:16])[0]
                sampwidth = bits_per_sample // 8
                fmt_found = True
            elif chunk_id == b'data':
                data_raw = f.read(chunk_size)
                nframes = len(data_raw) // (nchannels * sampwidth)
                break
            else:
                f.read(chunk_size)
                if chunk_size % 2 == 1:
                    f.read(1)  # padding byte

        if not fmt_found or data_raw is None:
            print("Error: could not find fmt/data chunks in WAV file")
            sys.exit(1)

        # audio_format: 1 = PCM, 3 = IEEE float
        is_float = (audio_format == 3)
        if audio_format not in (1, 3):
            print("Error: unsupported WAV format code %d (only PCM=1 and IEEE float=3 are supported)" % audio_format)
            sys.exit(1)

        return nchannels, sampwidth, samplerate, nframes, data_raw, is_float

def wav2mozzi(infile, outfile, tablename):
    nchannels, sampwidth, samplerate, nframes, raw, is_float = read_wav(infile)
    print("opened " + infile)
    print("  channels: %d, sample width: %d bytes, rate: %d, frames: %d, format: %s" %
          (nchannels, sampwidth, samplerate, nframes, "float" if is_float else "PCM"))

    # Decode raw bytes into samples (mono only – take first channel)
    values = []
    is_float_data = is_float
    frame_size = nchannels * sampwidth
    for i in range(nframes):
        offset = i * frame_size
        sample_bytes = raw[offset:offset + sampwidth]  # first channel only
        if is_float:
            if sampwidth == 4:
                val = struct.unpack('<f', sample_bytes)[0]
            elif sampwidth == 3:
                # 24-bit float is non-standard but sometimes encountered;
                # pad to 32-bit float
                val = struct.unpack('<f', b'\x00' + sample_bytes)[0]
            else:
                print("Unsupported float sample width: %d bytes" % sampwidth)
                sys.exit(1)
        else:
            if sampwidth == 1:
                # 8-bit WAV is unsigned 0..255 -> convert to signed -128..127
                val = struct.unpack('B', sample_bytes)[0] - 128
            elif sampwidth == 2:
                # 16-bit little-endian signed
                val = struct.unpack('<h', sample_bytes)[0]
            elif sampwidth == 3:
                # 24-bit little-endian signed
                b = sample_bytes
                val = b[0] | (b[1] << 8) | (b[2] << 16)
                if val >= 0x800000:
                    val -= 0x1000000
            elif sampwidth == 4:
                val = struct.unpack('<i', sample_bytes)[0]
            else:
                print("Unsupported sample width: %d" % sampwidth)
                sys.exit(1)
        values.append(val)

    # By this point, everything in values is signed.

    # Convert everything to signed int8
    c_type = 'int8_t'
    if is_float_data:
        # Float WAV: values are in -1.0..1.0 range
        store_values = [max(-128, min(127, int(v * 128 + 0.5))) for v in values]
    elif sampwidth == 1:
        # already int8 range
        store_values = values
    elif sampwidth == 2:
        # 16-bit -> int8: divide by 256
        store_values = [max(-128, min(127, int(round(v / 256.0)))) for v in values]
    elif sampwidth == 3:
        # 24-bit -> int8: divide by 65536
        store_values = [max(-128, min(127, int(round(v / 65536.0)))) for v in values]
    elif sampwidth == 4:
        # 32-bit -> int8: divide by 16777216
        store_values = [max(-128, min(127, int(round(v / 16777216.0)))) for v in values]
    else:
        print("Unsupported sample width: %d bytes" % sampwidth)
        sys.exit(1)

    # Dither triple-33 sequences (taken from char2mozzi.py)
    for i in range(len(store_values) - 2):
        if store_values[i] == 33 and store_values[i+1] == 33 and store_values[i+2] == 33:
            store_values[i+2] = random.choice([32, 34])

    fout = open(os.path.expanduser(outfile), "w")
    fout.write('#ifndef ' + tablename + '_H_\n')
    fout.write('#define ' + tablename + '_H_\n\n')
    fout.write('#include <Arduino.h>\n')
    fout.write('#include "mozzi_pgmspace.h"\n\n')
    fout.write('#define ' + tablename + '_NUM_CELLS ' + str(len(store_values)) + '\n')
    fout.write('#define ' + tablename + '_SAMPLERATE ' + str(samplerate) + '\n\n')
    fout.write('CONSTTABLE_STORAGE(' + c_type + ') ' + tablename + '_DATA [] = {\n')
    outstring = ''
    for v in store_values:
        outstring += str(v) + ", "
    outstring = textwrap.fill(outstring, 80)
    fout.write(outstring)
    fout.write('\n};\n')
    fout.write('\n#endif /* ' + tablename + '_H_ */\n')
    fout.close()
    print("wrote " + outfile)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Convert a .WAV file to a Mozzi wavetable header.')
    parser.add_argument('infile', help='Input .WAV file')
    parser.add_argument('-t', '--tablename', help='Table name for the generated header (default: uppercase input filename)')
    parser.add_argument('-o', '--outfile', help='Output .h file (default: derived from input filename)')
    args = parser.parse_args()

    infile = os.path.expanduser(args.infile)
    if args.tablename:
        tablename = args.tablename
    else:
        # derive from filename: strip extension, keep only alnum/underscore, uppercase
        tablename = re.sub(r'[^A-Za-z0-9_]', '_', os.path.splitext(os.path.basename(infile))[0]).upper()
    outfile = args.outfile if args.outfile else os.path.splitext(infile)[0] + '.h'

    wav2mozzi(infile, outfile, tablename)
