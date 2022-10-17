/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include "resources/resource-loader.hpp"
#include "resources/resource-manager.hpp"
#include "render/model.hpp"
#include "render/vertex-attribute.hpp"
#include "gl/vertex-attribute.hpp"
#include "gl/drawing-mode.hpp"
#include "utility/fundamental-types.hpp"
#include "math/constants.hpp"
#include <sstream>
#include <stdexcept>
#include <limits>
#include <physfs.h>
#include <nlohmann/json.hpp>

static const float3 barycentric_coords[3] =
{
	float3{1, 0, 0},
	float3{0, 1, 0},
	float3{0, 0, 1}
};

template <>
render::model* resource_loader<render::model>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Read file into buffer
	std::size_t size = static_cast<int>(PHYSFS_fileLength(file));
	std::vector<std::uint8_t> buffer(size);
	PHYSFS_readBytes(file, &buffer.front(), size);
	
	// Parse CBOR in file buffer
	nlohmann::json json = nlohmann::json::from_cbor(buffer);
	
	// Find model name
	std::string model_name;
	if (auto it = json.find("name"); it != json.end())
		model_name = it.value();
	
	// Load attributes
	std::unordered_map<std::string, std::tuple<std::size_t, std::vector<float>>> attributes;
	if (auto attributes_node = json.find("attributes"); attributes_node != json.end())
	{
		for (const auto& attribute_node: attributes_node.value().items())
		{
			// Look up attribute name
			std::string attribute_name;
			if (auto type_node = attribute_node.value().find("name"); type_node != attribute_node.value().end())
				attribute_name = type_node.value().get<std::string>();
			
			// Allocate attribute in attribute map
			auto& attribute = attributes[attribute_name];
			std::size_t& attribute_size = std::get<0>(attribute);
			std::vector<float>& attribute_data = std::get<1>(attribute);
			
			// Look up attribute size (per vertex)
			attribute_size = 0;
			if (auto size_node = attribute_node.value().find("size"); size_node != attribute_node.value().end())
				attribute_size = size_node.value().get<std::size_t>();
			
			// Look up attribute data
			if (auto data_node = attribute_node.value().find("data"); data_node != attribute_node.value().end())
			{
				// Resize attribute data
				attribute_data.resize(data_node.value().size());
				
				// Fill attribute data
				float* v = &attribute_data.front();
				for (const auto& element: data_node.value())
					*(v++) = element.get<float>();
			}
		}
	}
	
	// Load bounds
	geom::aabb<float> bounds =
	{
		{std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()},
		{-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()}
	};
	if (auto bounds_node = json.find("bounds"); bounds_node != json.end())
	{
		if (auto min_node = bounds_node.value().find("min"); min_node != bounds_node.value().end())
		{
			float* v = &bounds.min_point.x();
			for (const auto& element: min_node.value())
				*(v++) = element.get<float>();
		}
		
		if (auto max_node = bounds_node.value().find("max"); max_node != bounds_node.value().end())
		{
			float* v = &bounds.max_point.x();
			for (const auto& element: max_node.value())
				*(v++) = element.get<float>();
		}
	}
	
	// Allocate a model
	render::model* model = new render::model();
	
	// Set the model bounds
	model->set_bounds(bounds);
	
	// Calculate vertex size, count, and stride
	std::size_t vertex_size = 0;
	std::size_t vertex_count = 0;
	for (auto it = attributes.begin(); it != attributes.end(); ++it)
	{
		vertex_size += std::get<0>(it->second);
		vertex_count = std::get<1>(it->second).size() / std::get<0>(it->second);
	}
	std::size_t vertex_stride = sizeof(float) * vertex_size;
	
	// Build interleaved vertex data buffer
	float* vertex_data = new float[vertex_size * vertex_count];
	float* v = &vertex_data[0];
	for (std::size_t i = 0; i < vertex_count; ++i)
	{
		for (auto it = attributes.begin(); it != attributes.end(); ++it)
		{
			std::size_t attribute_size = std::get<0>(it->second);
			const float* a = &(std::get<1>(it->second)[i * attribute_size]);
			
			for (std::size_t j = 0; j < attribute_size; ++j)
				*(v++) = *(a++);
		}
	}
	
	// Resize VBO and upload vertex data
	gl::vertex_buffer* vbo = model->get_vertex_buffer();
	vbo->resize(sizeof(float) * vertex_size * vertex_count, vertex_data);
	
	// Free interleaved vertex data buffer
	delete[] vertex_data;
	
	// Map attribute names to locations
	static const std::unordered_map<std::string, unsigned int> attribute_location_map =
	{
		{"position", render::vertex_attribute::position},
		{"texcoord", render::vertex_attribute::uv},
		{"normal", render::vertex_attribute::normal},
		{"tangent", render::vertex_attribute::tangent},
		{"color", render::vertex_attribute::color},
		{"bone_index", render::vertex_attribute::bone_index},
		{"bone_weight", render::vertex_attribute::bone_weight},
		{"barycentric", render::vertex_attribute::barycentric},
		{"target", render::vertex_attribute::target}
	};
	
	// Bind attributes to VAO
	gl::vertex_array* vao = model->get_vertex_array();
	std::size_t attribute_offset = 0;
	for (auto attribute_it = attributes.begin(); attribute_it != attributes.end(); ++attribute_it)
	{
		std::string attribute_name = attribute_it->first;
		
		if (auto location_it = attribute_location_map.find(attribute_name); location_it != attribute_location_map.end())
		{
			gl::vertex_attribute attribute;
			attribute.buffer = vbo;
			attribute.offset = attribute_offset;
			attribute.stride = vertex_stride;
			attribute.type = gl::vertex_attribute_type::float_32;
			attribute.components = std::get<0>(attribute_it->second);
			
			vao->bind(location_it->second, attribute);
			
			attribute_offset += attribute.components * sizeof(float);
		}
	}
	
	// Load materials
	if (auto materials_node = json.find("materials"); materials_node != json.end())
	{
		for (const auto& material_node: materials_node.value().items())
		{
			std::string group_name;
			std::size_t group_offset = 0;
			std::size_t group_size = 0;
			render::material* group_material = nullptr;
			
			if (auto name_node = material_node.value().find("name"); name_node != material_node.value().end())
				group_name = name_node.value().get<std::string>();
			if (auto offset_node = material_node.value().find("offset"); offset_node != material_node.value().end())
				group_offset = offset_node.value().get<std::size_t>();
			if (auto size_node = material_node.value().find("size"); size_node != material_node.value().end())
				group_size = size_node.value().get<std::size_t>();
			
			// Slugify material filename
			std::string material_filename = group_name + ".mtl";
			std::replace(material_filename.begin(), material_filename.end(), '_', '-');
			
			// Load material from file
			group_material = resource_manager->load<render::material>(material_filename);
			
			render::model_group* model_group = model->add_group(group_name);
			model_group->set_drawing_mode(gl::drawing_mode::triangles);
			model_group->set_start_index(group_offset * 3);
			model_group->set_index_count(group_size * 3);
			model_group->set_material(group_material);
		}
	}
	
	// Build skeleton
	if (auto skeleton_node = json.find("skeleton"); skeleton_node != json.end())
	{
		if (auto bones_node = skeleton_node->find("bones"); bones_node != skeleton_node->end())
		{
			::skeleton& skeleton = model->get_skeleton();
			pose& bind_pose = skeleton.bind_pose;
			
			std::uint8_t bone_index = 0;
			for (const auto& bone_node: bones_node.value())
			{
				// Find parent bone
				std::uint8_t bone_parent_index = bone_index;
				if (auto parent_node = bone_node.find("parent"); parent_node != bone_node.end())
				{
					if (!parent_node->is_null())
						bone_parent_index = parent_node->get<std::uint8_t>();
				}
				
				// Construct bone identifier
				::bone bone = make_bone(bone_index, bone_parent_index);
				
				// Get reference to the bone's bind pose transform
				auto& bone_transform = bind_pose[bone];
				
				// Get bone translation
				if (auto translation_node = bone_node.find("translation"); translation_node != bone_node.end())
				{
					if (translation_node->size() == 3)
					{
						bone_transform.translation.x() = (*translation_node)[0].get<float>();
						bone_transform.translation.y() = (*translation_node)[1].get<float>();
						bone_transform.translation.z() = (*translation_node)[2].get<float>();
					}
				}
				
				// Get bone rotation
				if (auto rotation_node = bone_node.find("rotation"); rotation_node != bone_node.end())
				{
					if (rotation_node->size() == 4)
					{
						bone_transform.rotation.w = (*rotation_node)[0].get<float>();
						bone_transform.rotation.x = (*rotation_node)[1].get<float>();
						bone_transform.rotation.y = (*rotation_node)[2].get<float>();
						bone_transform.rotation.z = (*rotation_node)[3].get<float>();
					}
				}
				
				// Set bone scale
				bone_transform.scale = {1, 1, 1};
				
				// Get bone length
				/*
				if (auto length_node = bone_node.find("length"); length_node != bone_node.end())
					bone.length = length_node->get<float>();
				else
					bone.length = 0.0f;
				*/
				
				// Get bone name
				if (auto name_node = bone_node.find("name"); name_node != bone_node.end())
				{
					// Add bone to bone map
					skeleton.bone_map[name_node->get<std::string>()] = bone;
				}
				
				++bone_index;
			}
			
			// Calculate inverse skeleton-space bind pose
			::concatenate(skeleton.bind_pose, skeleton.inverse_bind_pose);
			::inverse(skeleton.inverse_bind_pose, skeleton.inverse_bind_pose);
		}
	}
	
	return model;
}
