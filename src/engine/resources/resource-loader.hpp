// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_RESOURCES_RESOURCE_LOADER_HPP
#define ANTKEEPER_RESOURCES_RESOURCE_LOADER_HPP

#include <engine/resources/deserialize-context.hpp>
#include <memory>

class resource_manager;

/// Templated resource loader.
/// @tparam T Resource type.
template <typename T>
class resource_loader
{
public:
	/// Loads a resource.
	/// @param ctx Deserialize context.
	/// @param resource_manager Resource manager to load resource dependencies.
	/// @return Unique pointer to the loaded resource.
	[[nodiscard]] static std::unique_ptr<T> load(::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx);
};

#endif // ANTKEEPER_RESOURCES_RESOURCE_LOADER_HPP
