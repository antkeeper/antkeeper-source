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

#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/resources/deserializer.hpp>
#include <engine/render/model.hpp>
#include <engine/render/vertex-attribute.hpp>
#include <engine/gl/vertex-attribute.hpp>
#include <engine/gl/drawing-mode.hpp>
#include <engine/math/numbers.hpp>
#include <cstdint>

inline constexpr std::uint16_t vertex_attribute_position     = 0b0000000000000001;
inline constexpr std::uint16_t vertex_attribute_uv           = 0b0000000000000010;
inline constexpr std::uint16_t vertex_attribute_normal       = 0b0000000000000100;
inline constexpr std::uint16_t vertex_attribute_tangent      = 0b0000000000001000;
inline constexpr std::uint16_t vertex_attribute_color        = 0b0000000000010000;
inline constexpr std::uint16_t vertex_attribute_bone         = 0b0000000000100000;
inline constexpr std::uint16_t vertex_attribute_barycentric  = 0b0000000001000000;
inline constexpr std::uint16_t vertex_attribute_morph_target = 0b0000000010000000;
inline constexpr std::uint16_t vertex_attribute_index        = 0b0000000100000000;

template <>
render::model* resource_loader<render::model>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	deserialize_context ctx(file);
	
	// Read vertex format
	std::uint16_t vertex_format_flags = 0;
	ctx.read16<std::endian::little>(reinterpret_cast<std::byte*>(&vertex_format_flags), 1);
	
	// Read bone per vertex (if any)
	std::uint8_t bones_per_vertex = 0;
	if (vertex_format_flags & vertex_attribute_bone)
	{
		ctx.read8(reinterpret_cast<std::byte*>(&bones_per_vertex), 1);
	}
	
	// Read vertex count
	std::uint32_t vertex_count = 0;
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&vertex_count), 1);
	
	// Determine vertex size
	std::size_t vertex_size = 0;
	if (vertex_format_flags & vertex_attribute_position)
	{
		vertex_size += sizeof(float) * 3;
	}
	if (vertex_format_flags & vertex_attribute_uv)
	{
		vertex_size += sizeof(float) * 2;
	}
	if (vertex_format_flags & vertex_attribute_normal)
	{
		vertex_size += sizeof(float) * 3;
	}
	if (vertex_format_flags & vertex_attribute_tangent)
	{
		vertex_size += sizeof(float) * 4;
	}
	if (vertex_format_flags & vertex_attribute_color)
	{
		vertex_size += sizeof(float) * 4;
	}
	if (vertex_format_flags & vertex_attribute_bone)
	{
		vertex_size += sizeof(std::uint32_t) * bones_per_vertex;
		vertex_size += sizeof(float) * bones_per_vertex;
	}
	if (vertex_format_flags & vertex_attribute_barycentric)
	{
		vertex_size += sizeof(float) * 3;
	}
	if (vertex_format_flags & vertex_attribute_morph_target)
	{
		vertex_size += sizeof(float) * 3;
	}
	
	// Allocate vertex data
	std::byte* vertex_data = new std::byte[vertex_count * vertex_size];
	
	// Read vertices
	if constexpr (std::endian::native == std::endian::little)
	{
		ctx.read8(vertex_data, vertex_count * vertex_size);
	}
	else
	{
		std::byte* vertex_data_offset = vertex_data;
		for (std::size_t i = 0; i < vertex_count; ++i)
		{
			if (vertex_format_flags & vertex_attribute_position)
			{
				ctx.read32<std::endian::little>(vertex_data_offset, 3);
				vertex_data_offset += sizeof(float) * 3;
			}
			if (vertex_format_flags & vertex_attribute_uv)
			{
				ctx.read32<std::endian::little>(vertex_data_offset, 2);
				vertex_data_offset += sizeof(float) * 2;
			}
			if (vertex_format_flags & vertex_attribute_normal)
			{
				ctx.read32<std::endian::little>(vertex_data_offset, 3);
				vertex_data_offset += sizeof(float) * 3;
			}
			if (vertex_format_flags & vertex_attribute_tangent)
			{
				ctx.read32<std::endian::little>(vertex_data_offset, 4);
				vertex_data_offset += sizeof(float) * 4;
			}
			if (vertex_format_flags & vertex_attribute_color)
			{
				ctx.read32<std::endian::little>(vertex_data_offset, 4);
				vertex_data_offset += sizeof(float) * 4;
			}
			if (vertex_format_flags & vertex_attribute_bone)
			{
				ctx.read32<std::endian::little>(vertex_data_offset, bones_per_vertex);
				ctx.read32<std::endian::little>(vertex_data_offset, bones_per_vertex);
				
				vertex_data_offset += sizeof(std::uint32_t) * bones_per_vertex;
				vertex_data_offset += sizeof(float) * bones_per_vertex;
			}
			if (vertex_format_flags & vertex_attribute_barycentric)
			{
				ctx.read32<std::endian::little>(vertex_data_offset, 3);
				vertex_data_offset += sizeof(float) * 3;
			}
			if (vertex_format_flags & vertex_attribute_morph_target)
			{
				ctx.read32<std::endian::little>(vertex_data_offset, 3);
				vertex_data_offset += sizeof(float) * 3;
			}
		}
	}
	
	// Read geometry bounds
	geom::aabb<float> bounds;
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(bounds.min_point.data()), 3);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(bounds.max_point.data()), 3);
	
	// Allocate a model
	render::model* model = new render::model();
	
	// Set the model bounds
	model->set_bounds(bounds);
	
	// Resize model VBO and upload vertex data
	gl::vertex_buffer* vbo = model->get_vertex_buffer();
	vbo->resize(vertex_count * vertex_size, vertex_data);
	
	// Free vertex data
	delete[] vertex_data;
	
	// Bind vertex attributes to VAO
	gl::vertex_array* vao = model->get_vertex_array();
	gl::vertex_attribute attribute;
	attribute.buffer = vbo;
	attribute.offset = 0;
	attribute.stride = vertex_size;
	if (vertex_format_flags & vertex_attribute_position)
	{
		attribute.type = gl::vertex_attribute_type::float_32;
		attribute.components = 3;
		vao->bind(render::vertex_attribute::position, attribute);
		attribute.offset += sizeof(float) * attribute.components;
	}
	if (vertex_format_flags & vertex_attribute_uv)
	{
		attribute.type = gl::vertex_attribute_type::float_32;
		attribute.components = 2;
		vao->bind(render::vertex_attribute::uv, attribute);
		attribute.offset += sizeof(float) * attribute.components;
	}
	if (vertex_format_flags & vertex_attribute_normal)
	{
		attribute.type = gl::vertex_attribute_type::float_32;
		attribute.components = 3;
		vao->bind(render::vertex_attribute::normal, attribute);
		attribute.offset += sizeof(float) * attribute.components;
	}
	if (vertex_format_flags & vertex_attribute_tangent)
	{
		attribute.type = gl::vertex_attribute_type::float_32;
		attribute.components = 4;
		vao->bind(render::vertex_attribute::tangent, attribute);
		attribute.offset += sizeof(float) * attribute.components;
	}
	if (vertex_format_flags & vertex_attribute_color)
	{
		attribute.type = gl::vertex_attribute_type::float_32;
		attribute.components = 4;
		vao->bind(render::vertex_attribute::color, attribute);
		attribute.offset += sizeof(float) * attribute.components;
	}
	if (vertex_format_flags & vertex_attribute_bone)
	{
		attribute.type = gl::vertex_attribute_type::uint_16;
		attribute.components = bones_per_vertex;
		vao->bind(render::vertex_attribute::bone_index, attribute);
		attribute.offset += sizeof(std::uint32_t) * attribute.components;
		
		attribute.type = gl::vertex_attribute_type::float_32;
		attribute.components = bones_per_vertex;
		vao->bind(render::vertex_attribute::bone_weight, attribute);
		attribute.offset += sizeof(float) * attribute.components;
	}
	if (vertex_format_flags & vertex_attribute_barycentric)
	{
		attribute.type = gl::vertex_attribute_type::float_32;
		attribute.components = 3;
		vao->bind(render::vertex_attribute::barycentric, attribute);
		attribute.offset += sizeof(float) * attribute.components;
	}
	if (vertex_format_flags & vertex_attribute_morph_target)
	{
		attribute.type = gl::vertex_attribute_type::float_32;
		attribute.components = 3;
		vao->bind(render::vertex_attribute::target, attribute);
		attribute.offset += sizeof(float) * attribute.components;
	}
	
	// Read material count
	std::uint16_t material_count = 0;
	ctx.read16<std::endian::little>(reinterpret_cast<std::byte*>(&material_count), 1);
	
	// Read materials
	for (std::uint16_t i = 0; i < material_count; ++i)
	{
		// Read material name length
		std::uint8_t material_name_length = 0;
		ctx.read8(reinterpret_cast<std::byte*>(&material_name_length), 1);
		
		// Read material name
		std::string material_name(static_cast<std::size_t>(material_name_length), '\0');
		ctx.read8(reinterpret_cast<std::byte*>(material_name.data()), material_name_length);
		
		// Read offset to index of first vertex
		std::uint32_t material_vertex_offset = 0;
		ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&material_vertex_offset), 1);
		
		// Read vertex count
		std::uint32_t material_vertex_count = 0;
		ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&material_vertex_count), 1);
		
		// Slugify material filename
		std::string material_filename = material_name + ".mtl";
		std::replace(material_filename.begin(), material_filename.end(), '_', '-');
		
		// Load material from file
		render::material* material = resource_manager->load<render::material>(material_filename);
		
		// Create model material group
		render::model_group* material_group = model->add_group(material_name);
		material_group->set_drawing_mode(gl::drawing_mode::triangles);
		material_group->set_start_index(material_vertex_offset);
		material_group->set_index_count(material_vertex_count);
		material_group->set_material(material);
	}
	
	// Read skeleton
	if (vertex_format_flags & vertex_attribute_bone)
	{
		::skeleton& skeleton = model->get_skeleton();
		pose& bind_pose = skeleton.bind_pose;
		
		// Read bone count
		std::uint16_t bone_count = 0;
		ctx.read16<std::endian::little>(reinterpret_cast<std::byte*>(&bone_count), 1);
		
		// Read bones
		for (std::uint16_t i = 0; i < bone_count; ++i)
		{
			// Read bone name length
			std::uint8_t bone_name_length = 0;
			ctx.read8(reinterpret_cast<std::byte*>(&bone_name_length), 1);
			
			// Read bone name
			std::string bone_name(static_cast<std::size_t>(bone_name_length), '\0');
			ctx.read8(reinterpret_cast<std::byte*>(bone_name.data()), bone_name_length);
			
			// Read parent bone index
			std::uint16_t parent_bone_index = i;
			ctx.read16<std::endian::little>(reinterpret_cast<std::byte*>(&parent_bone_index), 1);
			
			// Construct bone identifier
			::bone bone = make_bone(i, parent_bone_index);
			
			// Add bone to bone map
			skeleton.bone_map[bone_name] = bone;
			
			// Get reference to the bone's bind pose transform
			auto& bone_transform = bind_pose[bone];
			
			// Read bone translation
			ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(bone_transform.translation.data()), 3);
			
			// Read bone rotation
			ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&bone_transform.rotation.r), 1);
			ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(bone_transform.rotation.i.data()), 3);
			
			// Set bone scale
			bone_transform.scale = {1, 1, 1};
			
			// Read bone length
			float bone_length = 0.0f;
			ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&bone_length), 1);
		}
		
		// Calculate inverse skeleton-space bind pose
		::concatenate(skeleton.bind_pose, skeleton.inverse_bind_pose);
		::inverse(skeleton.inverse_bind_pose, skeleton.inverse_bind_pose);
	}
	
	return model;
}
