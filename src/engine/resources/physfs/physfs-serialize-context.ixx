// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.resources.physfs_serialize_context;
import <physfs.h>;
export import engine.resources.serialize_context;
import engine.utility.sized_types;
export import <filesystem>;

export namespace engine::resources
{
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
		usize write8(const std::byte* data, usize count) noexcept(false) override;
		usize write16_le(const std::byte* data, usize count) noexcept(false) override;
		usize write16_be(const std::byte* data, usize count) noexcept(false) override;
		usize write32_le(const std::byte* data, usize count) noexcept(false) override;
		usize write32_be(const std::byte* data, usize count) noexcept(false) override;
		usize write64_le(const std::byte* data, usize count) noexcept(false) override;
		usize write64_be(const std::byte* data, usize count) noexcept(false) override;

	private:
		PHYSFS_File* file{nullptr};
		std::filesystem::path m_path;
		bool m_error{false};
	};
}
