# SPDX-FileCopyrightText: 2024 C. J. Howard
# SPDX-License-Identifier: GPL-3.0-or-later

'''
Displays localization coverage for a given locale.

usage: python locale-coverage.py <strings file> <language tag>
'''

import csv
import sys

input_file_path = sys.argv[1]
language_tag = sys.argv[2]

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

# Find index of column containing English language
source_column_index = None
for i in range(0, len(table_rows[0])):
	if table_rows[0][i] == "en":
		source_column_index = i
		break
if source_column_index is None:
	raise KeyError("Source language not found")

# Find index of column containing target language
target_column_index = None
for i in range(0, len(table_rows[0])):
	if table_rows[0][i] == language_tag:
		target_column_index = i
		break
if target_column_index is None:
	raise KeyError("Target language not found")

# Calculate coverage
localized_strings = 0
total_strings = 0
for i in range(1, len(table_rows)):

	key = table_rows[i][key_column_index]
	source = table_rows[i][source_column_index]
	target = table_rows[i][target_column_index]
	
	# Ignore empty strings in the source language
	if not key or not source:
		continue
	
	total_strings += 1
	if target:
		localized_strings += 1

print("%.0f" % (localized_strings / total_strings * 100), end='')
