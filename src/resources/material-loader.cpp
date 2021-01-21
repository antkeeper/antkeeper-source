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

#include "renderer/material.hpp"
#include "resource-loader.hpp"
#include "resource-manager.hpp"
#include "gl/shader-variable-type.hpp"
#include "gl/texture-wrapping.hpp"
#include "gl/texture-filter.hpp"
#include "gl/texture-2d.hpp"
#include "utility/fundamental-types.hpp"
#include "string-table.hpp"
#include <cctype>
#include <vector>
#include <map>

static const std::map<std::string, gl::texture_wrapping> texture_wrapping_map =
{
	{"clamp", gl::texture_wrapping::clamp},
	{"repeat", gl::texture_wrapping::repeat},
	{"mirrored_repeat", gl::texture_wrapping::mirrored_repeat},
};

static const std::map<std::string, gl::texture_min_filter> texture_min_filter_map =
{
	{"nearest", gl::texture_min_filter::nearest},
	{"linear", gl::texture_min_filter::linear},
	{"nearest_mipmap_nearest", gl::texture_min_filter::nearest_mipmap_nearest},
	{"linear_mipmap_nearest", gl::texture_min_filter::linear_mipmap_nearest},
	{"nearest_mipmap_linear", gl::texture_min_filter::nearest_mipmap_linear},
	{"linear_mipmap_linear", gl::texture_min_filter::linear_mipmap_linear}
};

static const std::map<std::string, gl::texture_mag_filter> texture_mag_filter_map =
{
	{"nearest", gl::texture_mag_filter::nearest},
	{"linear", gl::texture_mag_filter::linear},
};

static bool load_bool_property(material* material, const string_table_row& row, int vector_size, int array_size)
{
	if (row.size() - 4 != vector_size * array_size || vector_size < 1 || vector_size > 4)
	{
		return false;
	}

	std::size_t size = array_size * vector_size;
	bool* values = new bool[size];
	for (std::size_t i = 0; i < size; ++i)
	{
		values[i] = (std::stoi(row[4 + i]) != 0);
	}

	if (vector_size == 1)
	{
		material_property<bool>* property = material->add_property<bool>(row[1], array_size);
		property->set_values(0, values, array_size);
	}
	else if (vector_size == 2)
	{
		material_property<bool2>* property = material->add_property<bool2>(row[1], array_size);
		property->set_values(0, reinterpret_cast<const bool2*>(values), array_size);
	}
	else if (vector_size == 3)
	{
		material_property<bool3>* property = material->add_property<bool3>(row[1], array_size);
		property->set_values(0, reinterpret_cast<const bool3*>(values), array_size);
	}
	else if (vector_size == 4)
	{
		material_property<bool4>* property = material->add_property<bool4>(row[1], array_size);
		property->set_values(0, reinterpret_cast<const bool4*>(values), array_size);
	}

	delete[] values;

	return true;
}

static bool load_int_property(material* material, const string_table_row& row, int vector_size, int array_size)
{
	if (row.size() - 4 != vector_size * array_size || vector_size < 1 || vector_size > 4)
	{
		return false;
	}

	std::size_t size = array_size * vector_size;
	int* values = new int[size];
	for (std::size_t i = 0; i < size; ++i)
	{
		values[i] = std::stoi(row[4 + i]);
	}

	if (vector_size == 1)
	{
		material_property<int>* property = material->add_property<int>(row[1], array_size);
		property->set_values(0, values, array_size);
	}
	else if (vector_size == 2)
	{
		material_property<int2>* property = material->add_property<int2>(row[1], array_size);
		property->set_values(0, reinterpret_cast<const int2*>(values), array_size);
	}
	else if (vector_size == 3)
	{
		material_property<int3>* property = material->add_property<int3>(row[1], array_size);
		property->set_values(0, reinterpret_cast<const int3*>(values), array_size);
	}
	else if (vector_size == 4)
	{
		material_property<int4>* property = material->add_property<int4>(row[1], array_size);
		property->set_values(0, reinterpret_cast<const int4*>(values), array_size);
	}

	delete[] values;

	return true;
}

static bool load_uint_property(material* material, const string_table_row& row, int vector_size, int array_size)
{
	if (row.size() - 4 != vector_size * array_size || vector_size < 1 || vector_size > 4)
	{
		return false;
	}

	std::size_t size = array_size * vector_size;
	unsigned int* values = new unsigned int[size];
	for (std::size_t i = 0; i < size; ++i)
	{
		values[i] = static_cast<unsigned int>(std::stoul(row[4 + i]));
	}

	if (vector_size == 1)
	{
		material_property<unsigned int>* property = material->add_property<unsigned int>(row[1], array_size);
		property->set_values(0, values, array_size);
	}
	else if (vector_size == 2)
	{
		material_property<uint2>* property = material->add_property<uint2>(row[1], array_size);
		property->set_values(0, reinterpret_cast<const uint2*>(values), array_size);
	}
	else if (vector_size == 3)
	{
		material_property<uint3>* property = material->add_property<uint3>(row[1], array_size);
		property->set_values(0, reinterpret_cast<const uint3*>(values), array_size);
	}
	else if (vector_size == 4)
	{
		material_property<uint4>* property = material->add_property<uint4>(row[1], array_size);
		property->set_values(0, reinterpret_cast<const uint4*>(values), array_size);
	}

	delete[] values;

	return true;
}

static bool load_float_property(material* material, const string_table_row& row, int vector_size, int array_size)
{
	if (row.size() - 4 != vector_size * array_size || vector_size < 1 || vector_size > 4)
	{
		return false;
	}

	std::size_t size = array_size * vector_size;
	float* values = new float[size];
	for (std::size_t i = 0; i < size; ++i)
	{
		values[i] = static_cast<float>(std::stod(row[4 + i]));
	}

	if (vector_size == 1)
	{
		material_property<float>* property = material->add_property<float>(row[1], array_size);
		property->set_values(0, values, array_size);
	}
	else if (vector_size == 2)
	{
		material_property<float2>* property = material->add_property<float2>(row[1], array_size);
		property->set_values(0, reinterpret_cast<const float2*>(values), array_size);
	}
	else if (vector_size == 3)
	{
		material_property<float3>* property = material->add_property<float3>(row[1], array_size);
		property->set_values(0, reinterpret_cast<const float3*>(values), array_size);
	}
	else if (vector_size == 4)
	{
		material_property<float4>* property = material->add_property<float4>(row[1], array_size);
		property->set_values(0, reinterpret_cast<const float4*>(values), array_size);
	}

	delete[] values;

	return true;
}

static bool load_float_matrix_property(material* material, const string_table_row& row, int matrix_columns, int matrix_rows, int array_size)
{
	int matrix_size = matrix_columns * matrix_rows;

	if (row.size() - 4 != matrix_size * array_size)
	{
		return false;
	}

	std::size_t size = array_size * matrix_size;
	float* values = new float[size];
	for (std::size_t i = 0; i < size; ++i)
	{
		values[i] = static_cast<float>(std::stod(row[4 + i]));
	}

	if (matrix_size == 2*2)
	{
		material_property<float2x2>* property = material->add_property<float2x2>(row[1], array_size);
		property->set_values(0, reinterpret_cast<const float2x2*>(values), array_size);
	}
	else if (matrix_size == 3*3)
	{
		material_property<float3x3>* property = material->add_property<float3x3>(row[1], array_size);
		property->set_values(0, reinterpret_cast<const float3x3*>(values), array_size);
	}
	else if (matrix_size == 4*4)
	{
		material_property<float4x4>* property = material->add_property<float4x4>(row[1], array_size);
		property->set_values(0, reinterpret_cast<const float4x4*>(values), array_size);
	}

	delete[] values;

	return true;
}

static bool load_texture_2d_property(material* material, const string_table_row& row, resource_manager* resource_manager, int array_size)
{
	if (row.size() - 4 != array_size * 6)
	{
		return false;
	}

	const gl::texture_2d** values = new const gl::texture_2d*[array_size];
	for (std::size_t i = 0; i < array_size; ++i)
	{
		std::size_t offset = 4 + i;

		const std::string& filename = row[offset];
		gl::texture_wrapping wrap_s = gl::texture_wrapping::clamp;
		gl::texture_wrapping wrap_t = gl::texture_wrapping::clamp;
		gl::texture_min_filter min_filter = gl::texture_min_filter::linear_mipmap_linear;
		gl::texture_mag_filter mag_filter = gl::texture_mag_filter::linear;
		if (auto it = texture_wrapping_map.find(row[offset + 1]); it != texture_wrapping_map.end())
		{
			wrap_s = it->second;
		}
		if (auto it = texture_wrapping_map.find(row[offset + 2]); it != texture_wrapping_map.end())
		{
			wrap_t = it->second;
		}
		if (auto it = texture_min_filter_map.find(row[offset + 3]); it != texture_min_filter_map.end())
		{
			min_filter = it->second;
		}
		if (auto it = texture_mag_filter_map.find(row[offset + 4]); it != texture_mag_filter_map.end())
		{
			mag_filter = it->second;
		}
		float max_anisotropy = static_cast<float>(std::stod(row[offset + 5]));

		gl::texture_2d* texture = resource_manager->load<gl::texture_2d>(row[4 + i]);
		texture->set_wrapping(wrap_s, wrap_t);
		texture->set_filters(min_filter, mag_filter);
		texture->set_max_anisotropy(max_anisotropy);

		values[i] = texture;
	}
	
	material_property<const gl::texture_2d*>* property = material->add_property<const gl::texture_2d*>(row[1], array_size);
	property->set_values(0, values, array_size);

	delete[] values;

	return true;
}

static bool load_texture_cube_property(material* material, const string_table_row& row, resource_manager* resource_manager, int array_size)
{
	return false;
}

static bool load_material_property(material* material, const string_table_row& row, resource_manager* resource_manager)
{
	// Ensure row has at least five columns
	if (row.size() < 5)
	{
		return false;
	}

	const std::string& name = row[1];
	if (name.empty())
	{
		return false;
	}

	const std::string& type = row[2];
	if (type.empty())
	{
		return false;
	}

	int vector_size = 1;
	if (std::isdigit(type.back()))
	{
		vector_size = std::stoi(type.substr(type.size() - 1, 1));
	}

	int matrix_columns = 0;
	int matrix_rows = 0;
	if (type[type.size() - 2] == 'x' && std::isdigit(type[type.size() - 3]) && std::isdigit(type.back()))
	{
		matrix_columns = std::stoi(type.substr(type.size() - 3, 1));
		matrix_rows = std::stoi(type.substr(type.size() - 1, 1));
	}

	int array_size = std::stoi(row[3]);
	if (array_size <= 0)
	{
		return false;
	}

	if (type == "bool" || type == "bool2" || type == "bool3" || type == "bool4")
	{
		return load_bool_property(material, row, vector_size, array_size);
	}
	else if (type == "int" || type == "int2" || type == "int3" || type == "int4")
	{
		return load_int_property(material, row, vector_size, array_size);
	}
	else if (type == "uint" || type == "uint2" || type == "uint3" || type == "uint4")
	{
		return load_uint_property(material, row, vector_size, array_size);
	}
	else if (type == "float" || type == "float2" || type == "float3" || type == "float4")
	{
		return load_float_property(material, row, vector_size, array_size);
	}
	else if (type == "float2x2" || type == "float3x3" || type == "float4x4")
	{
		return load_float_matrix_property(material, row, matrix_columns, matrix_rows, array_size);
	}
	else if (type == "texture_2d")
	{
		return load_texture_2d_property(material, row, resource_manager, array_size);
	}
	else if (type == "texture_cube")
	{
		return load_texture_cube_property(material, row, resource_manager, array_size);
	}

	return false;
}

template <>
material* resource_loader<material>::load(resource_manager* resource_manager, PHYSFS_File* file)
{
	// Load string table from input stream
	string_table* table = resource_loader<string_table>::load(resource_manager, file);

	// Ensure table is not empty.
	if (!table || table->empty())
	{
		delete table;
		return nullptr;
	}

	// Allocate material
	::material* material = new ::material();

	// Parse table rows
	for (const string_table_row& row: *table)
	{
		// Skip empty rows and comments
		if (row.empty() || row[0].empty() || row[0][0] == '#')
		{
			continue;
		}

		if (row[0] == "shader" && row.size() == 2)
		{
			gl::shader_program* program = resource_manager->load<gl::shader_program>(row[1]);
			material->set_shader_program(program);
		}
		else if (row[0] == "flags" && row.size() == 2)
		{
			std::uint32_t flags = std::stoi(row[1]);
			material->set_flags(flags);
		}
		else if (row[0] == "property")
		{
			load_material_property(material, row, resource_manager);
		}
	}
	
	return material;
}

