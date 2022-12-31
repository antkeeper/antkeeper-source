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

#ifndef ANTKEEPER_GAME_STATE_NUPTIAL_FLIGHT_HPP
#define ANTKEEPER_GAME_STATE_NUPTIAL_FLIGHT_HPP

#include "game/state/base.hpp"
#include "entity/id.hpp"
#include "utility/fundamental-types.hpp"

namespace game {
namespace state {

class nuptial_flight: public game::state::base
{
public:
	nuptial_flight(game::context& ctx);
	virtual ~nuptial_flight();
	
private:
	void create_camera_rig();
	void destroy_camera_rig();
	
	void set_camera_rig_zoom(float zoom);
	void satisfy_camera_rig_constraints();
	
	void enable_controls();
	void disable_controls();
	
	void select_entity(entity::id entity_id);
	void select_nearest_entity(const float3& direction);
	
	entity::id camera_rig_focus_eid;
	entity::id camera_rig_focus_ease_to_eid;
	
	entity::id camera_rig_eid;
	entity::id camera_rig_spring_translation_eid;
	entity::id camera_rig_spring_rotation_eid;
	entity::id camera_rig_copy_translation_eid;
	entity::id camera_rig_pivot_eid;
	float camera_rig_near_distance;
	float camera_rig_far_distance;
	float camera_rig_near_fov;
	float camera_rig_far_fov;
	float camera_rig_zoom_speed;
	float camera_rig_zoom;
	
	entity::id camera_rig_fov_spring_eid;
	
	float camera_rig_translation_spring_angular_frequency;
	float camera_rig_rotation_spring_angular_frequency;
	float camera_rig_fov_spring_angular_frequency;
	float camera_rig_focus_ease_to_duration;

	
	entity::id swarm_eid;
	
	std::uint32_t selected_picking_flag;
	entity::id selected_eid;
	
	bool mouse_look;
};

} // namespace state
} // namespace game

#endif // ANTKEEPER_GAME_STATE_NUPTIAL_FLIGHT_HPP
