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

#include "game/systems/camera-system.hpp"
#include "game/components/orbit-camera-component.hpp"
#include "game/components/scene-component.hpp"
#include <engine/animation/ease.hpp>
#include <engine/math/projection.hpp>
#include <engine/scene/camera.hpp>
#include <execution>

camera_system::camera_system(entity::registry& registry):
	updatable_system(registry)
{}

void camera_system::update(float t, float dt)
{

}

void camera_system::interpolate(float alpha)
{
	auto orbit_cam_group = registry.group<orbit_camera_component>(entt::get<scene_component>);
	std::for_each
	(
		std::execution::seq,
		orbit_cam_group.begin(),
		orbit_cam_group.end(),
		[&](auto entity_id)
		{
			auto& orbit_cam = orbit_cam_group.get<orbit_camera_component>(entity_id);
			auto& scene = orbit_cam_group.get<scene_component>(entity_id);
			auto& camera = static_cast<scene::camera&>(*scene.object);
			
			math::transform<double> subject_transform = math::transform<double>::identity();
			if (orbit_cam.subject_eid != entt::null)
			{
				const auto subject_scene = registry.try_get<scene_component>(orbit_cam.subject_eid);
				if (subject_scene)
				{
					subject_transform.translation = math::dvec3(subject_scene->object->get_translation());
					subject_transform.rotation = math::dquat(subject_scene->object->get_rotation());
				}
			}
			
			// Calculate focal point
			const auto focal_point = subject_transform * orbit_cam.focal_point;
			
			// Clamp zoom
			orbit_cam.zoom = std::min<double>(std::max<double>(orbit_cam.zoom, 0.0), 1.0);
			
			// Update FoV
			orbit_cam.hfov = ease<double, double>::out_sine(orbit_cam.far_hfov, orbit_cam.near_hfov, orbit_cam.zoom);
			orbit_cam.vfov = math::vertical_fov(orbit_cam.hfov, static_cast<double>(camera.get_aspect_ratio()));
			
			// Update focal plane size
			orbit_cam.focal_plane_height = ease<double, double>::out_sine(orbit_cam.far_focal_plane_height, orbit_cam.near_focal_plane_height, orbit_cam.zoom);
			orbit_cam.focal_plane_width = orbit_cam.focal_plane_height * static_cast<double>(camera.get_aspect_ratio());
			
			// Update focal distance
			orbit_cam.focal_distance = orbit_cam.focal_plane_height * 0.5 / std::tan(orbit_cam.vfov * 0.5);
			
			const auto camera_up = orbit_cam.up_rotation * math::dvec3{0, 1, 0};
			const auto subject_up = subject_transform.rotation * math::dvec3{0, 1, 0};
			orbit_cam.up_rotation = math::normalize(math::rotation(camera_up, subject_up) * orbit_cam.up_rotation);
			
			// Update orientation
			orbit_cam.yaw_rotation = math::angle_axis(orbit_cam.yaw, {0.0, 1.0, 0.0});
			orbit_cam.pitch_rotation = math::angle_axis(orbit_cam.pitch, {-1.0, 0.0, 0.0});
			orbit_cam.orientation = math::normalize(orbit_cam.up_rotation * math::normalize(orbit_cam.yaw_rotation * orbit_cam.pitch_rotation));
			// orbit_cam.orientation = math::normalize(subject_transform.rotation * math::normalize(orbit_cam.yaw_rotation * orbit_cam.pitch_rotation));
			// orbit_cam.orientation = math::normalize(math::normalize(orbit_cam.yaw_rotation * orbit_cam.pitch_rotation));
			
			// Update transform
			const auto camera_translation = focal_point + orbit_cam.orientation * math::dvec3{0.0f, 0.0f, orbit_cam.focal_distance};
			
			math::transform<float> camera_transform;
			camera_transform.translation = math::fvec3(camera_translation);
			camera_transform.rotation = math::fquat(orbit_cam.orientation);
			camera_transform.scale = {1, 1, 1};
			
			
			// double center_offset = (1.0 - std::abs(orbit_cam.pitch) / math::half_pi<double>) * (orbit_cam.focal_plane_height / 3.0 * 0.5);
			// camera_transform.translation += math::fvec3(orbit_cam.orientation * math::dvec3{0, center_offset, 0});
			
			camera.set_transform(camera_transform);
			camera.set_perspective(static_cast<float>(orbit_cam.vfov), camera.get_aspect_ratio(), camera.get_clip_near(), camera.get_clip_far());
		}
	);
}

void camera_system::set_viewport(const math::fvec4& viewport)
{
	m_viewport = math::dvec4(viewport);
	m_aspect_ratio = m_viewport[2] /  m_viewport[3];
}

