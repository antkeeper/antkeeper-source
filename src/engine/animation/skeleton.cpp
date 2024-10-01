// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/skeleton.hpp>
#include <engine/resources/deserializer.hpp>
#include <engine/resources/deserialize-error.hpp>
#include <engine/resources/resource-loader.hpp>
#include <stdexcept>
#include <format>
#include <nlohmann/json.hpp>

skeleton::skeleton(std::size_t bone_count)
{
	m_bones = bone_container(*this, bone_count);
	m_rest_pose = skeleton_rest_pose(*this);
}

skeleton::skeleton(const skeleton& other)
{
	*this = other;
}

skeleton::skeleton(skeleton&& other)
{
	*this = other;
}

skeleton& skeleton::operator=(const skeleton& other)
{
	m_name = other.m_name;
	m_bones = other.m_bones;
	m_rest_pose = other.m_rest_pose;

	// Fix skeleton pointers
	m_bones.m_skeleton = this;
	for (auto& bone: m_bones.m_bones)
	{
		bone.m_skeleton = this;
	}
	m_rest_pose.m_skeleton = this;

	return *this;
}

skeleton& skeleton::operator=(skeleton&& other)
{
	m_name = std::move(other.m_name);
	m_bones = std::move(other.m_bones);
	m_rest_pose = std::move(other.m_rest_pose);

	// Fix skeleton pointers
	m_bones.m_skeleton = this;
	for (auto& bone: m_bones.m_bones)
	{
		bone.m_skeleton = this;
	}
	m_rest_pose.m_skeleton = this;

	return *this;
}

void skeleton::rename(const std::string& name)
{
	m_name = name;
}

/// Deserializes a skeleton.
/// @param[out] skeleton Skeleton to deserialize.
/// @param[in,out] ctx Deserialize context.
/// @throw deserialize_error Read error.
template <>
void deserializer<skeleton>::deserialize(skeleton& skeleton, deserialize_context& ctx)
{
	// Read file into buffer
	std::string file_buffer(ctx.size(), '\0');
	ctx.read8(reinterpret_cast<std::byte*>(file_buffer.data()), ctx.size());
	
	// Parse JSON from file buffer
	const auto json = nlohmann::json::parse(file_buffer, nullptr, true, true);

	// Check version string
	const auto& version = json.at("version").get_ref<const std::string&>();
	if (version != "1.0.0")
	{
		throw deserialize_error(std::format("Unsupported skeleton format (version {}).", version));
	}

	// Allocate bones
	skeleton = ::skeleton(json.at("bones").size());
	
	// Set skeleton name
	skeleton.rename(json.at("name"));

	// Initialize bones
	const auto& bones_element = json.at("bones");
	for (auto& bone: skeleton.bones())
	{
		const auto& bone_element = bones_element.at(bone.index());

		bone.rename(bone_element.at("name").get_ref<const std::string&>());
		bone.length() = bone_element.at("length").get<float>();

		math::transform<float> bone_pose;

		const auto& translation_element = bone_element.at("translation");
		bone_pose.translation.x() = translation_element.at(0).get<float>();
		bone_pose.translation.y() = translation_element.at(1).get<float>();
		bone_pose.translation.z() = translation_element.at(2).get<float>();

		const auto& rotation_element = bone_element.at("rotation");
		bone_pose.rotation.w() = rotation_element.at(0).get<float>();
		bone_pose.rotation.x() = rotation_element.at(1).get<float>();
		bone_pose.rotation.y() = rotation_element.at(2).get<float>();
		bone_pose.rotation.z() = rotation_element.at(3).get<float>();

		bone_pose.scale.x() = 1.0f;
		bone_pose.scale.y() = 1.0f;
		bone_pose.scale.z() = 1.0f;

		skeleton.rest_pose().set_relative_transform(bone.index(), bone_pose);
	}

	// Build bone hierarchy
	for (auto& bone: skeleton.bones())
	{
		const auto& bone_element = bones_element.at(bone.index());
		const auto& parent_element = bone_element.at("parent");

		if (!parent_element.is_null())
		{
			bone.reparent(&skeleton.bones().at(parent_element.get_ref<const std::string&>()));
		}
	}

	// Update rest post
	skeleton.rest_pose().update();
}

template <>
std::unique_ptr<skeleton> resource_loader<skeleton>::load([[maybe_unused]] ::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
{
	auto resource = std::make_unique<skeleton>();
	
	deserializer<skeleton>().deserialize(*resource, *ctx);
	
	return resource;
}

