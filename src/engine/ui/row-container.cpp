// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/ui/row-container.hpp>
#include <engine/math/functions.hpp>
#include <engine/utility/sized-types.hpp>

namespace engine::ui
{
	void row_container::set_alignment(ui::alignment alignment)
	{
		m_alignment = alignment;
		redistribute_children();
	}

	void row_container::set_spacing(float spacing)
	{
		m_spacing = spacing;
		redistribute_children();
	}

	void row_container::redistribute_children()
	{
		// Calculate minimum size of the container
		math::fvec2 min_size{};
		const auto& children = get_children();
		for (const auto& child : children)
		{
			const auto& child_min_size = child->get_min_size();
			min_size.x() += child_min_size.x();
			min_size.y() = math::max(min_size.y(), child_min_size.y());
		}

		if (children.size() > 1)
		{
			min_size.x() += m_spacing * (children.size() - 1);
		}

		// Align children
		float x = 0.0f;
		for (usize i = 0; i < children.size(); ++i)
		{
			const auto& child = children[i];
			const auto& child_min_size = child->get_min_size();

			if (m_alignment == alignment::start)
			{
				child->set_anchors(0.0f, 1.0f, 0.0f, 1.0f);
				child->set_margins
				(
					x,
					-child_min_size.y(),
					x + child_min_size.x(),
					0.0f
				);
			}
			else if (m_alignment == alignment::end)
			{
				child->set_anchors(0.0f, 0.0f, 0.0f, 0.0f);
				child->set_margins
				(
					x,
					0.0f,
					x + child_min_size.x(),
					child_min_size.y()
				);
			}
			else
			{
				child->set_anchors(0.0f, 0.5f, 0.0f, 0.5f);
				child->set_margins
				(
					x,
					-child_min_size.y() * 0.5f,
					x + child_min_size.x(),
					child_min_size.y() * 0.5f
				);
			}

			x += child_min_size.x();
			if (i < children.size() - 1)
			{
				x += m_spacing;
			}
		}

		set_min_size(min_size);
	}
}
