// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.resources.deserialize_error;
export import <stdexcept>;

export namespace engine::resources
{
	/// An exception of this type is thrown when an error occurs during deserialization.
	class deserialize_error: public std::runtime_error
	{
		using std::runtime_error::runtime_error;
	};
}
