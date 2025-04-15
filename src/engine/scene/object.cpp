// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.scene.object;
import engine.utility.sized_types;

namespace engine::scene
{
	usize object_base::next_object_type_id()
	{
		static std::atomic<usize> id{0};
		return id++;
	}

	void object_base::look_at(const vector_type& position, const vector_type& target, const vector_type& up)
	{
		m_transform.translation = position;
		m_transform.rotation = math::look_rotation(math::normalize(target - position), up);
		transformed();
	}
}
