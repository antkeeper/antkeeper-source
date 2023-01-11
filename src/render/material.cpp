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

#include "render/material.hpp"

namespace render {

material::material(gl::shader_program* program):
	program(program),
	flags(0),
	blend_mode(blend_mode::opaque),
	opacity_threshold(0.5f),
	two_sided(false),
	shadow_mode(shadow_mode::opaque)
{}

material::material():
	material(nullptr)
{}

material::material(const material& other)
{
	*this = other;
}

material::~material()
{
	for (material_property_base* property: properties)
	{
		delete property;
	}
}

material& material::operator=(const material& other)
{
	// Remove all properties
	for (material_property_base* property: properties)
	{
		delete property;
	}
	properties.clear();
	property_map.clear();

	this->program = other.program;
	this->flags = other.flags;
	this->blend_mode = other.blend_mode;
	this->opacity_threshold = other.opacity_threshold;
	this->two_sided = other.two_sided;
	this->shadow_mode = other.shadow_mode;
	for (auto it = other.property_map.begin(); it != other.property_map.end(); ++it)
	{
		material_property_base* property = it->second->clone();
		properties.push_back(property);
		property_map[it->first] = property;
	}

	return *this;
}

void material::update_tweens()
{
	for (material_property_base* property: properties)
	{
		property->update_tweens();
	}
}

std::size_t material::upload(double a) const
{
	if (!program)
	{
		return false;
	}

	std::size_t failed_upload_count = 0;

	for (material_property_base* property: properties)
	{
		if (!property->upload(a))
		{
			++failed_upload_count;
		}
	}

	return failed_upload_count;
}

void material::set_shader_program(gl::shader_program* program)
{
	this->program = program;
	reconnect_properties();
}

void material::set_flags(std::uint32_t flags) noexcept
{
	this->flags = flags;
}

void material::set_blend_mode(render::blend_mode mode) noexcept
{
	blend_mode = mode;
}

void material::set_opacity_threshold(float threshold) noexcept
{
	opacity_threshold = threshold;
}

void material::set_two_sided(bool two_sided) noexcept
{
	this->two_sided = two_sided;
}

void material::set_shadow_mode(render::shadow_mode mode) noexcept
{
	shadow_mode = mode;
}

std::size_t material::reconnect_properties()
{
	std::size_t disconnected_property_count = properties.size();

	for (auto it = property_map.begin(); it != property_map.end(); ++it)
	{
		material_property_base* property = it->second;

		property->disconnect();

		if (program != nullptr)
		{
			if (property->connect(program->get_input(it->first)))
			{
				--disconnected_property_count;
			}
		}
	}

	return disconnected_property_count;
}

} // namespace render
