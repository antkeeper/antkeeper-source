/*
 * Copyright (C) 2023  Christopher J. Howard
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

#ifndef ANTKEEPER_UTILITY_UUID_HPP
#define ANTKEEPER_UTILITY_UUID_HPP

#include <array>
#include <cstddef>
#include <ostream>
#include <string>

/**
 * 128-bit universally unique identifier (UUID).
 */
struct uuid
{
	/// Returns a string representation of the UUID.
	[[nodiscard]] std::string string() const;
	
	/// UUID data.
	std::array<std::byte, 16> data{};
};

/**
 * Writes a UUID to an output stream.
 *
 * @param os Output stream.
 * @param id UUID.
 *
 * @return Output stream.
 */
std::ostream& operator<<(std::ostream& os, const uuid& id);

#endif // ANTKEEPER_UTILITY_UUID_HPP
