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

#ifndef ANTKEEPER_GAME_STATE_NEST_SELECTION_HPP
#define ANTKEEPER_GAME_STATE_NEST_SELECTION_HPP

#include "game/state/base.hpp"
#include "entity/id.hpp"
#include "utility/fundamental-types.hpp"

namespace game {
namespace state {

class nest_selection: public game::state::base
{
public:
	nest_selection(game::context& ctx);
	virtual ~nest_selection();
	
private:
	void create_first_person_camera_rig();
	void destroy_first_person_camera_rig();
	
	void set_first_person_camera_rig_pedestal(float pedestal);
	void move_first_person_camera_rig(const float2& direction, float factor);
	void satisfy_first_person_camera_rig_constraints();
	
	void enable_controls();
	void disable_controls();
	
	entity::id first_person_camera_rig_eid;
	entity::id first_person_camera_rig_spring_translation_eid;
	entity::id first_person_camera_rig_spring_rotation_eid;
	entity::id first_person_camera_rig_fov_spring_eid;
	float first_person_camera_rig_translation_spring_angular_frequency;
	float first_person_camera_rig_rotation_spring_angular_frequency;
	float first_person_camera_rig_fov_spring_angular_frequency;
	float first_person_camera_rig_min_elevation;
	float first_person_camera_rig_max_elevation;
	float first_person_camera_near_fov;
	float first_person_camera_far_fov;
	float first_person_camera_near_speed;
	float first_person_camera_far_speed;
	float first_person_camera_rig_pedestal_speed;
	float first_person_camera_rig_pedestal;
	
	bool mouse_look;
};

} // namespace state
} // namespace game

#endif // ANTKEEPER_GAME_STATE_NEST_SELECTION_HPP
