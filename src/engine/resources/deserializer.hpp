// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_RESOURCES_DESERIALIZER_HPP
#define ANTKEEPER_RESOURCES_DESERIALIZER_HPP

#include <engine/resources/deserialize-context.hpp>

/// Specializations of deserializer define the deserialization process for a given type.
/// @tparam T Deserializable type.
template <class T>
struct deserializer
{
	/// Deserializes a value.
	/// @param value Value to deserialize.
	/// @param ctx Deserialize context.
	void deserialize(T& value, deserialize_context& ctx);
};

#endif // ANTKEEPER_RESOURCES_DESERIALIZER_HPP
