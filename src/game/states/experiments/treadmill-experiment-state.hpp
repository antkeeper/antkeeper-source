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

#ifndef ANTKEEPER_TREADMILL_EXPERIMENT_STATE_HPP
#define ANTKEEPER_TREADMILL_EXPERIMENT_STATE_HPP

#include "game/states/game-state.hpp"
#include <engine/entity/id.hpp>
#include <engine/math/vector.hpp>
#include <engine/render/model.hpp>
#include <engine/event/subscription.hpp>
#include <engine/input/mouse-events.hpp>
#include <engine/geom/primitives/ray.hpp>
#include <engine/geom/primitives/plane.hpp>
#include <engine/math/angles.hpp>
#include <engine/scene/light-probe.hpp>
#include <engine/scene/rectangle-light.hpp>
#include <engine/geom/bvh/bvh.hpp>
#include <engine/geom/brep/brep-mesh.hpp>
#include <engine/animation/ik/ik-rig.hpp>
#include "game/ant/ant-phenome.hpp"

class treadmill_experiment_state: public game_state
{
public:
	explicit treadmill_experiment_state(::game& ctx);
	~treadmill_experiment_state() override;
	
private:
	void create_third_person_camera_rig();
	void destroy_third_person_camera_rig();
	
	void set_third_person_camera_zoom(double zoom);
	void set_third_person_camera_rotation(double yaw, double pitch);
	
	void zoom_third_person_camera(double zoom);
	void translate_third_person_camera(const math::dvec3& direction, double magnitude);
	void rotate_third_person_camera(const input::mouse_moved_event& event);
	
	void handle_mouse_motion(const input::mouse_moved_event& event);
	
	void update_third_person_camera();
	void load_camera_preset(std::uint8_t index);
	void save_camera_preset(std::uint8_t index);
	void load_or_save_camera_preset(std::uint8_t index);
	
	[[nodiscard]] geom::ray<float, 3> get_mouse_ray(const math::vec2<std::int32_t>& mouse_position) const;
	
	void setup_controls();
	
	std::vector<std::shared_ptr<::event::subscription>> action_subscriptions;
	std::shared_ptr<::event::subscription> mouse_motion_subscription;
	
	bool mouse_look{false};
	bool mouse_grip{false};
	bool mouse_zoom{false};
	geom::plane<float> mouse_grip_plane{{0.0, 1.0, 0.0}, 0.0};
	math::fvec3 mouse_grip_point{};
	
	bool moving{false};
	
	entity::id third_person_camera_rig_eid{entt::null};
	double third_person_camera_yaw{0.0};
	double third_person_camera_pitch{math::radians(45.0)};
	math::dvec3 third_person_camera_focal_point{0.0, 0.0, 0.0};
	double third_person_camera_zoom{0.25};
	std::uint32_t third_person_camera_zoom_step_count{6};
	
	double third_person_camera_near_focal_plane_height{1.0f};
	double third_person_camera_far_focal_plane_height{50.0f};
	
	double third_person_camera_near_hfov{math::radians(90.0)};
	double third_person_camera_far_hfov{math::radians(45.0)};
	
	/// In focal plane heights per second.
	double third_person_camera_speed{1.0f};
	
	double third_person_camera_hfov{};
	double third_person_camera_vfov{};
	double third_person_camera_focal_plane_width{};
	double third_person_camera_focal_plane_height{};
	double third_person_camera_focal_distance{};
	math::dquat third_person_camera_yaw_rotation{math::dquat::identity()};
	math::dquat third_person_camera_pitch_rotation{math::dquat::identity()};
	math::dquat third_person_camera_orientation{math::dquat::identity()};
	
	struct camera_preset
	{
		double yaw{};
		double pitch{};
		math::dvec3 focal_point{};
		double zoom{0.25};
	};
	
	std::vector<std::optional<camera_preset>> camera_presets{10};
	
	std::shared_ptr<render::matvar_fvec3> light_rectangle_emissive;
	std::shared_ptr<scene::light_probe> light_probe;
	
	entity::id larva_eid;
	entity::id worker_eid;
	
	std::shared_ptr<ant_phenome> worker_phenome;
	std::shared_ptr<ik_rig> worker_ik_rig;
	
	std::shared_ptr<scene::light_probe> sky_probe;
	std::shared_ptr<scene::rectangle_light> area_light;
};

#endif // ANTKEEPER_TREADMILL_EXPERIMENT_STATE_HPP
