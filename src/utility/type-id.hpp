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

#ifndef ANTKEEPER_UTILITY_TYPE_ID_HPP
#define ANTKEEPER_UTILITY_TYPE_ID_HPP

/**
 * Provides unique type IDs at compile-time via the templated addresses of this function.
 *
 * @tparam T Type.
 */
template <class T>
void type_id_generator() noexcept {}

/// Type ID type.
using type_id_t = void(*)(void);

/**
 * Compile-time constant type ID.
 *
 * @tparam T Type.
 */
template <class T>
constexpr type_id_t type_id = &type_id_generator<T>;

#endif // ANTKEEPER_UTILITY_TYPE_ID_HPP
