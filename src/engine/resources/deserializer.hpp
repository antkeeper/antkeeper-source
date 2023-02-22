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

#ifndef ANTKEEPER_RESOURCES_DESERIALIZER_HPP
#define ANTKEEPER_RESOURCES_DESERIALIZER_HPP

#include <engine/resources/deserialize-context.hpp>

/**
 * Specializations of deserializer define the deserialization process for a given type.
 *
 * @tparam T Deserializable type.
 */
template <class T>
struct deserializer
{
	/**
	 * Deserializes a value.
	 *
	 * @param value Value to deserialize.
	 * @param ctx Deserialize context.
	 */
	void deserialize(T& value, deserialize_context& ctx);
};

#endif // ANTKEEPER_RESOURCES_DESERIALIZER_HPP
