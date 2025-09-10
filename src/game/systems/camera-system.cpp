// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <entt/entt.hpp>
#include "game/systems/camera-system.hpp"
#include "game/components/autofocus-component.hpp"
#include "game/components/spring-arm-component.hpp"
#include "game/components/scene-object-component.hpp"
#include <engine/animation/ease.hpp>
#include <engine/math/functions.hpp>
#include <engine/math/projection.hpp>
#include <engine/math/euler-angles.hpp>
#include <engine/scene/camera.hpp>

void camera_system::fixed_update(entity::registry&, float, float)
{}

void camera_system::variable_update(entity::registry& registry, float t, float dt, float alpha)
{
	const double variable_update_time = t + dt * alpha;
	const double variable_timestep = math::max(0.0, variable_update_time - m_variable_update_time);
	m_variable_update_time = variable_update_time;
	
	/*
	auto autofocus_group = m_registry.group<autofocus_component>(entt::get<scene_object_component>);
	std::for_each
	(
		std::execution::seq,
		autofocus_group.begin(),
		autofocus_group.end(),
		[&](auto entity_id)
		{
			auto& autofocus = autofocus_group.get<autofocus_component>(entity_id);
			auto& camera = static_cast<scene::camera&>(*autofocus_group.get<scene_object_component>(entity_id).object);
			
			// Clamp zoom factor
			autofocus.zoom = math::clamp(autofocus.zoom, 0.0, 1.0);
			
			// Calculate horizontal and vertical FoV
			autofocus.hfov = animation::ease<double, double>::out_sine(autofocus.far_hfov, autofocus.near_hfov, autofocus.zoom);
			autofocus.vfov = math::vertical_fov(autofocus.hfov, static_cast<double>(camera.get_aspect_ratio()));
			
			// Calculate focal plane dimensions
			autofocus.focal_plane_size.y() = animation::ease<double, double>::out_sine(autofocus.far_focal_plane_height, autofocus.near_focal_plane_height, autofocus.zoom);
			autofocus.focal_plane_size.x() = autofocus.focal_plane_size.y() * static_cast<double>(camera.get_aspect_ratio());
			
			// Calculate focal distance
			autofocus.focal_distance = autofocus.focal_plane_height * 0.5 / math::tan(autofocus.vfov * 0.5);
			
			// Update camera projection matrix
			camera.set_vertical_fov(static_cast<float>(autofocus.vfov));
		}
	);
	*/
	
	auto spring_arm_group = registry.group<spring_arm_component>(entt::get<scene_object_component>);
	for (auto entity_id: spring_arm_group)
	{
		auto& spring_arm = spring_arm_group.get<spring_arm_component>(entity_id);
		auto& camera = static_cast<scene::camera&>(*spring_arm_group.get<scene_object_component>(entity_id).object);
		
		math::transform<double> parent_transform = math::identity<math::transform<double>>;
		if (spring_arm.parent_eid != entt::null)
		{
			const auto parent_scene = registry.try_get<scene_object_component>(spring_arm.parent_eid);
			if (parent_scene)
			{
				parent_transform.translation = math::dvec3(parent_scene->object->get_translation());
				parent_transform.rotation = math::dquat(parent_scene->object->get_rotation());
			}
		}
		
		// Calculate focal point
		spring_arm.focal_point_spring.set_target_value(parent_transform * spring_arm.focal_point_offset);
		
		// Integrate angular velocities
		spring_arm.angles_spring.set_target_value(spring_arm.angles_spring.get_target_value() + spring_arm.angular_velocities * variable_timestep);
		
		// Apply angular constraints
		spring_arm.angles_spring.set_target_value(math::clamp(spring_arm.angles_spring.get_target_value(), spring_arm.min_angles, spring_arm.max_angles));
		
		// Solve springs
		spring_arm.focal_point_spring.solve(variable_timestep);
		spring_arm.angles_spring.solve(variable_timestep);
		
		// Recalculate zoom
		// if (spring_arm.pitch_velocity)
		{
			spring_arm.zoom = animation::ease<double, double>::in_sine(1.0, 0.0, spring_arm.angles_spring.get_value().x() / -math::half_pi<double>);
		}
		
		// Clamp zoom
		spring_arm.zoom = math::clamp(spring_arm.zoom, 0.0, 1.0);
		
		// Update FoV
		spring_arm.hfov = animation::ease<double, double>::out_sine(spring_arm.far_hfov, spring_arm.near_hfov, spring_arm.zoom);
		spring_arm.vfov = math::vertical_fov(spring_arm.hfov, static_cast<double>(camera.get_aspect_ratio()));
		
		// Update focal plane size
		spring_arm.focal_plane_height = animation::ease<double, double>::out_sine(spring_arm.far_focal_plane_height, spring_arm.near_focal_plane_height, spring_arm.zoom);
		spring_arm.focal_plane_width = spring_arm.focal_plane_height * static_cast<double>(camera.get_aspect_ratio());
		
		// Update focal distance
		spring_arm.focal_distance = spring_arm.focal_plane_height * 0.5 / math::tan(spring_arm.vfov * 0.5);
		
		const auto camera_up = spring_arm.up_rotation * math::dvec3{0, 1, 0};
		const auto parent_up = parent_transform.rotation * math::dvec3{0, 1, 0};
		spring_arm.up_rotation = math::normalize(math::rotation(camera_up, parent_up) * spring_arm.up_rotation);
		
		// Update camera rotation
		spring_arm.camera_rotation = math::normalize(spring_arm.up_rotation * math::euler_xyz_to_quat(spring_arm.angles_spring.get_value()));
		
		// Update transform
		const auto camera_translation = spring_arm.focal_point_spring.get_value() + spring_arm.camera_rotation * math::dvec3{0.0f, 0.0f, spring_arm.focal_distance};
		
		math::transform<float> camera_transform;
		camera_transform.translation = math::fvec3(camera_translation);
		camera_transform.rotation = math::fquat(spring_arm.camera_rotation);
		camera_transform.scale = {1, 1, 1};
		
		
		double center_offset = (1.0 - math::abs(spring_arm.angles_spring.get_value().x()) / math::half_pi<double>) * (spring_arm.focal_plane_height / 3.0 * 0.5);
		camera_transform.translation += math::fvec3(spring_arm.camera_rotation * math::dvec3{0, center_offset, 0});
		
		camera.set_transform(camera_transform);
		camera.set_vertical_fov(static_cast<float>(spring_arm.vfov));
	}
}
