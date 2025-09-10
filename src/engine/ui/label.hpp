// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/ui/element.hpp>
#include <engine/scene/text.hpp>
#include <engine/type/font.hpp>
#include <memory>

namespace engine::ui
{
	/// UI label.
	class label: public element
	{
	public:
		/// Constructs an label.
		label();

		/// Destructs an label.
		~label() override = default;

		[[nodiscard]] inline constexpr element_type get_type() const noexcept override
		{
			return element_type::label;
		}

		/// Sets the label text.
		/// @param text UTF-8 string of text.
		void set_text(std::string_view text);

		/// Sets the label font.
		/// @param font Label font.
		void set_font(std::shared_ptr<type::font> font);

		/// Returns the label font.
		[[nodiscard]] std::shared_ptr<type::font> get_font() const noexcept;

		/// Sets the label material.
		/// @param material Label material.
		void set_material(std::shared_ptr<render::material> material);

		/// Sets the label color and opacity.
		/// @param color Label color and opacity.
		void set_color(const math::fvec4& color);

		/// Sets the label color.
		/// @param color Label color.
		void set_color(const math::fvec3& color);

		/// Returns the label text.
		[[nodiscard]] const std::string& get_text() const noexcept;

		/// Returns the label color and opacity.
		[[nodiscard]] inline constexpr const math::fvec4& get_color() const noexcept
		{
			return m_color;
		}

		/// Returns the label text scene object.
		[[nodiscard]] inline const auto& get_scene_object() const noexcept
		{
			return *m_text_object;
		}

		/// Sets the function used to refresh the label text.
		/// @param refresher Text refresher function.
		void set_text_refresher(std::function<std::string(const label&)> refresher);

		/// Refreshes the label text.
		void refresh_text();

	private:
		void add_to_scene(scene::collection& scene) override;
		void remove_from_scene(scene::collection& scene) override;
		void bounds_recalculated() override;
		void effective_opacity_changed() override;
		void reposition_text();
		void recalculate_min_size();

		math::fvec4 m_color{1.0f, 0.0f, 1.0f, 1.0f};
		std::unique_ptr<scene::text> m_text_object;
		std::function<std::string(const label&)> m_text_refresher;
	};
}
