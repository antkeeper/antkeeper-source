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

#ifndef ANTKEEPER_PHYSICS_GAS_OZONE_HPP
#define ANTKEEPER_PHYSICS_GAS_OZONE_HPP

namespace physics {
namespace gas {
	
/// Ozone-related constants and functions.
namespace ozone {

/**
 * Cross section of ozone at wavelength 680nm and temperature 293K, in m-2/molecule.
 *
 * @see https://www.iup.uni-bremen.de/gruppen/molspec/databases/referencespectra/o3spectra2011/index.html
 */
template <class T>
constexpr T cross_section_680nm_293k = T(1.36820899679147e-25);

/**
 * Cross section of ozone at wavelength 550nm and temperature 293K, in m-2/molecule.
 *
 * @see https://www.iup.uni-bremen.de/gruppen/molspec/databases/referencespectra/o3spectra2011/index.html
 */
template <class T>
constexpr T cross_section_550nm_293k = T(3.31405330400124e-25);

/**
 * Cross section of ozone at wavelength 550nm and temperature 293K, in m-2/molecule.
 *
 * @see https://www.iup.uni-bremen.de/gruppen/molspec/databases/referencespectra/o3spectra2011/index.html
 */
template <class T>
constexpr T cross_section_440nm_293k = T(1.36017282525383e-26);

/**
 * Calculates an ozone absorption coefficient (wavelength-dependent).
 *
 * @param cross_section Ozone cross section of a wavelength, in m-2/molecule.
 * @param n Molecular number density of standard air, in molecules/m-3.
 * @param c Concentration of ozone in the atmosphere, unitless.
 *
 * @return Ozone absorption coefficient.
 *
 * @see https://skyrenderer.blogspot.com/2012/10/ozone-absorption.html
 */
template <class T>
T absorption(T cross_section, T n, T c)
{
	return cross_section * n * c;
}

} // namespace ozone

} // namespace gas
} // namespace physics

#endif // ANTKEEPER_PHYSICS_GAS_OZONE_HPP
