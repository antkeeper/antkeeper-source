// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/ui/element.hpp>
#include <engine/debug/log.hpp>
#include <algorithm>
#include <stdexcept>

namespace ui {

element::~element()
{
	remove_children();
}

void element::add_child(std::shared_ptr<element> child)
{
	if (!child)
	{
		throw std::invalid_argument("UI element cannot add null child element.");
	}
	
	if (child->has_parent())
	{
		throw std::runtime_error("UI element cannot add child element with parent.");
	}
	
	// Add child
	m_children.emplace_back(child);
	child->set_parent(weak_from_this());
	
	// Notify ancestors that a descendant was added
	child->visit_ancestors
	(
		[&](element& e)
		{
			e.descendant_added(*child);
		}
	);
}

void element::remove_child(std::shared_ptr<element> child)
{
	if (!child)
	{
		throw std::invalid_argument("UI element cannot remove null child element.");
	}
	
	if (auto it = std::find(m_children.begin(), m_children.end(), child); it != m_children.end())
	{
		// Remove child
		m_children.erase(it);
		
		// Notify ancestors that a descendant was removed
		child->visit_ancestors
		(
			[&](element& e)
			{
				e.descendant_removed(*child);
			}
		);
		
		// De-parent child
		child->set_parent({});
	}
	else
	{
		throw std::invalid_argument("UI element cannot remove non-child element.");
	}
}

void element::remove_children()
{
	while (!m_children.empty())
	{
		remove_child(m_children.back());
	}
}

std::shared_ptr<element> element::get_root()
{
	auto root = shared_from_this();

	while (auto parent = root->m_parent.lock())
	{
		root = std::move(parent);
	}

	return root;
}

std::shared_ptr<const element> element::get_root() const
{
	auto root = shared_from_this();
	
	while (auto parent = root->m_parent.lock())
	{
		root = std::move(parent);
	}
	
	return root;
}

void element::visit_ancestors(const std::function<void(element&)>& visitor)
{
	for (auto parent = m_parent.lock(); parent; parent = parent->m_parent.lock())
	{
		visitor(*parent);
	}
}

void element::visit_descendants(const std::function<void(element&)>& visitor)
{
	for (const auto& child: m_children)
	{
		visitor(*child);
		child->visit_descendants(visitor);
	}
}

void element::set_anchors(float left, float bottom, float right, float top)
{
	m_anchor_min.x() = left;
	m_anchor_min.y() = bottom;
	m_anchor_max.x() = right;
	m_anchor_max.y() = top;
	recalculate_bounds();
}

void element::set_anchor_left(float left)
{
	m_anchor_min.x() = left;
	recalculate_bounds();
}

void element::set_anchor_bottom(float bottom)
{
	m_anchor_min.y() = bottom;
	recalculate_bounds();
}

void element::set_anchor_right(float right)
{
	m_anchor_max.x() = right;
	recalculate_bounds();
}

void element::set_anchor_top(float top)
{
	m_anchor_max.y() = top;
	recalculate_bounds();
}

void element::set_margins(float left, float bottom, float right, float top)
{
	m_margin_min.x() = left;
	m_margin_min.y() = bottom;
	m_margin_max.x() = right;
	m_margin_max.y() = top;
	recalculate_bounds();
}

void element::set_margin_left(float left)
{
	m_margin_min.x() = left;
	recalculate_bounds();
}

void element::set_margin_bottom(float bottom)
{
	m_margin_min.y() = bottom;
	recalculate_bounds();
}

void element::set_margin_right(float right)
{
	m_margin_max.x() = right;
	recalculate_bounds();
}

void element::set_margin_top(float top)
{
	m_margin_max.y() = top;
	recalculate_bounds();
}

void element::set_focus(bool focus)
{
	if (m_focus != focus)
	{
		if (focus)
		{
			// Set focus state of all other elements to `false`
			get_root()->visit_descendants
			(
				[&](auto& e)
				{
					if (&e != this)
					{
						e.set_focus(false);
					}
				}
			);
		}
		
		m_focus = focus;
		
		if (m_focus_changed_callback)
		{
			m_focus_changed_callback({this, m_focus});
		}
	}
}

void element::set_focus_back(std::weak_ptr<element> back)
{
	m_focus_back = std::move(back);
}

void element::set_focus_forward(std::weak_ptr<element> forward)
{
	m_focus_forward = std::move(forward);
}

void element::set_focus_left(std::weak_ptr<element> left)
{
	m_focus_left = std::move(left);
}

void element::set_focus_right(std::weak_ptr<element> right)
{
	m_focus_right = std::move(right);
}

void element::set_focus_up(std::weak_ptr<element> up)
{
	m_focus_up = std::move(up);
}

void element::set_focus_down(std::weak_ptr<element> down)
{
	m_focus_down = std::move(down);
}

void element::set_focus_changed_callback(std::function<void(const element_focus_changed_event&)> callback)
{
	m_focus_changed_callback = std::move(callback);
}

void element::set_mouse_entered_callback(std::function<void(const element_mouse_entered_event&)> callback)
{
	m_mouse_entered_callback = std::move(callback);
}

void element::set_mouse_exited_callback(std::function<void(const element_mouse_exited_event&)> callback)
{
	m_mouse_exited_callback = std::move(callback);
}

void element::set_mouse_button_pressed_callback(std::function<void(const element_mouse_button_pressed_event&)> callback)
{
	m_mouse_button_pressed_callback = std::move(callback);
}

void element::set_mouse_button_released_callback(std::function<void(const element_mouse_button_released_event&)> callback)
{
	m_mouse_button_released_callback = std::move(callback);
}

void element::press(input::mouse_button button)
{
	if (m_mouse_button_pressed_callback)
	{
		m_mouse_button_pressed_callback({this, nullptr, m_bounds.center(), button});
	}
}

void element::release(input::mouse_button button)
{
	if (m_mouse_button_released_callback)
	{
		m_mouse_button_released_callback({this, nullptr, m_bounds.center(), button});
	}
}

void element::click(input::mouse_button button)
{
	press(button);
	release(button);
}

void element::add_to_scene([[maybe_unused]] ::scene::collection& scene)
{}

void element::remove_from_scene([[maybe_unused]] ::scene::collection& scene)
{}

bool element::handle_mouse_moved(const input::mouse_moved_event& event)
{
	for (const auto& child: m_children)
	{
		if (child->handle_mouse_moved(event))
		{
			return true;
		}
	}
	
	if (m_bounds.contains(math::fvec2(event.position)))
	{
		if (!m_hover)
		{
			m_hover = true;
			
			if (m_mouse_entered_callback)
			{
				m_mouse_entered_callback({this, event.mouse, event.position});
			}
		}
	}
	else
	{
		if (m_hover)
		{
			m_hover = false;
			
			if (m_mouse_exited_callback)
			{
				m_mouse_exited_callback({this, event.mouse, event.position});
			}
		}
	}
	
	return false;
}

bool element::handle_mouse_button_pressed(const input::mouse_button_pressed_event& event)
{
	for (const auto& child: m_children)
	{
		if (child->handle_mouse_button_pressed(event))
		{
			return true;
		}
	}
	
	if (m_bounds.contains(math::fvec2(event.position)))
	{
		if (m_mouse_button_pressed_callback)
		{
			m_mouse_button_pressed_callback({this, event.mouse, event.position, event.button});
		}
	}
	
	return false;
}

bool element::handle_mouse_button_released(const input::mouse_button_released_event& event)
{
	for (const auto& child: m_children)
	{
		if (child->handle_mouse_button_released(event))
		{
			return true;
		}
	}
	
	if (m_bounds.contains(math::fvec2(event.position)))
	{
		if (m_mouse_button_released_callback)
		{
			m_mouse_button_released_callback({this, event.mouse, event.position, event.button});
		}
	}
	
	return false;
}

void element::bounds_recalculated()
{}

void element::set_parent(std::weak_ptr<element> parent)
{
	m_parent = std::move(parent);
	recalculate_bounds();
}

void element::recalculate_bounds()
{
	if (auto parent = m_parent.lock())
	{
		const auto& parent_bounds = parent->get_bounds();
		const auto parent_size = parent_bounds.size();
		
		m_bounds.min = m_anchor_min * parent_size + m_margin_min + parent_bounds.min;
		m_bounds.max = m_anchor_max * parent_size + m_margin_max + parent_bounds.min;
	}
	else
	{
		m_bounds.min = m_margin_min;
		m_bounds.max = m_margin_max;
	}
	
	bounds_recalculated();
	
	for (auto& child: m_children)
	{
		child->recalculate_bounds();
	}
}

void element::descendant_added([[maybe_unused]] element& descendant)
{}

void element::descendant_removed([[maybe_unused]] element& descendant)
{}

} // namespace ui
