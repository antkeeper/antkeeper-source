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

#include <engine/scene/object.hpp>
#include <engine/math/interpolation.hpp>

namespace scene {

typename object_base::transform_type object_base::interpolate_transforms(const transform_type& x, const transform_type& y, float a)
{
	return
		{
			math::lerp(x.translation, y.translation, a),
			math::nlerp(x.rotation, y.rotation, a),
			math::lerp(x.scale, y.scale, a),
		};
}

object_base::object_base():
	active(true),
	transform(math::transform<float>::identity, interpolate_transforms),
	culling_mask(nullptr)
{}

void object_base::set_culling_mask(const bounding_volume_type* culling_mask)
{
	this->culling_mask = culling_mask;
}

std::size_t object_base::next_object_type_id()
{
	static std::atomic<std::size_t> id{0};
	return id++;
}

void object_base::render(const render::context& ctx, render::queue& queue) const
{}

void object_base::update_tweens()
{
	transform.update();
}

void object_base::look_at(const vector_type& position, const vector_type& target, const vector_type& up)
{
	transform[1].translation = position;
	transform[1].rotation = math::look_rotation(math::normalize(math::sub(target, position)), up);
	transformed();
}

void object_base::transformed()
{}

} // namespace scene
