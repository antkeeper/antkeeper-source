/*
 * Copyright (C) 2021  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANTKEEPER_GENETICS_CODON_HPP
#define ANTKEEPER_GENETICS_CODON_HPP

namespace genetics {
namespace codon {

/**
 * Returns `true` if a codon is a start codon.
 *
 * @param base1 IUPAC base code of first nucleobase, either `U`, `T`, `C`, `A`, or `G`.
 * @param base2 IUPAC base code of second nucleobase, either `U`, `T`, `C`, `A`, or `G`.
 * @param base3 IUPAC base code of third nucleobase, either `U`, `T`, `C`, `A`, or `G`.
 * @param starts String of 64 IUPAC amino acid codes, in TCAG order.
 * @return `true` if the codon is a start codon, `false` otherwise.
 */
bool is_start(char base1, char base2, char base3, const char* starts);

/**
 * Returns `true` if a codon is a stop codon.
 *
 * @param base1 IUPAC base code of first nucleobase, either `U`, `T`, `C`, `A`, or `G`.
 * @param base2 IUPAC base code of second nucleobase, either `U`, `T`, `C`, `A`, or `G`.
 * @param base3 IUPAC base code of third nucleobase, either `U`, `T`, `C`, `A`, or `G`.
 * @param aas String of 64 IUPAC amino acid codes, in TCAG order.
 * @return `true` if the codon is a stop codon, `false` otherwise.
 */
bool is_stop(char base1, char base2, char base3, const char* aas);

/**
 * Translates a codon into an amino acid.
 *
 * @param base1 IUPAC base code of first nucleobase, either `U`, `T`, `C`, `A`, or `G`.
 * @param base2 IUPAC base code of second nucleobase, either `U`, `T`, `C`, `A`, or `G`.
 * @param base3 IUPAC base code of third nucleobase, either `U`, `T`, `C`, `A`, or `G`.
 * @param aas String of 64 IUPAC amino acid codes, in TCAG order.
 * @return IUPAC amino acid code of corresponding amino acid, or `-` if an invalid codon was supplied.
 */
char translate(char base1, char base2, char base3, const char* aas);

} // namspace codon
} // namespace genetics

#endif // ANTKEEPER_GENETICS_CODON_HPP
