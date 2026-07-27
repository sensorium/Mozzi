#!/usr/bin/env python3
"""
audio2mozzi.py

This file is part of Mozzi.

Copyright 2024 Leonardo Bellettini and the Mozzi Team.

Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.

Script for converting raw audio files to Mozzi table

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
import logging
import random
import sys
import textwrap
from argparse import ArgumentParser
from pathlib import Path

import numpy as np

logger = logging.getLogger(__name__)

def map_value(value, in_min, in_max, out_min, out_max):
    return ((value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)

def replace_three_33(arr):
    """Mega2560 boards won't upload if there is 33, 33, 33 in the array,
    so dither the 3rd 33 if there is one
    """
    for i in range(len(arr) - 2):  # Ensure we don't go out of bounds
        # Check for three consecutive "33"
        if arr[i] == 33 and arr[i + 1] == 33 and arr[i + 2] == 33:
            arr[i] = random.choice((32, 34))  # Replace the first "33" in the group
    return arr

def float2mozzi(args):
    input_path = args.input_file.expanduser()
    output_path = (
        args.output_file.expanduser()
        if args.output_file is not None
        else input_path.with_suffix(".h")
    )

    with input_path.open("rb") as fin, output_path.open("w") as fout:
        logger.debug("Opened %s", input_path)
        num_input_values = int(
            input_path.stat().st_size / (args.input_bits / 8),
        )  # Adjust for number format (table at top of https://docs.python.org/3/library/array.html)

        array_type = ""
        input_dtype = None
        if args.input_bits == 8:
            array_type = "b"
            input_dtype = np.int8
        elif args.input_bits == 16:
            array_type = "h"
            input_dtype = np.int16 if args.input_encoding == "int" else np.float16
        elif args.input_bits == 32:
            input_dtype = np.int32 if args.input_encoding == "int" else np.float32
            array_type = "f" if args.input_encoding == "float" else "i"
        else:
            raise Exception("Unrecognised --input-bits value")

        valuesfromfile = array.array(array_type)
        try:
            valuesfromfile.fromfile(fin, num_input_values)
        except EOFError:
            pass
        in_values = valuesfromfile.tolist()
        input_array = np.array(in_values, dtype=input_dtype)

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

        output_dtype = None
        if args.output_bits == 8:
            output_dtype = np.int8
        elif args.output_bits == 16:
            output_dtype = np.int16
        elif args.output_bits == 32:
            output_dtype = np.int32
        else:
            raise Exception("Unrecognised --input-bits value")

        output_array = input_array.astype(output_dtype)
        replace_three_33(output_array)
        if args.make_symmetrical:
            min_value_to_normalize = np.iinfo(output_dtype).min
            max_value_to_normalize = np.iinfo(output_dtype).max
            min_final_value = np.iinfo(output_dtype).min + 1
            max_final_value = max_value_to_normalize
            if min_value_to_normalize in output_array:
                output_array = (
                    map_value(
                        value,
                        min_value_to_normalize,
                        max_value_to_normalize,
                        min_final_value,
                        max_final_value,
                    )
                    for value in output_array
                )

        table += ", ".join(map(str, output_array))
        table += "};"
        table = textwrap.fill(table, 80)
        fout.write(table)
        fout.write("\n\n")
        fout.write(f"#endif /* {tablename}_H_ */\n")
        logger.debug("Wrote %s to %s", table, output_path)

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
    parser.add_argument(
        "-v",
        "--verbose",
        action="store_true",
        help="Increase verbosity",
    )

    args_ = parser.parse_args()
    if args_.verbose:
        logging.basicConfig(level=logging.DEBUG)

    sys.exit(float2mozzi(args_))
