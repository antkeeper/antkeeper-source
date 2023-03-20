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

namespace scene {

std::size_t object_base::next_object_type_id()
{
	static std::atomic<std::size_t> id{0};
	return id++;
}

void object_base::look_at(const vector_type& position, const vector_type& target, const vector_type& up)
{
	m_transform.translation = position;
	m_transform.rotation = math::look_rotation(math::normalize(math::sub(target, position)), up);
	transformed();
}

} // namespace scene
