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

#ifndef ANTKEEPER_RESOURCES_SERIALIZER_HPP
#define ANTKEEPER_RESOURCES_SERIALIZER_HPP

#include "resources/serialize-context.hpp"

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
