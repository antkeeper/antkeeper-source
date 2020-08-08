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

#include "scene/scene-object.hpp"

static transform<float> transform_interpolate(const transform<float>& x, const transform<float>& y, float a)
{
	return
		{
			vmq::lerp(x.translation, y.translation, a),
			vmq::slerp(x.rotation, y.rotation, a),
			vmq::lerp(x.scale, y.scale, a),
		};
}

scene_object_base::scene_object_base():
	active(true),
	transform(vmq::identity_transform<float>, transform_interpolate),
	culling_mask(nullptr)
{}

void scene_object_base::set_culling_mask(const bounding_volume<float>* culling_mask)
{
	this->culling_mask = culling_mask;
}

std::size_t scene_object_base::next_object_type_id()
{
	static std::atomic<std::size_t> id{0};
	return id++;
}

void scene_object_base::update_tweens()
{
	transform.update();
}

void scene_object_base::look_at(const float3& position, const float3& target, const float3& up)
{
	transform[1].translation = position;
	transform[1].rotation = vmq::look_rotation(vmq::normalize(vmq::sub(target, position)), up);
	transformed();
}

void scene_object_base::transformed()
{}
