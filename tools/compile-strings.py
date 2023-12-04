# SPDX-FileCopyrightText: 2023 C. J. Howard
# SPDX-License-Identifier: GPL-3.0-or-later

'''
Builds a hashed string map for a single column in a string table.

usage: python compile-strings.py <input file> <language tag> <output file>
'''

import csv
import struct
import sys

input_file_path = sys.argv[1]
value_column_name = sys.argv[2]
output_file_path = sys.argv[3]

# 32-bit FNV-1a hash function.
def fnv1a32(data):
	prime = 16777619
	offset = 2166136261
	for byte in data:
		offset = (offset ^ byte)
		offset = (offset * prime) % (2 ** 32)
	return offset

# Read string table from CSV file
table_rows = []
with open(input_file_path, 'r', encoding="utf8") as csvfile:
	csvreader = csv.reader(csvfile, delimiter=',')
	for row in csvreader:
		table_rows.append(row)

# Find index of column containing string keys
key_column_index = None
for i in range(0, len(table_rows[0])):
	if table_rows[0][i] == "key":
		key_column_index = i
		break
if key_column_index is None:
	raise KeyError("Key column not found")

# Find index of column containing string values
value_column_index = None
for i in range(0, len(table_rows[0])):
	if table_rows[0][i] == value_column_name:
		value_column_index = i
		break
if value_column_index is None:
	raise ValueError("Value column not found")

# Build string map
strings = {}
for i in range(1, len(table_rows)):

	# Get string key and value
	key = table_rows[i][key_column_index]
	value = table_rows[i][value_column_index]
	
	# Ignore empty keys
	if not key or not value:
		continue
    
    # Set empty values to $key
    if not value:
        value = '$' + key
	
	# Hash string key
	key_hash = fnv1a32(key.encode('utf-8'))
	
	# Map hashed string key to string value
	strings[key_hash] = value.encode('utf-8')

# Write string map to file
with open(output_file_path, "wb") as file:
	
	# Write number of strings
	file.write(struct.pack('<L', len(strings)))
	
	# Write strings
	for key, value in strings.items():
	
		# Write string key and string length
		file.write(struct.pack('<2L', key, len(value)))
		
		# Write string value
		file.write(value)
