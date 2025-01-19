// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GENETICS_STANDARD_CODE_HPP
#define ANTKEEPER_GENETICS_STANDARD_CODE_HPP

#include <engine/genetics/codon.hpp>

namespace genetics {

/// Codon table for standard genetic code.
/// @see https://www.ncbi.nlm.nih.gov/Taxonomy/Utils/wprintgc.cgi#SG1
constexpr codon::table standard_code =
{
	"FFLLSSSSYY**CC*WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
	"---M------**--*----M---------------M----------------------------",
};

} // namespace genetics

#endif // ANTKEEPER_GENETICS_STANDARD_CODE_HPP
