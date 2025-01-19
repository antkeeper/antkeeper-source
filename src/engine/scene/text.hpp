// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_SCENE_TEXT_HPP
#define ANTKEEPER_SCENE_TEXT_HPP

#include <engine/scene/object.hpp>
#include <engine/math/vector.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/render/material.hpp>
#include <engine/type/font.hpp>
#include <engine/type/text-direction.hpp>
#include <engine/event/subscription.hpp>
#include <memory>
#include <string_view>

namespace scene {

/// Text scene object.
class text: public object<text>
{
public:
	/// Constructs a text object.
	text();
	
	/// Destructs a text object.
	~text() override = default;
	
	void render(render::context& ctx) const override;
	
	/// Manually updates the text object if its font has been updated or altered in any way.
	void refresh();
	
	/// Sets the text material.
	/// @param material Text material.
	void set_material(std::shared_ptr<render::material> material);
	
	/// Sets the text font.
	/// @param font Shared pointer to a font.
	void set_font(std::shared_ptr<type::font> font);
	
	/// Sets the direction of the text.
	/// @param direction Text direction.
	void set_direction(type::text_direction direction);
	
	/// Sets the text content.
	/// @param content UTF-8 string of text.
	void set_content(std::string_view content);
	
	/// Sets the text color and opacity.
	/// @param color Text color and opacity.
	void set_color(const math::fvec4& color);
	
	/// Returns the text material.
	[[nodiscard]] inline const std::shared_ptr<render::material>& get_material() const noexcept
	{
		return m_render_op.material;
	}
	
	/// Returns the text font.
	[[nodiscard]] inline const auto& get_font() const noexcept
	{
		return m_font;
	}
	
	/// Returns the text direction.
	[[nodiscard]] inline const auto& get_direction() const noexcept
	{
		return m_direction;
	}
	
	/// Returns the text content.
	[[nodiscard]] inline const auto& get_content() const noexcept
	{
		return m_content_u8;
	}
	
	/// Returns the text color.
	[[nodiscard]] inline const auto& get_color() const noexcept
	{
		return m_color;
	}
	
	/// Returns the bounds of the text.
	[[nodiscard]] inline const aabb_type& get_bounds() const noexcept override
	{
		return m_world_bounds;
	}

private:
	/// Updates UV coordinates of each characters.
	void update_uvs();
	
	/// Updates the colors of each character.
	void update_colors();
	
	void update_content();
	
	/// Uploads character veretex data to the vertex buffer.
	void update_vertex_buffer();

	void transformed() override;

	bool is_visible(char32_t code) const;
	
	mutable render::operation m_render_op;
	aabb_type m_local_bounds{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
	aabb_type m_world_bounds{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
	std::shared_ptr<type::font> m_font;
	std::shared_ptr<event::subscription> m_font_texture_resized_subscription;
	type::text_direction m_direction{type::text_direction::ltr};
	std::string m_content_u8;
	std::u32string m_content_u32;
	math::fvec4 m_color{1.0f, 0.0f, 1.0f, 1.0f};
	std::vector<float> m_vertex_data;
	std::unique_ptr<gl::vertex_array> m_vertex_array;
	std::unique_ptr<gl::vertex_buffer> m_vertex_buffer;
};

} // namespace scene

#endif // ANTKEEPER_SCENE_TEXT_HPP
