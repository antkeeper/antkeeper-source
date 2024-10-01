# SPDX-FileCopyrightText: 2024 C. J. Howard
# SPDX-License-Identifier: GPL-3.0-or-later

import argparse
import csv
import json
import sys

if __name__ == "__main__":
    
    # Parse arguments
    parser = argparse.ArgumentParser(description='Export JSON strings for a given language.')
    parser.add_argument('input_file', help='Input file')
    parser.add_argument('language_tag', help='Language tag')
    parser.add_argument('output_file', help='Output file')
    args = parser.parse_args()
    
    # Build string dict for the given language
    with open(args.input_file, 'r', encoding='utf-8') as file:
    
        csv_reader = csv.DictReader(file)
        header = next(csv_reader, None)
        
        if not args.language_tag in header:
            print(f"error: language \"{args.language_tag}\" not found in \"{args.input_file}\"")
            sys.exit(1)
        
        strings = {row['key']: row[args.language_tag] or None for row in csv_reader if row['key']}
    
    # Export string dict as JSON
    with open(args.output_file, 'w', encoding='utf-8') as file:
        json.dump(strings, file, indent='\t', ensure_ascii=False)
    
