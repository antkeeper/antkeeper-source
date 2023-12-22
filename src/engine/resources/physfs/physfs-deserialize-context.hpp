// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_RESOURCES_PHYSFS_DESERIALIZE_CONTEXT_HPP
#define ANTKEEPER_RESOURCES_PHYSFS_DESERIALIZE_CONTEXT_HPP

#include <engine/resources/deserialize-context.hpp>
#include <physfs.h>
#include <filesystem>

/**
 * Deserialize context implementation using PhysicsFS.
 */
class physfs_deserialize_context: public deserialize_context
{
public:
	/**
	 * Constructs a PhysicsFS deserialize context, opening a file using PhysicsFS and associating it with this deserialize context.
	 *
	 * @param path Path to a file to open.
	 *
	 * @throw deserialize_error File open error.
	 */
	explicit physfs_deserialize_context(const std::filesystem::path& path) noexcept(false);
	
	/**
	 * Constructs a PhysicsFS deserialize context.
	 */
	physfs_deserialize_context() noexcept = default;
	
	/**
	 * Destructs a PhysicsFS deserialize context, internally closing a file using PhysicsFS.
	 */
	~physfs_deserialize_context() override;
	
	/**
	 * Opens a file using PhysicsFS and associates it with the deserialize context.
	 *
	 * @param path Path to a file to open.
	 *
	 * @throw deserialize_error File open error.
	 */
	void open(const std::filesystem::path& path) noexcept(false);
	
	/**
	 * Closes the associated file using PhysicsFS.
	 */
	void close() noexcept;
	
	/**
	 * Returns `true` if the PhysicsFS file associated with this deserialize context is open, `false` otherwise.
	 */
	[[nodiscard]] bool is_open() const noexcept;
	
	[[nodiscard]] const std::filesystem::path& path() const noexcept override;
	[[nodiscard]] bool error() const noexcept override;
	[[nodiscard]] bool eof() const noexcept override;
	[[nodiscard]] std::size_t size() const noexcept override;
	[[nodiscard]] std::size_t tell() const override;
	void seek(std::size_t offset) override;
	std::size_t read8(std::byte* data, std::size_t count) noexcept(false) override;
	std::size_t read16_le(std::byte* data, std::size_t count) noexcept(false) override;
	std::size_t read16_be(std::byte* data, std::size_t count) noexcept(false) override;
	std::size_t read32_le(std::byte* data, std::size_t count) noexcept(false) override;
	std::size_t read32_be(std::byte* data, std::size_t count) noexcept(false) override;
	std::size_t read64_le(std::byte* data, std::size_t count) noexcept(false) override;
	std::size_t read64_be(std::byte* data, std::size_t count) noexcept(false) override;
	
private:
	PHYSFS_File* m_file{nullptr};
	std::filesystem::path m_path;
	bool m_eof{true};
	bool m_error{false};
};

#endif // ANTKEEPER_RESOURCES_PHYSFS_DESERIALIZE_CONTEXT_HPP
