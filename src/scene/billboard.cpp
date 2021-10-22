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

#include "scene/billboard.hpp"
#include "configuration.hpp"

namespace scene {

const typename billboard::aabb_type billboard::local_bounds = {{-1, -1, -1}, {1, 1, 1}};

billboard::billboard():
	world_bounds(local_bounds),
	material(nullptr),
	type(billboard_type::flat),
	alignment_axis(global_up)
{}

billboard::billboard(const billboard& other)
{
	*this = other;
}

billboard& billboard::operator=(const billboard& other)
{
	material = other.material;
	type = other.type;
	alignment_axis = other.alignment_axis;
	set_transform(other.get_transform());
	set_active(other.is_active());
	set_culling_mask(other.get_culling_mask());
	return *this;
}

void billboard::set_material(render::material* material)
{
	this->material = material;
}

void billboard::set_billboard_type(billboard_type type)
{
	this->type = type;
}

void billboard::set_alignment_axis(const float3& axis)
{
	this->alignment_axis = axis;
}

void billboard::transformed()
{
	world_bounds = aabb_type::transform(local_bounds, get_transform());
}

void billboard::update_tweens()
{
	object_base::update_tweens();
	if (material)
	{
		material->update_tweens();
	}
}

} // namespace scene
