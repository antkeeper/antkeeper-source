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

#include "scene/scene-object.hpp"
#include "math/math.hpp"

typename scene_object_base::transform_type scene_object_base::interpolate_transforms(const transform_type& x, const transform_type& y, float a)
{
	return
		{
			math::lerp(x.translation, y.translation, a),
			math::nlerp(x.rotation, y.rotation, a),
			math::lerp(x.scale, y.scale, a),
		};
}

scene_object_base::scene_object_base():
	active(true),
	transform(math::identity_transform<float>, interpolate_transforms),
	culling_mask(nullptr)
{}

void scene_object_base::set_culling_mask(const bounding_volume_type* culling_mask)
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

void scene_object_base::look_at(const vector_type& position, const vector_type& target, const vector_type& up)
{
	transform[1].translation = position;
	transform[1].rotation = math::look_rotation(math::normalize(math::sub(target, position)), up);
	transformed();
}

void scene_object_base::transformed()
{}
