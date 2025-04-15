// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.resources.resource_loader;
export import engine.resources.deserialize_context;
export import <memory>;

export namespace engine::resources
{
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
		[[nodiscard]] static std::unique_ptr<T> load(resources::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx);
	};
}
