// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.ui.column_container;
import engine.math.functions;
import engine.utility.sized_types;

namespace engine::ui
{
	void column_container::set_alignment(ui::alignment alignment)
	{
		m_alignment = alignment;
		redistribute_children();
	}

	void column_container::set_spacing(float spacing)
	{
		m_spacing = spacing;
		redistribute_children();
	}

	void column_container::redistribute_children()
	{
		// Calculate minimum size of the container
		math::fvec2 min_size{};
		const auto& children = get_children();
		for (const auto& child : children)
		{
			const auto& child_min_size = child->get_min_size();
			min_size.x() = math::max(min_size.x(), child_min_size.x());
			min_size.y() += child_min_size.y();
		}

		if (children.size() > 1)
		{
			min_size.y() += m_spacing * (children.size() - 1);
		}

		// Align children
		float y = 0.0f;
		for (usize i = 0; i < children.size(); ++i)
		{
			const auto& child = children[i];
			const auto& child_min_size = child->get_min_size();

			if (m_alignment == alignment::start)
			{
				child->set_anchors(0.0f, 1.0f, 0.0f, 1.0f);
				child->set_margins
				(
					0.0f,
					-y - child_min_size.y(),
					child_min_size.x(),
					-y
				);
			}
			else if (m_alignment == alignment::end)
			{
				child->set_anchors(1.0f, 1.0f, 1.0f, 1.0f);
				child->set_margins
				(
					-child_min_size.x(),
					-y - child_min_size.y(),
					0.0f,
					-y
				);
			}
			else
			{
				child->set_anchors(0.5f, 1.0f, 0.5f, 1.0f);
				child->set_margins
				(
					-child_min_size.x() * 0.5f,
					-y - child_min_size.y(),
					child_min_size.x() * 0.5f,
					-y
				);
			}

			y += child_min_size.y();
			if (i < children.size() - 1)
			{
				y += m_spacing;
			}
		}

		set_min_size(min_size);
	}
}
