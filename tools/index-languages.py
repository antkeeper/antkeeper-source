# SPDX-FileCopyrightText: 2023 C. J. Howard
# SPDX-License-Identifier: GPL-3.0-or-later

'''
Builds an index of supported language tags.

usage: python index-languages.py <strings file> <output file>
'''

import csv
import sys

input_file_path = sys.argv[1]
output_file_path = sys.argv[2]

# Read string table header from CSV file
header_row = None
with open(input_file_path, 'r', encoding='utf-8') as csvfile:
	csvreader = csv.reader(csvfile, delimiter=',')
	for row in csvreader:
		header_row = row
		break

# Index language tags
language_tags = []
for column in header_row:
	if column and column != "key" and column != "context":
		language_tags.append(column)

# Write index to text file
with open(output_file_path, "w", encoding='utf-8') as file:
	for i in range(0, len(language_tags)):
		if i != 0:
			file.write('\n')
		file.write(language_tags[i])
