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

#ifndef ANTKEEPER_SCENE_TEXT_HPP
#define ANTKEEPER_SCENE_TEXT_HPP

#include <engine/scene/object.hpp>
#include <engine/utility/fundamental-types.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/render/material.hpp>
#include <engine/type/bitmap-font.hpp>
#include <engine/type/text-direction.hpp>

namespace scene {

/**
 * Text scene object.
 */
class text: public object<text>
{
public:
	/// Constructs a text object.
	text();
	
	void render(render::context& ctx) const override;
	
	/**
	 * Manually updates the text object if its font has been updated or altered in any way.
	 */
	void refresh();
	
	/**
	 * Sets the text material.
	 *
	 * @param material Text material.
	 */
	void set_material(std::shared_ptr<render::material> material);
	
	/**
	 * Sets the text font.
	 *
	 * @param font Pointer to a font.
	 */
	void set_font(const type::bitmap_font* font);
	
	/**
	 * Sets the direction of the text.
	 *
	 * @param direction Text direction.
	 */
	void set_direction(type::text_direction direction);
	
	/**
	 * Sets the text content.
	 *
	 * @param content UTF-8 string of text.
	 */
	void set_content(const std::string& content);
	
	/**
	 * Sets the text color.
	 *
	 * Text color is passed to the text's material shader as a vertex color.
	 *
	 * @param color RGBA color.
	 */
	void set_color(const float4& color);
	
	/// Returns the text material.
	[[nodiscard]] inline std::shared_ptr<render::material> get_material() const noexcept
	{
		return m_render_op.material;
	}
	
	/// Returns the text font.
	[[nodiscard]] inline const type::bitmap_font* get_font() const noexcept
	{
		return m_font;
	}
	
	/// Returns the text direction.
	[[nodiscard]] inline const type::text_direction& get_direction() const noexcept
	{
		return m_direction;
	}
	
	/// Returns the text content.
	[[nodiscard]] inline const std::string& get_content() const noexcept
	{
		return m_content_u8;
	}
	
	/// Returns the text color.
	[[nodiscard]] inline const float4& get_color() const noexcept
	{
		return m_color;
	}
	
	[[nodiscard]] inline const aabb_type& get_bounds() const noexcept override
	{
		return m_world_bounds;
	}

private:
	void update_content();
	void update_color();
	
	virtual void transformed();
	
	mutable render::operation m_render_op;
	aabb_type m_local_bounds{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
	aabb_type m_world_bounds{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
	const type::bitmap_font* m_font{nullptr};
	type::text_direction m_direction{type::text_direction::ltr};
	std::string m_content_u8;
	std::u32string m_content_u32;
	float4 m_color{1.0f, 0.0f, 1.0f, 1.0f};
	std::size_t m_vertex_stride{0};
	std::size_t m_vertex_count{0};
	std::vector<std::byte> m_vertex_data;
	std::unique_ptr<gl::vertex_array> m_vao;
	std::unique_ptr<gl::vertex_buffer> m_vbo;
};

} // namespace scene

#endif // ANTKEEPER_SCENE_TEXT_HPP
