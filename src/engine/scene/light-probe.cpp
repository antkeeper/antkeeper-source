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

#include <engine/scene/light-probe.hpp>

namespace scene {

light_probe::light_probe()
{
	// Allocate illuminance texture
	m_illuminance_texture = std::make_shared<gl::texture_1d>(12, gl::pixel_type::float_32, gl::pixel_format::rgba);
	m_illuminance_texture->set_filters(gl::texture_min_filter::nearest,  gl::texture_mag_filter::nearest);
	
	// Allocate and init illuminance framebuffer
	m_illuminance_framebuffer = std::make_shared<gl::framebuffer>(12, 1);
	m_illuminance_framebuffer->attach(gl::framebuffer_attachment_type::color, m_illuminance_texture.get());
	
	// Init illuminance matrices
	m_illuminance_matrices[0] = {};
	m_illuminance_matrices[1] = {};
	m_illuminance_matrices[2] = {};
}

void light_probe::update_illuminance_matrices()
{
	m_illuminance_texture->read(std::as_writable_bytes(std::span{m_illuminance_matrices}), gl::pixel_type::float_32, gl::pixel_format::rgba, 0);
}

void light_probe::set_luminance_texture(std::shared_ptr<gl::texture_cube> texture)
{
	if (m_luminance_texture != texture)
	{
		m_luminance_texture = texture;
		set_luminance_outdated(true);
		set_illuminance_outdated(true);
	}
}

void light_probe::set_luminance_outdated(bool outdated)
{
	m_luminance_outdated = outdated;
}

void light_probe::set_illuminance_outdated(bool outdated)
{
	m_illuminance_outdated = outdated;
}

void light_probe::transformed()
{
	m_bounds = {get_translation(), get_translation()};
}

} // namespace scene
