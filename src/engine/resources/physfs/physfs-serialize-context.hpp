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

#ifndef ANTKEEPER_RESOURCES_PHYSFS_SERIALIZE_CONTEXT_HPP
#define ANTKEEPER_RESOURCES_PHYSFS_SERIALIZE_CONTEXT_HPP

#include <engine/resources/serialize-context.hpp>
#include <physfs.h>
#include <filesystem>

/**
 * Provides access to a serialization state.
 */
class physfs_serialize_context: public serialize_context
{
public:
	/**
	 * Constructs a PhysicsFS serialize context, opening a file using PhysicsFS and associating it with this serialize context.
	 *
	 * @param path Path to a file to open.
	 *
	 * @throw serialize_error File open error.
	 */
	physfs_serialize_context(const std::filesystem::path& path) noexcept(false);
	
	/**
	 * Constructs a PhysicsFS serialize context.
	 */
	physfs_serialize_context() noexcept = default;
	
	/**
	 * Destructs a PhysicsFS serialize context, internally closing a file using PhysicsFS.
	 */
	virtual ~physfs_serialize_context();
	
	/**
	 * Opens a file using PhysicsFS and associates it with the serialize context.
	 *
	 * @param path Path to a file to open.
	 *
	 * @throw serialize_error File open error.
	 */
	void open(const std::filesystem::path& path) noexcept(false);
	
	/**
	 * Closes the associated file using PhysicsFS.
	 */
	void close() noexcept;
	
	/**
	 * Returns `true` if the PhysicsFS file associated with this serialize context is open, `false` otherwise.
	 */
	[[nodiscard]] bool is_open() const noexcept;
	
	[[nodiscard]] const std::filesystem::path& path() const noexcept override;
	[[nodiscard]] bool error() const noexcept override;
	std::size_t write8(const std::byte* data, std::size_t count) noexcept(false) override;
	std::size_t write16_le(const std::byte* data, std::size_t count) noexcept(false) override;
	std::size_t write16_be(const std::byte* data, std::size_t count) noexcept(false) override;
	std::size_t write32_le(const std::byte* data, std::size_t count) noexcept(false) override;
	std::size_t write32_be(const std::byte* data, std::size_t count) noexcept(false) override;
	std::size_t write64_le(const std::byte* data, std::size_t count) noexcept(false) override;
	std::size_t write64_be(const std::byte* data, std::size_t count) noexcept(false) override;
	
private:
	PHYSFS_File* file{nullptr};
	std::filesystem::path m_path;
	bool m_error{false};
};

#endif // ANTKEEPER_RESOURCES_PHYSFS_SERIALIZE_CONTEXT_HPP
