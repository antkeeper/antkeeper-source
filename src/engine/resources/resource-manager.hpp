// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_RESOURCES_RESOURCE_MANAGER_HPP
#define ANTKEEPER_RESOURCES_RESOURCE_MANAGER_HPP

#include <engine/debug/log.hpp>
#include <engine/resources/deserialize-context.hpp>
#include <engine/resources/deserializer.hpp>
#include <engine/resources/serialize-context.hpp>
#include <engine/resources/serializer.hpp>
#include <engine/resources/resource-loader.hpp>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <unordered_map>

/**
 * Manages the loading, caching, and saving of resources.
 */
class resource_manager
{
public:
	/**
	 * Constructs a resource manager.
	 *
	 * @throw std::runtime_error Failed to initialize PhysicsFS.
	 */
	resource_manager();
	
	/**
	 * Destructs a resource manager.
	 */
	~resource_manager();
	
	/**
	 * Adds a directory or archive to the search path.
	 *
	 * @param path Path to the directory or archive to mount.
	 *
	 * @return `true` if the directory or archive was successfully mounted, `false` otherwise.
	 */
	bool mount(const std::filesystem::path& path);
	
	/**
	 * Removes a directory or archive from the search path.
	 *
	 * @param path Path to the directory or archive to unmount.
	 *
	 * @return `true` if the directory or archive was successfully unmounted, `false` otherwise.
	 */
	bool unmount(const std::filesystem::path& path);
	
	/**
	 * Loads and caches a resource. If the resource has already been loaded the cached resource will be returned.
	 *
	 * @tparam T Resource type.
	 *
	 * @param path Path to the resource to load.
	 *
	 * @return Pointer to the loaded resource, or `nullptr` if the resource could not be loaded.
	 */
	template <class T>
	std::shared_ptr<T> load(const std::filesystem::path& path);
	
	/**
	 * Saves a resource to a file.
	 *
	 * @tparam T Resource type.
	 *
	 * @param resource Resource to save.
	 * @param path Path to where the resource file should be written.
	 *
	 * @return `true` if the resource was successfully saved, `false` otherwise.
	 */
	template <class T>
	bool save(const T& resource, const std::filesystem::path& path) const;
	
	/**
	 * Sets the path to a directory or archive where files can be written.
	 *
	 * @param path Path to the directory or archive to which files should be written.
	 *
	 * @return `true` if the write path was set successfully, `false` otherwise.
	 */
	bool set_write_path(const std::filesystem::path& path);
	
	/**
	 * Returns the path to the directory or archive to which files are written.
	 */
	[[nodiscard]] inline const std::filesystem::path& get_write_path() const noexcept
	{
		return write_path;
	}

private:
	/**
	 * Fetches a resource from the resource cache.
	 *
	 * @param path Path to a resource.
	 *
	 * @return Shared pointer to the cached resource, or `nullptr` if the resource was not found or has expired.
	 */
	[[nodiscard]] std::shared_ptr<void> fetch(const std::filesystem::path& path) const;
	
	/**
	 * Constructs a deserialize context from a file path.
	 *
	 * @param path Path to the file to open for reading.
	 *
	 * @return Unique pointer to a deserialize context, or `nullptr` if the file could not be opened for reading.
	 */
	[[nodiscard]] std::unique_ptr<deserialize_context> open_read(const std::filesystem::path& path) const;
	
	/**
	 * Constructs a serialize context from a file path.
	 *
	 * @param path Path to a file to open for writing.
	 *
	 * @return Unique pointer to a serialize context, or `nullptr` if the file could not be opened for writing.
	 */
	[[nodiscard]] std::unique_ptr<serialize_context> open_write(const std::filesystem::path& path) const;
	
	std::unordered_map<std::filesystem::path, std::weak_ptr<void>> resource_cache;
	std::filesystem::path write_path;
};

template <class T>
std::shared_ptr<T> resource_manager::load(const std::filesystem::path& path)
{
	// Fetch cached resource, if any
	if (auto resource = fetch(path))
	{
		return std::static_pointer_cast<T>(resource);
	}
	
	const auto path_string = path.string();
	
	try
	{
		debug::log_trace("Loading resource \"{}\"...", path_string);
		
		// Open file for reading
		auto deserialize_ctx = open_read(path);
		
		// Load and cache resource
		std::shared_ptr<T> resource = resource_loader<T>::load(*this, *deserialize_ctx);
		resource_cache[path] = resource;
		
		debug::log_trace("Loaded resource \"{}\"", path_string);
		
		return resource;
	}
	catch (const std::exception& e)
	{
		debug::log_error("Failed to load resource \"{}\": {}", path_string, e.what());
	}
	
	return nullptr;
}

template <class T>
bool resource_manager::save(const T& resource, const std::filesystem::path& path) const
{
	const auto path_string = path.string();
	
	try
	{
		debug::log_trace("Saving resource to \"{}\"...", path_string);
		
		// Open file for writing
		auto serialize_ctx = open_write(path);
		
		serializer<T>().serialize(resource, *serialize_ctx);
		
		debug::log_trace("Saved resource to \"{}\"", path_string);
		
		return true;
	}
	catch (const std::exception& e)
	{
		debug::log_error("Failed to save resource to \"{}\": {}", path_string, e.what());
	}
	
	return false;
}

#endif // ANTKEEPER_RESOURCES_RESOURCE_MANAGER_HPP
