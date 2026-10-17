#!/usr/bin/env python3

"""
A script for converting .WAV files to wavetables for Mozzi.

Reads bitness, sample format, and sample rate from the WAV header automatically.
Supports 8-bit unsigned, 16-bit signed, 24-bit signed, and 32-bit signed PCM WAV files,
as well as 32-bit IEEE float WAV files (samples in -1.0..1.0 range).

All sample data is converted to signed 8-bit or 16-bit values.
Output values are centered around 0 and can be negated without overflow.
If audio file is stereo, only the first channel is used.

Requires Python 3.9+, no dependencies.

NOTE: Using Audacity to prepare sound files:
	
For generated waveforms like sine or sawtooth, set the project
rate to the size of the wavetable you wish to create, which must
be a power of two (eg. 8192), and set the selection format
(beneath the editing window) to samples. Then you can generate
and save 1 second of a waveform and it will fit your table
length.

For a recorded audio sample, set the project rate to the
MOZZI_AUDIO_RATE (16384 in the current version). 
Samples can be any length, as long as they fit in your Arduino.
	
Save the file by "Export" -> "Export as WAV". 
To keep all the details, choose "32-bit float" encoding. 
Other supported encodings are 8,16,24,32-bit PCM.
	
Now use the file you just exported, as the "infile" to convert.


Author: Paul Melnikov, 2026-04
"""

# 	Usage:
# 	>>>wav2mozzi.py infile [-t tablename] [-o outfile] [-b {8,16}] [--no-symmetric-output]
#   Arguments:
# 	* infile        The .WAV file to convert.
#   * -t tablename  (Optional) The name to give the table. Default: uppercase input filename.
# 	* -o outfile    (Optional) The output .h file. Default: derived from input filename.
# 	* -b, --output-bits
#                   (Optional) Output sample size in bits. Allowed: 8 or 16. Default: 8.
# 	* --symmetric-output, --no-symmetric-output
#                   (Optional) Generate symmetric signed output range. Default: enabled.


import sys, os, textwrap, struct, random, argparse, re

def read_wav(infile):
    """Read a WAV file, supporting both PCM and IEEE float formats.
    Returns (nchannels, sample_size, samplerate, nframes, raw_bytes, is_float).
    
    Technically, python has built-in "wave" library for this,
    but it doesn't support IEEE floats, so decode headers manually.
    """
    with open(infile, 'rb') as f:
        # Parse RIFF header
        riff = f.read(4)
        if riff != b'RIFF':
            raise IOError("Not a valid WAV file (missing RIFF header)")
        f.read(4)  # file size
        wave_id = f.read(4)
        if wave_id != b'WAVE':
            raise IOError("Not a valid WAV file (missing WAVE identifier)")

        fmt_found = False
        data_raw = None
        audio_format = None
        n_channels = None
        sample_rate = None
        sample_size = None
        n_frames = None

        while True:
            chunk_header = f.read(8)
            if len(chunk_header) < 8:
                break
            chunk_id = chunk_header[:4]
            chunk_size = struct.unpack('<I', chunk_header[4:8])[0]

            if chunk_id == b'fmt ':
                fmt_data = f.read(chunk_size)
                audio_format = struct.unpack('<H', fmt_data[0:2])[0]
                n_channels = struct.unpack('<H', fmt_data[2:4])[0]
                sample_rate = struct.unpack('<I', fmt_data[4:8])[0]
                bits_per_sample = struct.unpack('<H', fmt_data[14:16])[0]
                sample_size = bits_per_sample // 8
                fmt_found = True
            elif chunk_id == b'data':
                data_raw = f.read(chunk_size)
                n_frames = len(data_raw) // (n_channels * sample_size)
                break
            else:
                f.read(chunk_size)
                if chunk_size % 2 == 1:
                    f.read(1)  # padding byte

        if not fmt_found or data_raw is None:
            raise IOError("Could not find fmt/data chunks in WAV file")

        # audio_format: 1 = PCM, 3 = IEEE float
        is_float = (audio_format == 3)
        if audio_format not in (1, 3):
            raise ValueError(f"Unsupported WAV format code {audio_format} (only PCM=1 and IEEE float=3 are supported)")

        return n_channels, sample_size, sample_rate, n_frames, data_raw, is_float


def wav2mozzi(infile, outfile, tablename, output_bytes=1, symmetric_output=True):
    """
    Convert a WAV file to a Mozzi wavetable header.
    - infile: input WAV file path
    - outfile: output .h file path
    - tablename: name to use for the generated variables (e.g. "MYTABLE")
    - output_bytes: number of bytes for output samples (1 or 2 supported)
    - symmetric_output: if True, negative range is the same as positive (e.g. -128 becomes -127 for 1 byte)
    """
    n_channels, sample_size, sample_rate, n_frames, data_bytes, is_float = read_wav(infile)
    print("opened", infile)
    format_str = "float" if is_float else "PCM"
    print(f"  channels: {n_channels}, rate: {sample_rate} Hz, sample size: {sample_size * 8}bit, samples: {n_frames}, format: {format_str}")

    # for clarity, convert input to -1.0...1.0 first

    input_midpoint = 0.0
    input_max = 1.0

    if is_float:
        if sample_size != 4:
            raise ValueError(f"Unsupported float sample size: {sample_size} B")
    else:
        if sample_size == 1:
            # 8-bit WAV is unsigned 0..255
            # by definition, mid point is 128
            input_midpoint = 128
            input_max = 255
        elif sample_size == 2:
            # 16-bit little-endian signed
            input_midpoint = 0
            input_max = 2**15 - 1
        elif sample_size == 3:
            # 24-bit little-endian signed
            input_midpoint = 0
            input_max = 2**23 - 1
        elif sample_size == 4:
            # 32-bit little-endian signed
            input_midpoint = 0
            input_max = 2**31 - 1
        else:
            raise ValueError(f"Unsupported sample size: {sample_size} B")

    # Decode raw bytes into samples (mono only - take first channel)
    values = []

    frame_size = n_channels * sample_size
    for i in range(n_frames):
        offset = i * frame_size
        sample_bytes = data_bytes[offset:offset + sample_size]  # first channel only
        if is_float:
            if sample_size == 4:
                val = struct.unpack('<f', sample_bytes)[0]
        else:
            if sample_size == 1:
                # 8-bit WAV is unsigned 0..255
                val = struct.unpack('B', sample_bytes)[0]
            elif sample_size == 2:
                # 16-bit little-endian signed
                val = struct.unpack('<h', sample_bytes)[0]
            elif sample_size == 3:
                # 24-bit little-endian signed
                b = sample_bytes
                val = b[0] | (b[1] << 8) | (b[2] << 16)
                if val >= 0x800000:  # convert to signed
                    val -= 0x1000000
            elif sample_size == 4:
                val = struct.unpack('<i', sample_bytes)[0]
        values.append(val)

    # Scale to -1.0...1.0 range
    if is_float:
        scaled_values = values  # already in -1.0...1.0 range
    else:
        input_max = input_max - input_midpoint  # for uint8, max should be is 127
        scaled_values = [(v - input_midpoint) / input_max for v in values]

    # Convert to signed 8-bit or 16-bit range for output
    c_type = 'int8_t' if output_bytes == 1 else 'int16_t'
    out_range = [-128, 127] if output_bytes == 1 else [-2**15, 2**15-1]
    if symmetric_output:
        out_range[0] += 1  # e.g. -128 becomes -127 for symmetric range

    out_values = [
        max(out_range[0], min(out_range[1], int(v * out_range[1])))
        for v in scaled_values
    ]

    # Dither triple-33 sequences (taken from char2mozzi.py)
    for i in range(len(out_values) - 2):
        if out_values[i] == 33 and out_values[i+1] == 33 and out_values[i+2] == 33:
            out_values[i+2] = random.choice([32, 34])

    with open(outfile, "w") as fout:
        fout.write(f'#ifndef {tablename}_H_\n')
        fout.write(f'#define {tablename}_H_\n\n')
        fout.write('// Generated by wav2mozzi.py"\n')
        fout.write('// Arguments: "' + ' '.join(sys.argv[1:]) + '"\n\n')
        fout.write('#include <Arduino.h>\n')
        fout.write('#include "mozzi_pgmspace.h"\n\n')
        fout.write(f'#define {tablename}_NUM_CELLS {len(out_values)}\n')
        fout.write(f'#define {tablename}_SAMPLERATE {sample_rate}\n\n')
        fout.write(f'CONSTTABLE_STORAGE({c_type}) {tablename}_DATA [] = {{\n')
        outstring = ''
        for v in out_values:
            outstring += str(v) + ", "
        outstring = textwrap.fill(outstring, width=80, initial_indent='  ', subsequent_indent='  ')
        fout.write(outstring)
        fout.write('\n};\n')
        fout.write(f'\n#endif /* {tablename}_H_ */\n')

    print("wrote", outfile)
    sym_str = ", symmetric" if symmetric_output else ""
    print(f"  table name: {tablename}, type: {c_type}{sym_str}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('infile', help='Input .WAV file')
    parser.add_argument('-t', '--tablename', help='Table name for the generated header (default: uppercase input filename)')
    parser.add_argument('-o', '--outfile', help='Output .h file (default: derived from input filename)')
    parser.add_argument('-b', '--output-bits', type=int, choices=(8, 16), default=8,
                        help='Output sample size in bits (default: 8)')
    parser.add_argument('-s', '--symmetric-output', action=argparse.BooleanOptionalAction, default=True,
                        help='Generate symmetric signed output range (default: enabled)')
    args = parser.parse_args()

    infile = os.path.expanduser(args.infile)
    if args.tablename:
        tablename = args.tablename
    else:
        # derive from filename: strip extension, keep only alnum/underscore, uppercase
        tablename = re.sub(r'[^A-Za-z0-9_]', '_', os.path.splitext(os.path.basename(infile))[0]).upper()
    outfile = os.path.expanduser(args.outfile) if args.outfile else os.path.splitext(infile)[0] + '.h'

    try:
        wav2mozzi(infile, outfile, tablename,
            output_bytes=args.output_bits // 8,
            symmetric_output=args.symmetric_output)
    except (IOError, ValueError) as e:
        print(f"Error: {e}", file=sys.stderr)
