// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/skeletal-animation.hpp>
#include <engine/animation/animation-sequence.hpp>
#include <engine/animation/skeleton.hpp>
#include <engine/math/euler-angles.hpp>
#include <engine/debug/log.hpp>
#include <filesystem>
#include <format>

void bind_skeletal_animation(animation_sequence& sequence, [[maybe_unused]] const ::skeleton& skeleton)
{
	for (auto& [key, track]: sequence.tracks())
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

		// Set track sampler according to bone and property
		if (property_name == "translation")
		{
			track.sampler() = [bone_index](void* context, auto sample)
			{
				const auto translation = math::fvec3{sample[0], sample[1], sample[2]};
				static_cast<skeleton_pose*>(context)->set_relative_translation(bone_index, translation);
			};
		}
		else if (property_name == "rotation_quaternion")
		{
			track.sampler() = [bone_index](void* context, auto sample)
			{
				const auto rotation = math::normalize(math::fquat{sample[0], sample[1], sample[2], sample[3]});

				debug::log_debug("rotated bone {} to {}", bone_index, rotation);
				static_cast<skeleton_pose*>(context)->set_relative_rotation(bone_index, rotation);
			};
		}
		else if (property_name == "rotation_euler")
		{
			track.sampler() = [bone_index](void* context, auto sample)
			{
				const auto rotation = math::euler_xyz_to_quat(math::fvec3{sample[0], sample[1], sample[2]});
				static_cast<skeleton_pose*>(context)->set_relative_rotation(bone_index, rotation);
			};
		}
		else if (property_name == "scale")
		{
			track.sampler() = [bone_index](void* context, auto sample)
			{
				const auto scale = math::fvec3{sample[0], sample[1], sample[2]};
				static_cast<skeleton_pose*>(context)->set_relative_scale(bone_index, scale);
			};
		}
		else
		{
			throw std::runtime_error(std::format("Failed to bind animation track to bone: unsupported property \"{}\".", property_name));
		}
	}
}

