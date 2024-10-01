// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_RESOURCES_PHYSFS_SERIALIZE_CONTEXT_HPP
#define ANTKEEPER_RESOURCES_PHYSFS_SERIALIZE_CONTEXT_HPP

#include <engine/resources/serialize-context.hpp>
#include <physfs.h>
#include <filesystem>

/// Provides access to a serialization state.
class physfs_serialize_context: public serialize_context
{
public:
	/// Constructs a PhysicsFS serialize context, opening a file using PhysicsFS and associating it with this serialize context.
	/// @param path Path to a file to open.
	/// @throw serialize_error File open error.
	explicit physfs_serialize_context(const std::filesystem::path& path) noexcept(false);
	
	/// Constructs a PhysicsFS serialize context.
	physfs_serialize_context() noexcept = default;
	
	/// Destructs a PhysicsFS serialize context, internally closing a file using PhysicsFS.
	~physfs_serialize_context() override;
	
	/// Opens a file using PhysicsFS and associates it with the serialize context.
	/// @param path Path to a file to open.
	/// @throw serialize_error File open error.
	void open(const std::filesystem::path& path) noexcept(false);
	
	/// Closes the associated file using PhysicsFS.
	void close() noexcept;
	
	/// Returns `true` if the PhysicsFS file associated with this serialize context is open, `false` otherwise.
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
