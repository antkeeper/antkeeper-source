// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_NEST_SELECTION_STATE_HPP
#define ANTKEEPER_NEST_SELECTION_STATE_HPP

#include "game/states/game-state.hpp"
#include <engine/entity/id.hpp>
#include <engine/math/vector.hpp>
#include <engine/render/model.hpp>
#include <engine/animation/ik/solvers/ccd-ik-solver.hpp>
#include <engine/scene/light-probe.hpp>

class nest_selection_state: public game_state
{
public:
	explicit nest_selection_state(::game& ctx);
	virtual ~nest_selection_state();
	
private:
	void create_first_person_camera_rig();
	void destroy_first_person_camera_rig();
	void set_first_person_camera_rig_pedestal(float pedestal);
	void move_first_person_camera_rig(const math::fvec2& direction, float factor);
	void satisfy_first_person_camera_rig_constraints();
	
	void setup_controls();
	void enable_controls();
	void disable_controls();
	
	std::vector<std::shared_ptr<::event::subscription>> action_subscriptions;
	std::shared_ptr<::event::subscription> mouse_motion_subscription;
	std::shared_ptr<render::model> worker_model;
	
	bool mouse_look{false};
	bool mouse_drag{false};
	bool adjust_exposure{false};
	bool adjust_time{false};
	
	bool moving{false};
	math::fvec2 movement_direction{0.0f, 0.0f};
	float movement_speed{0.0f};
	
	entity::id worker_ant_eid;
	
	entity::id larva_eid;
	
	entity::id first_person_camera_rig_eid;
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
	
	double first_person_camera_yaw{0.0};
	double first_person_camera_pitch{0.0};
	
	std::shared_ptr<ccd_ik_solver> larva_ik_solver;
	
	std::shared_ptr<scene::light_probe> sky_probe;
};

#endif // ANTKEEPER_NEST_SELECTION_STATE_HPP
