// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/render/model.hpp>
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/render/vertex-attribute-location.hpp>
#include <engine/math/constants.hpp>
#include <engine/hash/fnv1a.hpp>
#include <engine/debug/log.hpp>
#include <bit>
#include <cstdint>

inline constexpr std::uint16_t vertex_attribute_position     = 0b0000000000000001;
inline constexpr std::uint16_t vertex_attribute_uv           = 0b0000000000000010;
inline constexpr std::uint16_t vertex_attribute_normal       = 0b0000000000000100;
inline constexpr std::uint16_t vertex_attribute_tangent      = 0b0000000000001000;
inline constexpr std::uint16_t vertex_attribute_color        = 0b0000000000010000;
inline constexpr std::uint16_t vertex_attribute_bone_index   = 0b0000000000100000;
inline constexpr std::uint16_t vertex_attribute_bone_weight  = 0b0000000001000000;
inline constexpr std::uint16_t vertex_attribute_morph_target = 0b0000000010000000;

template <>
std::unique_ptr<render::model> resource_loader<render::model>::load(::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
{
	// Read vertex format
	std::uint16_t vertex_format_flags = 0;
	ctx->read16<std::endian::little>(reinterpret_cast<std::byte*>(&vertex_format_flags), 1);
	
	// Read bones per vertex (if any)
	std::uint8_t bones_per_vertex = 0;
	if ((vertex_format_flags & vertex_attribute_bone_index) || (vertex_format_flags & vertex_attribute_bone_weight))
	{
		ctx->read8(reinterpret_cast<std::byte*>(&bones_per_vertex), 1);
	}
	
	// Read vertex count
	std::uint32_t vertex_count = 0;
	ctx->read32<std::endian::little>(reinterpret_cast<std::byte*>(&vertex_count), 1);
	
	// Determine vertex stride
	std::size_t vertex_stride = 0;
	if (vertex_format_flags & vertex_attribute_position)
	{
		vertex_stride += sizeof(float) * 3;
	}
	if (vertex_format_flags & vertex_attribute_uv)
	{
		vertex_stride += sizeof(float) * 2;
	}
	if (vertex_format_flags & vertex_attribute_normal)
	{
		vertex_stride += sizeof(float) * 3;
	}
	if (vertex_format_flags & vertex_attribute_tangent)
	{
		vertex_stride += sizeof(float) * 4;
	}
	if (vertex_format_flags & vertex_attribute_color)
	{
		vertex_stride += sizeof(float) * 4;
	}
	if (vertex_format_flags & vertex_attribute_bone_index)
	{
		vertex_stride += sizeof(std::uint16_t) * bones_per_vertex;
	}
	if (vertex_format_flags & vertex_attribute_bone_weight)
	{
		vertex_stride += sizeof(float) * bones_per_vertex;
	}
	if (vertex_format_flags & vertex_attribute_morph_target)
	{
		vertex_stride += sizeof(float) * 3;
	}
	
	// Allocate vertex data
	std::vector<std::byte> vertex_data(vertex_count * vertex_stride);
	
	// Read vertices
	if constexpr (std::endian::native == std::endian::little)
	{
		ctx->read8(vertex_data.data(), vertex_count * vertex_stride);
	}
	else
	{
		std::byte* vertex_data_offset = vertex_data.data();
		for (std::uint32_t i = 0; i < vertex_count; ++i)
		{
			if (vertex_format_flags & vertex_attribute_position)
			{
				ctx->read32<std::endian::little>(vertex_data_offset, 3);
				vertex_data_offset += sizeof(float) * 3;
			}
			if (vertex_format_flags & vertex_attribute_uv)
			{
				ctx->read32<std::endian::little>(vertex_data_offset, 2);
				vertex_data_offset += sizeof(float) * 2;
			}
			if (vertex_format_flags & vertex_attribute_normal)
			{
				ctx->read32<std::endian::little>(vertex_data_offset, 3);
				vertex_data_offset += sizeof(float) * 3;
			}
			if (vertex_format_flags & vertex_attribute_tangent)
			{
				ctx->read32<std::endian::little>(vertex_data_offset, 4);
				vertex_data_offset += sizeof(float) * 4;
			}
			if (vertex_format_flags & vertex_attribute_color)
			{
				ctx->read32<std::endian::little>(vertex_data_offset, 4);
				vertex_data_offset += sizeof(float) * 4;
			}
			if (vertex_format_flags & vertex_attribute_bone_index)
			{
				ctx->read16<std::endian::little>(vertex_data_offset, bones_per_vertex);
				vertex_data_offset += sizeof(std::uint16_t) * bones_per_vertex;
			}
			if (vertex_format_flags & vertex_attribute_bone_weight)
			{
				ctx->read32<std::endian::little>(vertex_data_offset, bones_per_vertex);
				vertex_data_offset += sizeof(float) * bones_per_vertex;
			}
			if (vertex_format_flags & vertex_attribute_morph_target)
			{
				ctx->read32<std::endian::little>(vertex_data_offset, 3);
				vertex_data_offset += sizeof(float) * 3;
			}
		}
	}
	
	// Allocate model
	std::unique_ptr<render::model> model = std::make_unique<render::model>();
	
	// Build model vertex buffer
	model->get_vertex_buffer() = std::make_shared<gl::vertex_buffer>(gl::buffer_usage::static_draw, vertex_data);
	model->set_vertex_offset(0);
	model->set_vertex_stride(vertex_stride);
	
	// Free vertex data
	vertex_data.clear();
	
	// Build vertex input attributes
	std::vector<gl::vertex_input_attribute> attributes(std::popcount(vertex_format_flags));
	
	std::uint32_t vertex_offset = 0;
	std::uint32_t attribute_index = 0;
	
	if (vertex_format_flags & vertex_attribute_position)
	{
		auto& attribute = attributes[attribute_index];
		attribute.location = render::vertex_attribute_location::position;
		attribute.binding = 0;
		attribute.format = gl::format::r32g32b32_sfloat;
		attribute.offset = vertex_offset;
		
		vertex_offset += 3 * sizeof(float);
		++attribute_index;
	}
	if (vertex_format_flags & vertex_attribute_uv)
	{
		auto& attribute = attributes[attribute_index];
		attribute.location = render::vertex_attribute_location::uv;
		attribute.binding = 0;
		attribute.format = gl::format::r32g32_sfloat;
		attribute.offset = vertex_offset;
		
		vertex_offset += 2 * sizeof(float);
		++attribute_index;
	}
	if (vertex_format_flags & vertex_attribute_normal)
	{
		auto& attribute = attributes[attribute_index];
		attribute.location = render::vertex_attribute_location::normal;
		attribute.binding = 0;
		attribute.format = gl::format::r32g32b32_sfloat;
		attribute.offset = vertex_offset;
		
		vertex_offset += 3 * sizeof(float);
		++attribute_index;
	}
	if (vertex_format_flags & vertex_attribute_tangent)
	{
		auto& attribute = attributes[attribute_index];
		attribute.location = render::vertex_attribute_location::tangent;
		attribute.binding = 0;
		attribute.format = gl::format::r32g32b32a32_sfloat;
		attribute.offset = vertex_offset;
		
		vertex_offset += 4 * sizeof(float);
		++attribute_index;
	}
	if (vertex_format_flags & vertex_attribute_color)
	{
		auto& attribute = attributes[attribute_index];
		attribute.location = render::vertex_attribute_location::color;
		attribute.binding = 0;
		attribute.format = gl::format::r32g32b32a32_sfloat;
		attribute.offset = vertex_offset;
		
		vertex_offset += 4 * sizeof(float);
		++attribute_index;
	}
	if (vertex_format_flags & vertex_attribute_bone_index)
	{
		auto& attribute = attributes[attribute_index];
		attribute.location = render::vertex_attribute_location::bone_index;
		attribute.binding = 0;
		switch (bones_per_vertex)
		{
			case 1:
				attribute.format = gl::format::r16_uint;
				break;
			case 2:
				attribute.format = gl::format::r16g16_uint;
				break;
			case 3:
				attribute.format = gl::format::r16g16b16_uint;
				break;
			case 4:
				attribute.format = gl::format::r16g16b16a16_uint;
				break;
			default:
				attribute.format = gl::format::undefined;
				break;
		}
		attribute.offset = vertex_offset;
		
		vertex_offset += bones_per_vertex * sizeof(std::uint16_t);
		++attribute_index;
	}
	if (vertex_format_flags & vertex_attribute_bone_weight)
	{
		auto& attribute = attributes[attribute_index];
		attribute.location = render::vertex_attribute_location::bone_weight;
		attribute.binding = 0;
		switch (bones_per_vertex)
		{
			case 1:
				attribute.format = gl::format::r32_sfloat;
				break;
			case 2:
				attribute.format = gl::format::r32g32_sfloat;
				break;
			case 3:
				attribute.format = gl::format::r32g32b32_sfloat;
				break;
			case 4:
				attribute.format = gl::format::r32g32b32a32_sfloat;
				break;
			default:
				attribute.format = gl::format::undefined;
				break;
		}
		attribute.offset = vertex_offset;
		
		vertex_offset += bones_per_vertex * sizeof(float);
		++attribute_index;
	}
	if (vertex_format_flags & vertex_attribute_morph_target)
	{
		auto& attribute = attributes[attribute_index];
		attribute.location = render::vertex_attribute_location::target;
		attribute.binding = 0;
		attribute.format = gl::format::r32g32b32_sfloat;
		attribute.offset = vertex_offset;
		
		// vertex_offset += 3 * sizeof(float);
		// ++attribute_index;
	}
	
	// Build model vertex array
	model->get_vertex_array() = std::make_shared<gl::vertex_array>(attributes);
	
	// Read model bounds
	ctx->read32<std::endian::little>(reinterpret_cast<std::byte*>(&model->get_bounds()), 6);
	
	// Read material count
	std::uint16_t material_count = 0;
	ctx->read16<std::endian::little>(reinterpret_cast<std::byte*>(&material_count), 1);
	
	// Allocate material groups
	model->get_groups().resize(material_count);
	
	// Read materials
	for (auto& group: model->get_groups())
	{
		// Read material name length
		std::uint8_t material_name_length = 0;
		ctx->read8(reinterpret_cast<std::byte*>(&material_name_length), 1);
		
		// Read material name
		std::string material_name(static_cast<std::size_t>(material_name_length), '\0');
		ctx->read8(reinterpret_cast<std::byte*>(material_name.data()), material_name_length);
		
		// Generate group ID by hashing material name
		group.id = hash::fnv1a32<char>(material_name);
		
		// Set group primitive topology
		group.primitive_topology = gl::primitive_topology::triangle_list;
		
		// Read index of first vertex
		ctx->read32<std::endian::little>(reinterpret_cast<std::byte*>(&group.first_vertex), 1);
		
		// Read vertex count
		ctx->read32<std::endian::little>(reinterpret_cast<std::byte*>(&group.vertex_count), 1);
		
		// Slugify material filename
		std::string material_filename = material_name + ".mtl";
		std::replace(material_filename.begin(), material_filename.end(), '_', '-');
		
		// Load group material
		group.material = resource_manager.load<render::material>(material_filename);
	}
	
	// Read skeleton
	if ((vertex_format_flags & vertex_attribute_bone_index) || (vertex_format_flags & vertex_attribute_bone_weight))
	{
		::skeleton& skeleton = model->get_skeleton();
		
		// Read bone count
		std::uint16_t bone_count = 0;
		ctx->read16<std::endian::little>(reinterpret_cast<std::byte*>(&bone_count), 1);
		
		// Allocate bones
		skeleton = ::skeleton(bone_count);

		// Read bones
		for (auto& bone: skeleton.bones())
		{
			// Read bone name length
			std::uint8_t bone_name_length = 0;
			ctx->read8(reinterpret_cast<std::byte*>(&bone_name_length), 1);

			// Read bone name
			std::string bone_name(static_cast<std::size_t>(bone_name_length), '\0');
			ctx->read8(reinterpret_cast<std::byte*>(bone_name.data()), bone_name_length);
			
			// Read bone parent index
			std::uint16_t bone_parent_index = 0;
			ctx->read16<std::endian::little>(reinterpret_cast<std::byte*>(&bone_parent_index), 1);
			
			// Construct bone pose
			math::transform<float> bone_pose;
			
			// Read bone translation
			ctx->read32<std::endian::little>(reinterpret_cast<std::byte*>(bone_pose.translation.data()), 3);
			
			// Read bone rotation
			ctx->read32<std::endian::little>(reinterpret_cast<std::byte*>(&bone_pose.rotation.r), 1);
			ctx->read32<std::endian::little>(reinterpret_cast<std::byte*>(bone_pose.rotation.i.data()), 3);
			
			// Set bone scale
			bone_pose.scale = {1, 1, 1};
			
			// Read bone length
			float bone_length = 0.0f;
			ctx->read32<std::endian::little>(reinterpret_cast<std::byte*>(&bone_length), 1);

			// Construct bone
			bone.rename(bone_name);
			bone.length() = bone_length;
			if (bone.index() != bone_parent_index)
			{
				bone.reparent(&skeleton.bones().at(bone_parent_index));
			}
			skeleton.rest_pose().set_relative_transform(bone.index(), bone_pose);
		}

		// Update rest pose
		skeleton.rest_pose().update();
	}
	
	return model;
}
