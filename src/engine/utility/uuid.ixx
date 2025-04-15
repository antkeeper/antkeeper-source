// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.utility.uuid;
export import <array>;
export import <cstddef>;
export import <string>;
import <ostream>;

export namespace engine
{
	/// 128-bit universally unique identifier (UUID).
	struct uuid
	{
		/// Returns a string representation of the UUID.
		[[nodiscard]] std::string string() const;

		/// UUID data.
		std::array<std::byte, 16> data{};
	};
}

/// Writes a UUID to an output stream.
/// @param os Output stream.
/// @param id UUID.
/// @return Output stream.
export std::ostream& operator<<(std::ostream& os, const engine::uuid& id);
