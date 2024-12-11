#!/usr/bin/env python3
"""
For converting raw audio files to Mozzi table

To generate waveforms using Audacity:
Set the project rate to the size of the wavetable you wish to create, which must
be a power of two (eg. 8192), and set the selection format
(beneath the editing window) to samples. Then you can generate
and save 1 second of a waveform and it will fit your table
length.

To convert samples using Audacity:
For a recorded audio sample, set the project rate to the
MOZZI_AUDIO_RATE (16384 in the current version).
Samples can be any length, as long as they fit in your Arduino.
Save by exporting with the format set to "Other uncompressed formats",
"Header: RAW(headerless)" and choose the encoding you prefer (Signed 8/16/32-bit PCM or 32-bit Float).

Alternative to convert samples via CLI using sox (http://sox.sourceforge.net/):
sox <inputfile> -b <8/16/32> -c 1 -e <floating-point/signed-integer> -r 16384 <outputfile>

Now use the file you just exported, as the "input_file" to convert and
set the other parameters according to what you chose for conversion.
"""

import array
import math
import random
import sys
import textwrap
from argparse import ArgumentParser
from pathlib import Path


def float2mozzi(args):
    input_path = args.input_file.expanduser()
    output_path = (
        args.output_file.expanduser()
        if args.output_file is not None
        else input_path.with_suffix(".h")
    )

    with input_path.open("rb") as fin, output_path.open("w") as fout:
        print(f"opened {input_path}")
        num_input_values = int(
            input_path.stat().st_size / (args.input_bits / 8)
        )  # Adjust for number format (table at top of https://docs.python.org/3/library/array.html)

        array_type = ""
        if args.input_bits == 8:
            array_type = "b"
        elif args.input_bits == 16:
            array_type = "h"
        elif args.input_bits == 32:
            array_type = "f" if args.input_encoding == "float" else "i"

        valuesfromfile = array.array(array_type)
        try:
            valuesfromfile.fromfile(fin, num_input_values)
        except EOFError:
            pass
        in_values = valuesfromfile.tolist()

        tablename = (
            args.table_name
            if args.table_name is not None
            else output_path.stem.replace("-", "_").upper()
        )

        fout.write(f"#ifndef {tablename}_H_\n")
        fout.write(f"#define {tablename}_H_\n\n")
        fout.write("#include <Arduino.h>\n")
        fout.write('#include "mozzi_pgmspace.h"\n\n')
        fout.write(f"#define {tablename}_NUM_CELLS {len(in_values)}\n")
        fout.write(f"#define {tablename}_SAMPLERATE {args.sample_rate}\n\n")

        table = f"CONSTTABLE_STORAGE(int{args.output_bits}_t) {tablename}_DATA [] = {{"
        max_output_value = 2 << (args.output_bits - 2)  # Halved because signed
        out_values = []
        for value in in_values:
            cnt_33 = 0
            out_values.append(
                math.trunc((value * max_output_value) + 0.5)
                if args.input_encoding == "float"
                else value
            )
            # Mega2560 boards won't upload if there is 33, 33, 33 in the array, so dither the 3rd 33 if there is one
            if value == 33:
                cnt_33 += 1
                if cnt_33 == 3:
                    out_values.append(random.choice((32, 34)))
                    cnt_33 = 0
            else:
                cnt_33 = 0
        if args.make_symmetrical:
            value_to_remove = -max_output_value - 1
            if value_to_remove in out_values:
                in_min = value_to_remove
                in_max = max_output_value
                out_max = max_output_value
                out_min = -max_output_value
                out_values = (((value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min) for value in out_values)

        table += ", ".join(map(str, out_values))
        table += "};"
        table = textwrap.fill(table, 80)
        fout.write(table)
        fout.write("\n\n")
        fout.write(f"#endif /* {tablename}_H_ */\n")
        print(f"wrote {table} to {output_path}")

    return 0


if __name__ == "__main__":
    parser = ArgumentParser(
        description="Script for converting a raw audio file to a Mozzi table",
    )
    parser.add_argument(
        "-e",
        "--input-encoding",
        choices=("float", "int"),
        default="int",
        help="Input encoding",
    )
    parser.add_argument(
        "--input-bits",
        type=int,
        choices=(8, 16, 32),
        default=8,
        help="Number of bits for the INPUT encoding",
    )
    parser.add_argument(
        "--output-bits",
        type=int,
        choices=(8, 16, 32),
        default=8,
        help="Number of bits for each element of the OUTPUT table",
    )
    parser.add_argument("input_file", type=Path, help="Path to the input file")
    parser.add_argument(
        "-o",
        "--output-file",
        type=Path,
        help="Path to the output file. It will be input_file.h if not provided",
    )
    parser.add_argument(
        "-t",
        "--table-name",
        type=str,
        help="Name of the output table. If not provided, the name of the output will be used",
    )
    parser.add_argument(
        "-s",
        "--sample-rate",
        type=int,
        default=16384,
        help="Sample rate. Value of 16384 recommended",
    )
    parser.add_argument(
        "--make-symmetrical",
        action="store_true",
        help="Normalize the output between the range +/- max",
    )

    args_ = parser.parse_args()
    sys.exit(float2mozzi(args_))
