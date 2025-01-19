// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GENETICS_CODON_HPP
#define ANTKEEPER_GENETICS_CODON_HPP

namespace genetics {

/// Functions and structures related to triplets of IUPAC base symbols.
namespace codon {

/// Table for translating codons to amino acids.
/// @see https://www.ncbi.nlm.nih.gov/Taxonomy/Utils/wprintgc.cgi
struct table
{
	/// String of 64 IUPAC amino acid base symbols, in TCAG order.
	const char* aas;
	
	/// String of 64 IUPAC amino acid base symbols, in TCAG order, where symbols other than `-` and `*` indicate a start codon and its amino acid.
	const char* starts;
};

/// Returns `true` if a codon is a start codon.
/// @param base1 IUPAC base code of first nucleobase, either `U`, `T`, `C`, `A`, or `G`.
/// @param base2 IUPAC base code of second nucleobase, either `U`, `T`, `C`, `A`, or `G`.
/// @param base3 IUPAC base code of third nucleobase, either `U`, `T`, `C`, `A`, or `G`.
/// @param starts String of 64 IUPAC amino acid codes, in TCAG order.
/// @return `true` if the codon is a start codon, `false` otherwise.
bool is_start(char base1, char base2, char base3, const char* starts);

/// Returns `true` if a codon is a stop codon.
/// @param base1 IUPAC base code of first nucleobase, either `U`, `T`, `C`, `A`, or `G`.
/// @param base2 IUPAC base code of second nucleobase, either `U`, `T`, `C`, `A`, or `G`.
/// @param base3 IUPAC base code of third nucleobase, either `U`, `T`, `C`, `A`, or `G`.
/// @param aas String of 64 IUPAC amino acid codes, in TCAG order.
/// @return `true` if the codon is a stop codon, `false` otherwise.
bool is_stop(char base1, char base2, char base3, const char* aas);

/// Translates a codon into an amino acid.
/// @param base1 IUPAC base code of first nucleobase, either `U`, `T`, `C`, `A`, or `G`.
/// @param base2 IUPAC base code of second nucleobase, either `U`, `T`, `C`, `A`, or `G`.
/// @param base3 IUPAC base code of third nucleobase, either `U`, `T`, `C`, `A`, or `G`.
/// @param aas String of 64 IUPAC amino acid codes, in TCAG order.
/// @return IUPAC amino acid code of corresponding amino acid, or `-` if an invalid codon was supplied.
char translate(char base1, char base2, char base3, const char* aas);

} // namspace codon
} // namespace genetics

#endif // ANTKEEPER_GENETICS_CODON_HPP
