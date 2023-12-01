// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_NUPTIAL_FLIGHT_STATE_HPP
#define ANTKEEPER_NUPTIAL_FLIGHT_STATE_HPP

#include "game/states/game-state.hpp"
#include <engine/entity/id.hpp>
#include <engine/math/vector.hpp>
#include <engine/event/subscription.hpp>
#include <engine/scene/text.hpp>
#include <engine/utility/text-file.hpp>
#include <memory>

class nuptial_flight_state: public game_state
{
public:
	explicit nuptial_flight_state(::game& ctx);
	virtual ~nuptial_flight_state();
	
private:
	void create_camera_rig();
	void destroy_camera_rig();
	
	void set_camera_rig_zoom(float zoom);
	void satisfy_camera_rig_constraints();
	
	void setup_controls();
	void enable_controls();
	void disable_controls();
	
	void select_entity(entity::id entity_id);
	void select_nearest_entity(const math::fvec3& direction);
	
	// Camera
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

	// Ants
	entity::id swarm_eid;
	
	// Name generation
	std::shared_ptr<text_file> name_pool;
	
	// Picking
	std::uint32_t selected_picking_flag;
	entity::id selected_eid;
	
	// UI
	scene::text selection_text;
	
	// Controls
	bool mouse_look{false};
	std::vector<std::shared_ptr<::event::subscription>> action_subscriptions;	
	std::shared_ptr<::event::subscription> mouse_motion_subscription;
};

#endif // ANTKEEPER_NUPTIAL_FLIGHT_STATE_HPP
