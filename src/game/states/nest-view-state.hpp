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

#ifndef ANTKEEPER_NEST_VIEW_STATE_HPP
#define ANTKEEPER_NEST_VIEW_STATE_HPP

#include "game/states/game-state.hpp"
#include <engine/entity/id.hpp>
#include <engine/utility/fundamental-types.hpp>
#include <engine/render/model.hpp>
#include <engine/event/subscription.hpp>
#include <engine/input/mouse-events.hpp>
#include <engine/geom/primitives/ray.hpp>
#include <engine/geom/primitives/plane.hpp>
#include <engine/math/angles.hpp>

class nest_view_state: public game_state
{
public:
	explicit nest_view_state(::game& ctx);
	virtual ~nest_view_state();
	
private:
	void create_third_person_camera_rig();
	void destroy_third_person_camera_rig();
	
	void set_third_person_camera_zoom(double zoom);
	void set_third_person_camera_rotation(double yaw, double pitch);
	
	void zoom_third_person_camera(double zoom);
	void translate_third_person_camera(const math::vector<double, 3>& direction, double magnitude);
	void rotate_third_person_camera(const input::mouse_moved_event& event);
	
	void handle_mouse_motion(const input::mouse_moved_event& event);
	
	void update_third_person_camera();
	void load_camera_preset(std::uint8_t index);
	void save_camera_preset(std::uint8_t index);
	void load_or_save_camera_preset(std::uint8_t index);
	
	[[nodiscard]] geom::ray<float, 3> get_mouse_ray(const math::vector<std::int32_t, 2>& mouse_position) const;
	
	void setup_controls();
	
	std::vector<std::shared_ptr<::event::subscription>> action_subscriptions;
	std::shared_ptr<::event::subscription> mouse_motion_subscription;
	
	bool mouse_look{false};
	bool mouse_grip{false};
	bool mouse_zoom{false};
	geom::plane<float> mouse_grip_plane{{0.0, 1.0, 0.0}, 0.0};
	math::vector<float, 3> mouse_grip_point{};
	
	bool moving{false};
	
	entity::id third_person_camera_rig_eid{entt::null};
	double third_person_camera_yaw{0.0};
	double third_person_camera_pitch{math::radians(45.0)};
	math::vector<double, 3> third_person_camera_focal_point{0.0, 0.0, 0.0};
	double third_person_camera_zoom{0.25};
	std::uint32_t third_person_camera_zoom_step_count{6};
	
	double third_person_camera_near_focal_plane_height{2.0f};
	double third_person_camera_far_focal_plane_height{50.0f};
	
	double third_person_camera_near_hfov{math::radians(45.0)};
	double third_person_camera_far_hfov{math::radians(90.0)};
	
	/// In focal plane heights per second.
	double third_person_camera_speed{1.0f};
	
	double third_person_camera_hfov{};
	double third_person_camera_vfov{};
	double third_person_camera_focal_plane_width{};
	double third_person_camera_focal_plane_height{};
	double third_person_camera_focal_distance{};
	math::quaternion<double> third_person_camera_yaw_rotation{math::quaternion<double>::identity()};
	math::quaternion<double> third_person_camera_pitch_rotation{math::quaternion<double>::identity()};
	math::quaternion<double> third_person_camera_orientation{math::quaternion<double>::identity()};
	
	struct camera_preset
	{
		double yaw{};
		double pitch{};
		math::vector<double, 3> focal_point{};
		double zoom{0.25};
	};
	
	std::vector<std::optional<camera_preset>> camera_presets{10};
};

#endif // ANTKEEPER_NEST_VIEW_STATE_HPP
