// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/input/mouse.hpp>

namespace input {

void mouse::press(mouse_button button)
{
	button_pressed_publisher.publish({this, position, button});
}

void mouse::release(mouse_button button)
{
	button_released_publisher.publish({this, position, button});
}

void mouse::move(const math::vec2<std::int32_t>& position, const math::vec2<std::int32_t>& difference)
{
	this->position = position;
	moved_publisher.publish({this, position, difference});
}

void mouse::scroll(const math::fvec2& velocity)
{
	scrolled_publisher.publish({this, position, velocity});
}

} // namespace input
