// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_UTILITY_UUID_HPP
#define ANTKEEPER_UTILITY_UUID_HPP

#include <array>
#include <cstddef>
#include <ostream>
#include <string>

/// 128-bit universally unique identifier (UUID).
struct uuid
{
	/// Returns a string representation of the UUID.
	[[nodiscard]] std::string string() const;
	
	/// UUID data.
	std::array<std::byte, 16> data{};
};

/// Writes a UUID to an output stream.
/// @param os Output stream.
/// @param id UUID.
/// @return Output stream.
std::ostream& operator<<(std::ostream& os, const uuid& id);

#endif // ANTKEEPER_UTILITY_UUID_HPP
