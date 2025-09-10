// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/ui/center-container.hpp>

namespace engine::ui
{
	void center_container::redistribute_children()
	{
		math::fvec2 min_size{};

		const auto& children = get_children();
		for (const auto& child : children)
		{
			const auto& child_min_size = child->get_min_size();

			child->set_anchors(0.5f, 0.5f, 0.5f, 0.5f);
			child->set_margins
			(
				-child_min_size.x() * 0.5f,
				-child_min_size.y() * 0.5f,
				child_min_size.x() * 0.5f,
				child_min_size.y() * 0.5f
			);

			min_size = math::max(min_size, child_min_size);
		}

		set_min_size(min_size);
	}
}
