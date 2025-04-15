// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.ui.image;
export import engine.ui.element;
import engine.scene.billboard;
export import <memory>;

export namespace engine::ui
{
	/// UI image.
	class image: public element
	{
	public:
		/// Constructs an image.
		image();

		/// Destructs an image.
		~image() override = default;

		[[nodiscard]] inline constexpr element_type get_type() const noexcept override
		{
			return element_type::image;
		}

		/// Sets the image material.
		/// @param material Image material.
		void set_material(std::shared_ptr<render::material> material);

		// TODO: set_color() and set_opacity() methods can be added by implementing set_color() in scene::billboard.
		// The billboard can set the vertex colors rather than a shader uniform.

	protected:
		void bounds_recalculated() override;

	private:
		void add_to_scene(scene::collection& scene) override;
		void remove_from_scene(scene::collection& scene) override;
		void depth_changed() override;

		std::unique_ptr<scene::billboard> m_billboard;
	};
}
