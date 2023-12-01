// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_RESOURCES_SERIALIZER_HPP
#define ANTKEEPER_RESOURCES_SERIALIZER_HPP

#include <engine/resources/serialize-context.hpp>

/**
 * Specializations of serializer define the serialization process for a given type.
 *
 * @tparam T Serializable type.
 */
template <class T>
struct serializer
{
	/**
	 * Serializes a value.
	 *
	 * @param value Value to serialize.
	 * @param ctx Serialize context.
	 */
	void serialize(const T& value, serialize_context& ctx);
};

#endif // ANTKEEPER_RESOURCES_SERIALIZER_HPP
