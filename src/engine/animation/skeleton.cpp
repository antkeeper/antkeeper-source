// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/skeleton.hpp>
#include <engine/resources/deserializer.hpp>
#include <engine/resources/deserialize-error.hpp>
#include <engine/resources/resource-loader.hpp>
#include <stdexcept>
#include <nlohmann/json.hpp>

skeleton::skeleton(std::size_t bone_count):
	m_bone_parents(bone_count, 0)
{
	m_rest_pose.set_skeleton(*this);
}

skeleton::skeleton():
	skeleton(0)
{}

void skeleton::update_rest_pose()
{
	m_rest_pose.update();
}

bone_index_type skeleton::add_bones(std::size_t bone_count)
{
	const bone_index_type first_bone_index = static_cast<bone_index_type>(m_bone_parents.size());
	
	m_bone_parents.resize(m_bone_parents.size() + bone_count, 0);
	m_rest_pose.resize();
	
	return first_bone_index;
}

bone_index_type skeleton::add_bone(const std::string& name)
{
	const bone_index_type bone_index = add_bone();
	
	set_bone_name(bone_index, name);
	
	return bone_index;
}

void skeleton::remove_bones()
{
	m_bone_parents.clear();
	m_bone_map.clear();
	m_rest_pose.resize();
}

animation_pose& skeleton::add_pose(const std::string& name)
{
	const auto [iterator, inserted] = m_pose_map.emplace(name, *this);
	
	if (!inserted)
	{
		throw std::invalid_argument("Duplicate pose name");
	}
	
	return iterator->second;
}

void skeleton::remove_pose(const std::string& name)
{
	if (!m_pose_map.erase(name))
	{
		throw std::invalid_argument("Pose not found");
	}	
}

void skeleton::remove_poses()
{
	m_pose_map.clear();
}

void skeleton::set_name(const std::string& name)
{
	m_name = name;
}

void skeleton::set_bone_parent(bone_index_type child_index, bone_index_type parent_index)
{
	if (child_index < parent_index)
	{
		throw std::invalid_argument("Child bone index precedes parent bone index");
	}
	
	m_bone_parents[child_index] = parent_index;
}

void skeleton::set_bone_name(bone_index_type index, const std::string& name)
{
	if (auto i = m_bone_map.find(name); i != m_bone_map.end())
	{
		if (i->second != index)
		{
			throw std::invalid_argument("Duplicate bone name");
		}
	}
	else
	{
		m_bone_map[name] = index;
	}
}

std::optional<bone_index_type> skeleton::get_bone_index(const std::string& name) const
{
	if (auto i = m_bone_map.find(name); i != m_bone_map.end())
	{
		return i->second;
	}
	
	return std::nullopt;
}

const animation_pose* skeleton::get_pose(const std::string& name) const
{
	if (auto i = m_pose_map.find(name); i != m_pose_map.end())
	{
		return &i->second;
	}
	
	return nullptr;
}

animation_pose* skeleton::get_pose(const std::string& name)
{
	if (auto i = m_pose_map.find(name); i != m_pose_map.end())
	{
		return &i->second;
	}
	
	return nullptr;
}


/**
 * Deserializes a skeleton.
 *
 * @param[out] skeleton Skeleton to deserialize.
 * @param[in,out] ctx Deserialize context.
 *
 * @throw deserialize_error Read error.
 */
template <>
void deserializer<skeleton>::deserialize(skeleton& skeleton, deserialize_context& ctx)
{
	// Read file into buffer
	std::string file_buffer(ctx.size(), '\0');
	ctx.read8(reinterpret_cast<std::byte*>(file_buffer.data()), ctx.size());
	
	// Parse JSON from file buffer
	const auto json = nlohmann::json::parse(file_buffer, nullptr, true, true);
	
	// Set skeleton name
	skeleton.set_name(json.at("name"));

	// Allocate bones
	skeleton.add_bones(json.at("bones").size());

	// Set bone names
	bone_index_type bone_index = 0;
	for (const auto& element: json.at("bones"))
	{
		skeleton.set_bone_name(bone_index, element.at("name").get<std::string>());
		++bone_index;
	}

	// Set bone parents
	bone_index = 0;
	for (const auto& element: json.at("bones"))
	{
		const auto& parent = element.at("parent");

		if (parent.is_null())
		{
			skeleton.set_bone_parent(bone_index, bone_index);
		}
		else
		{
			skeleton.set_bone_parent(bone_index, *skeleton.get_bone_index(parent.get<std::string>()));
		}

		++bone_index;
	}
}

template <>
std::unique_ptr<skeleton> resource_loader<skeleton>::load([[maybe_unused]] ::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
{
	auto resource = std::make_unique<skeleton>();
	
	deserializer<skeleton>().deserialize(*resource, *ctx);
	
	return resource;
}

