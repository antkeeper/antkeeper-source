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
#include <sstream>
#include <stdexcept>
#include <limits>
#include <physfs.h>
#include <iostream>
#include <nlohmann/json.hpp>

static const float3 barycentric_coords[3] =
{
	float3{1, 0, 0},
	float3{0, 1, 0},
	float3{0, 0, 1}
};

/*
template <>
model* resource_loader<model>::load(resource_manager* resource_manager, PHYSFS_File* file)
{
	std::string line;
	std::vector<float3> positions;
	std::vector<float2> uvs;
	std::vector<float3> normals;
	std::vector<float3> tangents;
	std::vector<float3> bitangents;
	std::vector<std::vector<std::size_t>> faces;
	std::vector<material_group> material_groups;
	material_group* current_material_group = nullptr;
	aabb<float> bounds =
	{
		{std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()},
		{-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()}
	};

	while (!PHYSFS_eof(file))
	{
		// Read line
		physfs_getline(file, line);
		
		// Tokenize line
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream linestream(line);
		while (linestream >> token)
			tokens.push_back(token);
		
		// Skip empty lines and comments
		if (tokens.empty() || tokens[0][0] == '#')
			continue;
		
		if (tokens[0] == "v")
		{
			if (tokens.size() != 4)
			{
				std::stringstream stream;
				stream << "resource_loader<mesh>::load(): Invalid line \"" << line << "\"" << std::endl;
				throw std::runtime_error(stream.str());
			}
			
			float3 position;
			
			std::stringstream(tokens[1]) >> position[0];
			std::stringstream(tokens[2]) >> position[1];
			std::stringstream(tokens[3]) >> position[2];

			positions.push_back(position);
			
			// Add position to bounds
			for (int i = 0; i < 3; ++i)
			{
				bounds.min_point[i] = std::min<float>(bounds.min_point[i], position[i]);
				bounds.max_point[i] = std::max<float>(bounds.max_point[i], position[i]);
			}
		}
		else if (tokens[0] == "vt")
		{
			if (tokens.size() != 3)
			{
				std::stringstream stream;
				stream << "resource_loader<mesh>::load(): Invalid line \"" << line << "\"" << std::endl;
				throw std::runtime_error(stream.str());
			}
			
			float2 uv;
			
			std::stringstream(tokens[1]) >> uv[0];
			std::stringstream(tokens[2]) >> uv[1];

			uvs.push_back(uv);
		}
		else if (tokens[0] == "vn")
		{
			if (tokens.size() != 4)
			{
				std::stringstream stream;
				stream << "resource_loader<mesh>::load(): Invalid line \"" << line << "\"" << std::endl;
				throw std::runtime_error(stream.str());
			}
			
			float3 normal;
			
			std::stringstream(tokens[1]) >> normal[0];
			std::stringstream(tokens[2]) >> normal[1];
			std::stringstream(tokens[3]) >> normal[2];

			normals.push_back(normal);
		}
		else if (tokens[0] == "f")
		{
			if (tokens.size() != 4)
			{
				std::stringstream stream;
				stream << "resource_loader<mesh>::load(): Invalid line \"" << line << "\"" << std::endl;
				throw std::runtime_error(stream.str());
			}
			
			std::vector<std::size_t> face;

			for (std::size_t i = 0; i < 3; ++i)
			{
				std::stringstream ss(tokens[i + 1]);
				while (ss.good())
				{
					std::string substring;
					std::getline(ss, substring, '/');

					if (!substring.empty())
					{
						std::size_t index = std::stoul(substring) - 1;
						face.push_back(index);
					}
				}
			}

			faces.push_back(face);
		}
		else if (tokens[0] == "usemtl")
		{
			if (tokens.size() != 2)
			{
				std::stringstream stream;
				stream << "resource_loader<mesh>::load(): Invalid line \"" << line << "\"" << std::endl;
				throw std::runtime_error(stream.str());
			}

			if (current_material_group)
			{
				current_material_group->index_count = faces.size() * 3 - current_material_group->start_index;
			}

			material_groups.push_back(material_group());
			current_material_group = &material_groups.back();
			current_material_group->name = tokens[1];
			current_material_group->start_index = faces.size() * 3;
			current_material_group->index_count = 0;
		}
	}

	if (current_material_group)
	{
		current_material_group->index_count = faces.size() * 3 - current_material_group->start_index;
	}

	// Load material group materials
	for (material_group& material_group: material_groups)
	{
		material_group.material = resource_manager->load<material>(material_group.name + ".mtl");
	}

	bool has_uvs = (!uvs.empty());
	bool has_normals = (!normals.empty());
	bool has_tangents = (has_uvs && has_normals);
	bool has_barycentric = false;
	has_barycentric = true;
	
	// Calculate faceted tangents and bitangents
	if (has_tangents)
	{
		tangents.resize(positions.size());
		bitangents.resize(positions.size());
		for (std::size_t i = 0; i < positions.size(); ++i)
		{
			tangents[i] = {0.0f, 0.0f, 0.0f};
			bitangents[i] = {0.0f, 0.0f, 0.0f};
		}
		
		for (std::size_t i = 0; i < faces.size(); ++i)
		{
			const std::vector<std::size_t>& face = faces[i];
			
			std::size_t ia = face[0];
			std::size_t ib = face[3];
			std::size_t ic = face[6];
			
			const float3& a = positions[ia];
			const float3& b = positions[ib];
			const float3& c = positions[ic];
			const float2& uva = uvs[face[1]];
			const float2& uvb = uvs[face[4]];
			const float2& uvc = uvs[face[7]];
			
			float3 ba = b - a;
			float3 ca = c - a;
			float2 uvba = uvb - uva;
			float2 uvca = uvc - uva;
			
			float f = 1.0f / (uvba.x * uvca.y - uvca.x * uvba.y);
			float3 tangent = (ba * uvca.y - ca * uvba.y) * f;
			float3 bitangent = (ba * -uvca.x + ca * uvba.x) * f;
			
			tangents[ia] += tangent;
			tangents[ib] += tangent;
			tangents[ic] += tangent;
			bitangents[ia] += bitangent;
			bitangents[ib] += bitangent;
			bitangents[ic] += bitangent;
		}
	}
	
	std::size_t vertex_size = 3;
	if (has_uvs)
		vertex_size += 2;
	if (has_normals)
		vertex_size += 3;
	if (has_tangents)
		vertex_size += 4;
	if (has_barycentric)
		vertex_size += 3;

	std::size_t vertex_stride = sizeof(float) * vertex_size;

	// Generate vertex buffer
	float* vertex_data = new float[vertex_size * faces.size() * 3];
	float* v = &vertex_data[0];
	for (std::size_t i = 0; i < faces.size(); ++i)
	{
		const std::vector<std::size_t>& face = faces[i];
		
		std::size_t k = 0;
		for (std::size_t j = 0; j < 3; ++j)
		{
			const float3& position = positions[face[k++]];
			*(v++) = position.x;
			*(v++) = position.y;
			*(v++) = position.z;

			if (has_uvs)
			{
				const float2& uv = uvs[face[k++]];
				*(v++) = uv.x;
				*(v++) = uv.y;
			}
			
			if (has_normals)
			{
				const float3& normal = normals[face[k++]];
				*(v++) = normal.x;
				*(v++) = normal.y;
				*(v++) = normal.z;
			}
			
			if (has_tangents)
			{
				const float3& n = normals[face[k - 1]];
				const float3& t = tangents[face[k - 3]];
				const float3& b = bitangents[face[k - 3]];
				
				// Gram-Schmidt orthogonalize tangent and bitangent
				float3 tangent = math::normalize(t - n * dot(n, t));
				float bitangent_sign = (math::dot(math::cross(n, t), b) < 0.0f) ? -1.0f : 1.0f;
				
				*(v++) = tangent.x;
				*(v++) = tangent.y;
				*(v++) = tangent.z;
				*(v++) = bitangent_sign;
			}

			if (has_barycentric)
			{
				*(v++) = barycentric_coords[j].x;
				*(v++) = barycentric_coords[j].y;
				*(v++) = barycentric_coords[j].z;
			}
		}
	}

	// Allocate a model
	model* model = new ::model();
	model->set_bounds(bounds);
	vertex_buffer* vbo = model->get_vertex_buffer();
	vertex_array* vao = model->get_vertex_array();
	vbo->resize(sizeof(float) * vertex_size * faces.size() * 3, vertex_data);
	std::size_t offset = 0;
	vao->bind_attribute(VERTEX_POSITION_LOCATION, *vbo, 3, vertex_attribute_type::float_32, vertex_stride, 0);
	offset += 3;
	if (has_uvs)
	{
		vao->bind_attribute(VERTEX_TEXCOORD_LOCATION, *vbo, 2, vertex_attribute_type::float_32, vertex_stride, sizeof(float) * offset);
		offset += 2;
	}
	if (has_normals)
	{
		vao->bind_attribute(VERTEX_NORMAL_LOCATION, *vbo, 3, vertex_attribute_type::float_32, vertex_stride, sizeof(float) * offset);
		offset += 3;
	}
	if (has_tangents)
	{
		vao->bind_attribute(VERTEX_TANGENT_LOCATION, *vbo, 4, vertex_attribute_type::float_32, vertex_stride, sizeof(float) * offset);
		offset += 4;
	}
	if (has_barycentric)
	{
		vao->bind_attribute(VERTEX_BARYCENTRIC_LOCATION, *vbo, 3, vertex_attribute_type::float_32, vertex_stride, sizeof(float) * offset);
		offset += 3;
	}

	// Add model groups for each material
	for (const material_group& material_group: material_groups)
	{
		model_group* model_group = model->add_group(material_group.name);
		model_group->set_material(material_group.material);
		model_group->set_drawing_mode(drawing_mode::triangles);
		model_group->set_start_index(material_group.start_index);
		model_group->set_index_count(material_group.index_count);
	}

	// Deallocate vertex data
	delete[] vertex_data;

	return model;
}
*/

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
			float* v = &bounds.min_point.x;
			for (const auto& element: min_node.value())
				*(v++) = element.get<float>();
		}
		
		if (auto max_node = bounds_node.value().find("max"); max_node != bounds_node.value().end())
		{
			float* v = &bounds.max_point.x;
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
	
	return model;
}
