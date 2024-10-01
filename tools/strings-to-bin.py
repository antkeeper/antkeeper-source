# SPDX-FileCopyrightText: 2024 C. J. Howard
# SPDX-License-Identifier: GPL-3.0-or-later

import argparse
import csv
import struct
from functools import reduce

# 32-bit FNV-1a hash function.
def fnv1a32(data):
    return reduce(lambda h, b: (h ^ b) * 16777619 & 0xffffffff, data, 2166136261)

if __name__ == "__main__":
    
    # Parse arguments
    parser = argparse.ArgumentParser(description='Generate a binary string map from a CSV file for the given language.')
    parser.add_argument('input_file', help='Input file')
    parser.add_argument('language_tag', help='Language tag')
    parser.add_argument('output_file', help='Output file')
    args = parser.parse_args()
    
    # Build string dict, setting empty values to $key
    with open(args.input_file, 'r', encoding='utf-8') as file:
        strings = {row['key']: row[args.language_tag] for row in csv.DictReader(file) if row['key']}
    strings = {k: '$' + k if not v else v for k, v in strings.items()}
    
    # Hash string keys, encode string values in UTF-8, calculate sizes of UTF-8 string values
    keys = [fnv1a32(k.encode('utf-8')) for k in strings.keys()]
    values = [v.encode('utf-8') for v in strings.values()]
    sizes = [len(v) for v in values]
    
    # Pack UTF-8 string values into bytestring
    values = b''.join(values)
    
    # Generate output file
    with open(args.output_file, 'wb') as file:
        file.write(struct.pack('<L', len(values)) + values + struct.pack('<L', len(keys)))
        for key, size in zip(keys, sizes):
            file.write(struct.pack('<2L', key, size))
    
