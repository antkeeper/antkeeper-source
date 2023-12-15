// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
	m_transform.rotation = math::look_rotation(math::normalize(target - position), up);
	transformed();
}

} // namespace scene
