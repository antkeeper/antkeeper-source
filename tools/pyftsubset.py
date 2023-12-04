# SPDX-FileCopyrightText: 2023 C. J. Howard
# SPDX-License-Identifier: GPL-3.0-or-later

# Wrapper for pyftsubset from the fonttools package
# requirements: fonttools brotli
from fontTools.subset import main as subset
subset()
