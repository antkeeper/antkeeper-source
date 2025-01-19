# SPDX-FileCopyrightText: 2025 C. J. Howard
# SPDX-License-Identifier: GPL-3.0-or-later

'''
Converts any simplified Chinese characters in the zh-Hant column into traditional Chinese.

requirements: opencc-python-reimplemented

usage: python hans-to-hant.py <strings file>
'''

import csv
import sys
from opencc import OpenCC

cc = OpenCC('s2t')

file_path = sys.argv[1]

# Read string table from CSV file
table_rows = []
with open(file_path, 'r', encoding="utf8") as csvfile:
	csvreader = csv.reader(csvfile, delimiter=',')
	for row in csvreader:
		table_rows.append(row)

# Find index of column containing traditional Chinese
hant_column_index = None
for i in range(0, len(table_rows[0])):
	if table_rows[0][i] == "zh-Hant":
		hant_column_index = i
		break
if hant_column_index is None:
	raise KeyError("zh-Hant column not found")

# Convert simplified Chinese to traditional Chinese
for i in range(1, len(table_rows)):
	table_rows[i][hant_column_index] = cc.convert(table_rows[i][hant_column_index])

# Update CSV file
with open(file_path, 'w', newline='', encoding="utf8") as csvfile:
	csvwriter = csv.writer(csvfile, delimiter=',')
	for row in table_rows:
		csvwriter.writerow(row)
