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

#ifndef ANTKEEPER_RESOURCES_RESOURCE_LOADER_HPP
#define ANTKEEPER_RESOURCES_RESOURCE_LOADER_HPP

#include <engine/resources/deserialize-context.hpp>
#include <memory>

class resource_manager;

/**
 * Templated resource loader.
 *
 * @tparam T Resource type.
 */
template <typename T>
class resource_loader
{
public:
	/**
	 * Loads a resource.
	 *
	 * @param ctx Deserialize context.
	 * @param resource_manager Resource manager to load resource dependencies.
	 *
	 * @return Unique pointer to the loaded resource.
	 */
	[[nodiscard]] static std::unique_ptr<T> load(::resource_manager& resource_manager, deserialize_context& ctx);
};

#endif // ANTKEEPER_RESOURCES_RESOURCE_LOADER_HPP
