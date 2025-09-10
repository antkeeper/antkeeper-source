// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/geom/brep/loop-container.hpp>

namespace engine::geom::brep
{
	loop* loop_container::emplace_back()
	{
		auto loop = element_container<brep::loop>::emplace_back();
		loop->m_index = size() - 1;

		return loop;
	};
}
