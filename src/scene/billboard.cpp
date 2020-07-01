/*
 * Copyright (C) 2020  Christopher J. Howard
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
#include <algorithm>

const aabb<float> billboard::untransformed_bounds = {{-1, -1, -1}, {1, 1, 1}};

billboard::billboard():
	bounds(untransformed_bounds),
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
	bounds = other.bounds;
	material = other.material;
	type = other.type;
	alignment_axis = other.alignment_axis;
	return *this;
}

void billboard::set_material(::material* material)
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
	bounds = aabb<float>::transform(untransformed_bounds, get_transform());
}
