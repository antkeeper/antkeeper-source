# SPDX-FileCopyrightText: 2025 C. J. Howard
# SPDX-License-Identifier: GPL-3.0-or-later

import argparse
import json
import os
from pathlib import Path

if __name__ == "__main__":
    
    # Parse arguments
    parser = argparse.ArgumentParser(description='Generate a language manifest from a directory containing language files.')
    parser.add_argument('input_directory', help='Input directory')
    parser.add_argument('output_file', help='Output file')
    args = parser.parse_args()
    
    # Read languages into array
    languages = {}
    for filename in os.listdir(args.input_directory):
        if filename.endswith(".json"):
            file_path = os.path.join(args.input_directory, filename)
            with open(file_path, 'r', encoding='utf-8') as file:
                json_data = ''.join(line for line in file if not line.startswith('//'))
                languages[Path(file_path).stem] = json.loads(json_data)
    
    # Write language manifest
    with open(args.output_file, 'w', encoding='utf-8') as file:
        json.dump(languages, file, indent='\t', ensure_ascii=False)

