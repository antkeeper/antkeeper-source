/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_GAME_SYSTEM_CAMERA_HPP
#define ANTKEEPER_GAME_SYSTEM_CAMERA_HPP

#include "game/system/updatable.hpp"
#include "event/event-handler.hpp"
#include "event/input-events.hpp"
#include "event/window-events.hpp"
#include "utility/fundamental-types.hpp"

namespace game {
namespace system {

class camera:
	public updatable,
	public event_handler<window_resized_event>
{
public:
	camera(entity::registry& registry);
	virtual void update(double t, double dt);
	
	void set_viewport(const float4& viewport);

private:
	virtual void handle_event(const window_resized_event& event);
	
	float4 viewport;
};

} // namespace system
} // namespace game

#endif // ANTKEEPER_GAME_SYSTEM_CAMERA_HPP

