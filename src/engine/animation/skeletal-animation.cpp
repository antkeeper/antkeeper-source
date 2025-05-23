// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/components/scene-object-component.hpp"
import engine.animation.skeletal_animation;
import engine.animation.skeleton;
import engine.animation.animation_sequence;
import engine.scene.skeletal_mesh;
import engine.debug.log;
import engine.math.euler_angles;
import <filesystem>;
import <format>;
import <stdexcept>;

namespace engine::animation
{
	void bind_skeletal_animation(animation_sequence& sequence, const animation::skeleton& skeleton)
	{
		for (auto& [key, track] : sequence.tracks())
		{
			// Build path from track key
			const auto path = std::filesystem::path(key);

			// Extract bone name from path
			const auto bone_name = path.parent_path().string();
			if (bone_name.empty())
			{
				throw std::runtime_error("Failed to bind animation track to bone: invalid data path.");
			}

			// Find bone in skeleton by name
			const auto bone_it = skeleton.bones().find(bone_name);
			if (bone_it == skeleton.bones().end())
			{
				throw std::runtime_error(std::format("Failed to bind animation track to bone: bone \"{}\" not found.", bone_name));
			}

			// Get bone index
			const auto bone_index = bone_it->index();

			// Extract property name from path
			const auto property_name = path.filename().string();
			if (property_name.empty())
			{
				throw std::runtime_error("Failed to bind animation track to bone: invalid data path.");
			}

			// Set track output function according to bone and property
			if (property_name == "translation")
			{
				track.output() = [bone_index](auto samples, auto& context)
					{
						auto& scene_object = *(context.handle.template get<scene_object_component>().object);
						auto& skeletal_mesh = static_cast<scene::skeletal_mesh&>(scene_object);

						const auto translation = math::fvec3{samples[0], samples[1], samples[2]};

						skeletal_mesh.get_pose().set_relative_translation(bone_index, translation);
					};
			}
			else if (property_name == "rotation_quaternion")
			{
				track.output() = [bone_index](auto samples, auto& context)
					{
						auto& scene_object = *(context.handle.template get<scene_object_component>().object);
						auto& skeletal_mesh = static_cast<scene::skeletal_mesh&>(scene_object);

						const auto rotation = math::normalize(math::fquat{samples[0], samples[1], samples[2], samples[3]});

						skeletal_mesh.get_pose().set_relative_rotation(bone_index, rotation);
					};
			}
			else if (property_name == "rotation_euler")
			{
				track.output() = [bone_index](auto samples, auto& context)
					{
						auto& scene_object = *(context.handle.template get<scene_object_component>().object);
						auto& skeletal_mesh = static_cast<scene::skeletal_mesh&>(scene_object);

						const auto rotation = math::euler_xyz_to_quat(math::fvec3{samples[0], samples[1], samples[2]});

						skeletal_mesh.get_pose().set_relative_rotation(bone_index, rotation);
					};
			}
			else if (property_name == "scale")
			{
				track.output() = [bone_index](auto samples, auto& context)
					{
						auto& scene_object = *(context.handle.template get<scene_object_component>().object);
						auto& skeletal_mesh = static_cast<scene::skeletal_mesh&>(scene_object);

						const auto scale = math::fvec3{samples[0], samples[1], samples[2]};

						skeletal_mesh.get_pose().set_relative_scale(bone_index, scale);
					};
			}
			else
			{
				throw std::runtime_error(std::format("Failed to bind animation track to bone: unsupported property \"{}\".", property_name));
			}
		}
	}
}
