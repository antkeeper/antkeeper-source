/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include "scene/object.hpp"
#include "geom/aabb.hpp"
#include "utility/fundamental-types.hpp"
#include "gl/vertex-array.hpp"
#include "gl/vertex-buffer.hpp"
#include "renderer/material.hpp"
#include "type/bitmap-font.hpp"
#include "type/text-direction.hpp"

namespace scene {

/**
 * Text scene object.
 */
class text: public object<text>
{
public:
	typedef geom::aabb<float> aabb_type;
	
	/// Constructs a text object.
	text();
	
	/// Destructs a text object.
	~text();
	
	/// @copydoc scene::object_base::render(const render::context&, render::queue&) const
	virtual void render(const render::context& ctx, render::queue& queue) const;
	
	/**
	 * Manually updates the text object if its font has been updated or altered in any way.
	 */
	void refresh();
	
	/**
	 * Sets the text material.
	 *
	 * @param material Text material.
	 */
	void set_material(material* material);
	
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
	material* get_material() const;
	
	/// Returns the text font.
	const type::bitmap_font* get_font() const;
	
	/// Returns the text direction.
	const type::text_direction& get_direction() const;
	
	/// Returns the text content.
	const std::string& get_content() const;
	
	/// Returns the text color.
	const float4& get_color() const;
	
	/// @copydoc scene::object::get_bounds() const
	virtual const bounding_volume_type& get_bounds() const;
	
	/// @copydoc scene::object::update_tweens()
	virtual void update_tweens();

private:
	void update_content();
	void update_color();
	
	virtual void transformed();
	
	mutable render::operation render_op;
	aabb_type local_bounds;
	aabb_type world_bounds;
	material* material;
	const type::bitmap_font* font;
	type::text_direction direction;
	std::string content_u8;
	std::u32string content_u32;
	float4 color;
	std::size_t vertex_stride;
	std::size_t vertex_count;
	std::vector<std::byte> vertex_data;
	gl::vertex_array* vao;
	gl::vertex_buffer* vbo;
};

inline material* text::get_material() const
{
	return material;
}

inline const type::bitmap_font* text::get_font() const
{
	return font;
}

inline const type::text_direction& text::get_direction() const
{
	return direction;
}

inline const std::string& text::get_content() const
{
	return content_u8;
}

inline const float4& text::get_color() const
{
	return color;
}

inline const typename object_base::bounding_volume_type& text::get_bounds() const
{
	return world_bounds;
}

} // namespace scene

#endif // ANTKEEPER_SCENE_TEXT_HPP
