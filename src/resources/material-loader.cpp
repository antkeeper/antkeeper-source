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

#include "resource-loader.hpp"
#include "resource-manager.hpp"
#include "render/material.hpp"
#include "render/material-flags.hpp"
#include <nlohmann/json.hpp>
#include <physfs.h>
#include <utility>
#include <type_traits>
#include <string>

template <typename T>
static bool read_value(T* value, const nlohmann::json& json, const std::string& name)
{
	if (auto element = json.find(name); element != json.end())
	{
		*value = element.value().get<T>();
		return true;
	}
	
	return false;
}

static bool load_texture_1d_property(resource_manager* resource_manager, render::material* material, const std::string& name, const nlohmann::json& json)
{
	// If JSON element is an array
	if (json.is_array())
	{
		// Determine size of the array
		std::size_t array_size = json.size();
		
		// Create property
		render::material_property<const gl::texture_1d*>* property = material->add_property<const gl::texture_1d*>(name, array_size);
		
		// Load textures
		std::size_t i = 0;
		for (const auto& element: json)
		{
			std::string filename = element.get<std::string>();
			const gl::texture_1d* texture = resource_manager->load<gl::texture_1d>(filename);
			property->set_value(i++, texture);
		}
	}
	else
	{
		// Create property
		render::material_property<const gl::texture_1d*>* property = material->add_property<const gl::texture_1d*>(name);
		
		// Load texture
		std::string filename = json.get<std::string>();
		const gl::texture_1d* texture = resource_manager->load<gl::texture_1d>(filename);
		property->set_value(texture);
	}
	
	return true;
}

static bool load_texture_2d_property(resource_manager* resource_manager, render::material* material, const std::string& name, const nlohmann::json& json)
{
	// If JSON element is an array
	if (json.is_array())
	{
		// Determine size of the array
		std::size_t array_size = json.size();
		
		// Create property
		render::material_property<const gl::texture_2d*>* property = material->add_property<const gl::texture_2d*>(name, array_size);
		
		// Load textures
		std::size_t i = 0;
		for (const auto& element: json)
		{
			std::string filename = element.get<std::string>();
			const gl::texture_2d* texture = resource_manager->load<gl::texture_2d>(filename);
			property->set_value(i++, texture);
		}
	}
	else
	{
		// Create property
		render::material_property<const gl::texture_2d*>* property = material->add_property<const gl::texture_2d*>(name);
		
		// Load texture
		std::string filename = json.get<std::string>();
		const gl::texture_2d* texture = resource_manager->load<gl::texture_2d>(filename);
		property->set_value(texture);
	}
	
	return true;
}

static bool load_texture_cube_property(resource_manager* resource_manager, render::material* material, const std::string& name, const nlohmann::json& json)
{
	return false;
}

template <typename T>
static bool load_scalar_property(render::material* material, const std::string& name, const nlohmann::json& json)
{
	// If JSON element is an array
	if (json.is_array())
	{
		// Determine size of the array
		std::size_t array_size = json.size();
		
		// Create property
		render::material_property<T>* property = material->add_property<T>(name, array_size);
		
		// Set property values
		std::size_t i = 0;
		for (const auto& element: json)
			property->set_value(i++, element.get<T>());
	}
	else
	{
		// Create property
		render::material_property<T>* property = material->add_property<T>(name);
		
		// Set property value
		property->set_value(json.get<T>());
	}
	
	return true;
}

template <typename T>
static bool load_vector_property(render::material* material, const std::string& name, std::size_t vector_size, const nlohmann::json& json)
{
	// If JSON element is an array of arrays
	if (json.is_array() && json.begin().value().is_array())
	{
		// Determine size of the array
		std::size_t array_size = json.size();
		
		// Create property
		render::material_property<T>* property = material->add_property<T>(name, array_size);
		
		// For each vector in the array
		std::size_t i = 0;
		for (const auto& vector_element: json)
		{
			// Read vector elements
			T value;
			std::size_t j = 0;
			for (const auto& value_element: vector_element)
				value[j++] = value_element.get<typename T::element_type>();
			
			// Set property values
			property->set_value(i++, value);
		}
	}
	else
	{
		// Create property
		render::material_property<T>* property = material->add_property<T>(name);
		
		// Read vector elements
		T value;
		std::size_t i = 0;
		for (const auto& value_element: json)
			value[i++] = value_element.get<typename T::element_type>();
		
		// Set property values
		property->set_value(value);
	}
	
	return true;
}

template <typename T>
static bool load_matrix_property(render::material* material, const std::string& name, std::size_t column_count, std::size_t row_count, const nlohmann::json& json)
{
	// If JSON element is an array of arrays of arrays
	if (json.is_array() && json.begin().value().is_array())
	{
		if (json.begin().value().begin().value().is_array())
		{
			// Determine size of the array
			std::size_t array_size = json.size();
			
			// Create property
			render::material_property<T>* property = material->add_property<T>(name, array_size);
			
			// For each matrix in the array
			std::size_t i = 0;
			for (const auto& matrix_element: json)
			{
				// Read vector elements
				T value;
				std::size_t j = 0;
				for (const auto& column_element: matrix_element)
				{
					std::size_t k = 0;
					for (const auto& row_element: column_element)
					{
						value[j][k] = row_element.get<typename T::element_type>();
						++k;
					}
					
					++j;
				}
				
				// Set property values
				property->set_value(i, value);
				
				++i;
			}
			
			return true;
		}
		else
		{
			// Create property
			render::material_property<T>* property = material->add_property<T>(name);
			
			// Read matrix elements
			T value;
			std::size_t i = 0;
			for (const auto& column_element: json)
			{
				std::size_t j = 0;
				for (const auto& row_element: column_element)
				{
					value[i][j] = row_element.get<typename T::element_type>();
					++j;
				}
				
				++i;
			}
			
			// Set property values
			property->set_value(value);
			
			return true;
		}
	}
	
	return false;
}

template <>
render::material* resource_loader<render::material>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Read file into buffer
	std::size_t size = static_cast<int>(PHYSFS_fileLength(file));
	std::string buffer;
	buffer.resize(size);
	PHYSFS_readBytes(file, &buffer[0], size);
	
	// Parse json from file buffer
	nlohmann::json json = nlohmann::json::parse(buffer, nullptr, true, true);
	
	// Allocate material
	render::material* material = new render::material();
	
	// Read shader filename
	std::string shader_filename;
	if (read_value(&shader_filename, json, "shader"))
	{
		// Load shader program
		gl::shader_program* program = resource_manager->load<gl::shader_program>(shader_filename);
		material->set_shader_program(program);
	}
	
	// Init material flags
	std::uint32_t flags = 0;
	
	// Read blend mode
	std::string blend_mode;
	read_value(&blend_mode, json, "blend_mode");
	if (blend_mode == "opaque")
	{
		material->set_blend_mode(render::blend_mode::opaque);
	}
	else if (blend_mode == "masked")
	{
		material->set_blend_mode(render::blend_mode::masked);
	}
	else if (blend_mode == "translucent")
	{
		material->set_blend_mode(render::blend_mode::translucent);
	}
	
	// Read opacity threshold
	float opacity_threshold = 0.5f;
	if (read_value(&opacity_threshold, json, "opacity_threshold"))
	{
		material->set_opacity_threshold(opacity_threshold);
	}
	
	// Read two sided
	bool two_sided = false;
	read_value(&two_sided, json, "two_sided");
	material->set_two_sided(two_sided);
	
	// Read shadow mode
	std::string shadow_mode;
	read_value(&shadow_mode, json, "shadow_mode");
	if (shadow_mode == "opaque")
	{
		material->set_shadow_mode(render::shadow_mode::opaque);
	}
	else if (shadow_mode == "none")
	{
		material->set_shadow_mode(render::shadow_mode::none);
	}
	
	// Read depth mode
	std::string depth_mode;
	read_value(&depth_mode, json, "depth_mode");
	if (depth_mode == "in_front")
		flags |= MATERIAL_FLAG_X_RAY;
	
	// Read decal mode
	std::string decal_mode;
	read_value(&decal_mode, json, "decal_mode");
	if (decal_mode == "decal")
		flags |= MATERIAL_FLAG_DECAL;
	else if (decal_mode == "surface")
		flags |= MATERIAL_FLAG_DECAL_SURFACE;
	
	// Set material flags
	material->set_flags(flags);
	
	// Read material properties
	if (auto properties_element = json.find("properties"); properties_element != json.end())
	{
		for (const auto& property_element: properties_element.value())
		{
			// Read property name
			std::string name;
			if (!read_value(&name, property_element, "name"))
				// Ignore nameless properties
				continue;
			
			// Read property type
			std::string type;
			if (!read_value(&type, property_element, "type"))
				// Ignore typeless properties
				continue;
			
			// Find value element
			auto value_element = property_element.find("value");
			if (value_element == property_element.end())
				// Ignore valueless properties
				continue;
			
			if (type == "texture_1d")
			{
				load_texture_1d_property(resource_manager, material, name, value_element.value());
			}
			else if (type == "texture_2d")
			{
				load_texture_2d_property(resource_manager, material, name, value_element.value());
			}
			else if (type == "texture_cube")
			{
				load_texture_cube_property(resource_manager, material, name, value_element.value());
			}
			// If property type is a matrix
			else if (type[type.size() - 2] == 'x' &&
				std::isdigit(type[type.size() - 3]) &&
				std::isdigit(type.back()))
			{
				std::size_t columns = std::stoul(type.substr(type.size() - 3, 1));
				std::size_t rows = std::stoul(type.substr(type.size() - 1, 1));
				
				if (type.find("float") != std::string::npos)
				{
					if (size == 2)
						load_matrix_property<float2x2>(material, name, columns, rows, value_element.value());
					else if (size == 3)
						load_matrix_property<float3x3>(material, name, columns, rows, value_element.value());
					else if (size == 4)
						load_matrix_property<float4x4>(material, name, columns, rows, value_element.value());
				}
			}
			// If property type is a vector
			else if (std::isdigit(type.back()))
			{
				std::size_t size = std::stoul(type.substr(type.size() - 1, 1));
				
				if (type.find("float") != std::string::npos)
				{
					if (size == 2)
						load_vector_property<float2>(material, name, size, value_element.value());
					else if (size == 3)
						load_vector_property<float3>(material, name, size, value_element.value());
					else if (size == 4)
						load_vector_property<float4>(material, name, size, value_element.value());
				}
				else if (type.find("uint") != std::string::npos)
				{
					if (size == 2)
						load_vector_property<uint2>(material, name, size, value_element.value());
					else if (size == 3)
						load_vector_property<uint3>(material, name, size, value_element.value());
					else if (size == 4)
						load_vector_property<uint4>(material, name, size, value_element.value());
				}
				else if (type.find("int") != std::string::npos)
				{
					if (size == 2)
						load_vector_property<int2>(material, name, size, value_element.value());
					else if (size == 3)
						load_vector_property<int3>(material, name, size, value_element.value());
					else if (size == 4)
						load_vector_property<int4>(material, name, size, value_element.value());
				}
				else if (type.find("bool") != std::string::npos)
				{
					if (size == 2)
						load_vector_property<bool2>(material, name, size, value_element.value());
					else if (size == 3)
						load_vector_property<bool3>(material, name, size, value_element.value());
					else if (size == 4)
						load_vector_property<bool4>(material, name, size, value_element.value());
				}
			}
			// If property type is a scalar
			else
			{
				if (type.find("float") != std::string::npos)
					load_scalar_property<float>(material, name, value_element.value());
				else if (type.find("uint") != std::string::npos)
					load_scalar_property<unsigned int>(material, name, value_element.value());
				else if (type.find("int") != std::string::npos)
					load_scalar_property<int>(material, name, value_element.value());
				else if (type.find("bool") != std::string::npos)
					load_scalar_property<bool>(material, name, value_element.value());
			}
		}
	}
	
	// Update material tweens
	material->update_tweens();
	
	return material;
}
