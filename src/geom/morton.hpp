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

#ifndef ANTKEEPER_GEOM_MORTON_HPP
#define ANTKEEPER_GEOM_MORTON_HPP

#include <array>
#include <cstdint>

namespace geom {

/// Functions which encode or decode Morton location codes.
namespace morton {

/// Encodes 2D coordinates as a 32-bit Morton location code.
std::uint32_t encode_2d(std::uint32_t x, std::uint32_t y);

/// Encodes 3D coordinates as a 32-bit Morton location code.
std::uint32_t encode_3d(std::uint32_t x, std::uint32_t y, std::uint32_t z);

/// Decodes X and Y coordinates from a 32-bit Morton location code.
std::array<uint32_t, 2> decode_2d(std::uint32_t code);

/// Decodes X, Y, and Z coordinates from a 32-bit Morton location code.
std::array<uint32_t, 3> decode_3d(std::uint32_t code);

} // namespace morton
} // namespace geom

#endif // ANTKEEPER_GEOM_MORTON_HPP

