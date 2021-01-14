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

#include "morton.hpp"

namespace geom {
namespace morton {

std::uint32_t encode_2d(std::uint32_t x, std::uint32_t y)
{
	auto interleave = [](std::uint32_t x) -> std::uint32_t
	{
		x &= 0x0000ffff;
		x = (x ^ (x <<  8)) & 0x00ff00ff;
		x = (x ^ (x <<  4)) & 0x0f0f0f0f;
		x = (x ^ (x <<  2)) & 0x33333333;
		x = (x ^ (x <<  1)) & 0x55555555;
		return x;
	};
	
	return (interleave(y) << 1) + interleave(x);
}

std::uint32_t encode_3d(std::uint32_t x, std::uint32_t y, std::uint32_t z)
{
	auto interleave = [](std::uint32_t x) -> std::uint32_t
	{
		x &= 0x000003ff;
		x = (x ^ (x << 16)) & 0xff0000ff;
		x = (x ^ (x <<  8)) & 0x0300f00f;
		x = (x ^ (x <<  4)) & 0x030c30c3;
		x = (x ^ (x <<  2)) & 0x09249249;
		return x;
	};

	return interleave(x) | (interleave(y) << 1) | (interleave(z) << 2);
}

std::array<uint32_t, 2> decode_2d(std::uint32_t code)
{
	auto unravel = [](std::uint32_t x) -> std::uint32_t
	{
		x &= 0x55555555;
		x = (x ^ (x >>  1)) & 0x33333333;
		x = (x ^ (x >>  2)) & 0x0f0f0f0f;
		x = (x ^ (x >>  4)) & 0x00ff00ff;
		x = (x ^ (x >>  8)) & 0x0000ffff;
		return x;
	};

	return {unravel(code >> 0), unravel(code >> 1)};
}

std::array<uint32_t, 3> decode_3d(std::uint32_t code)
{
	auto unravel = [](std::uint32_t x) -> std::uint32_t
	{
		x &= 0x09249249;
		x = (x ^ (x >>  2)) & 0x030c30c3;
		x = (x ^ (x >>  4)) & 0x0300f00f;
		x = (x ^ (x >>  8)) & 0xff0000ff;
		x = (x ^ (x >> 16)) & 0x000003ff;
		return x;
	};

	return {unravel(code >> 0), unravel(code >> 1), unravel(code >> 2)};
}

} // namespace morton
} // namespace geom
