/*
 * Copyright (C) 2020  Christopher J. Howard
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

#ifndef ANTKEEPER_MODEL_HPP
#define ANTKEEPER_MODEL_HPP

#include "rasterizer/vertex-array.hpp"
#include "rasterizer/vertex-buffer.hpp"
#include "geometry/aabb.hpp"
#include <map>
#include <string>
#include <vector>

class material;
class skeleton;
class vertex_array;
class vertex_buffer;
enum class drawing_mode;

/**
 * Part of a model which is associated with exactly one material.
 */
class model_group
{
public:
	void set_material(material* material);
	void set_drawing_mode(drawing_mode mode);
	void set_start_index(std::size_t index);
	void set_index_count(std::size_t count);

	std::size_t get_index() const;
	const std::string& get_name() const;
	const material* get_material() const;
	material* get_material();
	drawing_mode get_drawing_mode() const;
	std::size_t get_start_index() const;
	std::size_t get_index_count() const;

private:
	friend class model;

	std::size_t index;
	std::string name;
	material* material;
	drawing_mode drawing_mode;
	std::size_t start_index;
	std::size_t index_count;
};

inline void model_group::set_material(::material* material)
{
	this->material = material;
}

inline void model_group::set_drawing_mode(::drawing_mode mode)
{
	this->drawing_mode = mode;
}

inline void model_group::set_start_index(std::size_t index)
{
	this->start_index = index;
}

inline void model_group::set_index_count(std::size_t count)
{
	this->index_count = count;
}

inline std::size_t model_group::get_index() const
{
	return index;
}

inline const std::string& model_group::get_name() const
{
	return name;
}

inline const material* model_group::get_material() const
{
	return material;
}

inline material* model_group::get_material()
{
	return material;
}

inline drawing_mode model_group::get_drawing_mode() const
{
	return drawing_mode;
}

inline std::size_t model_group::get_start_index() const
{
	return start_index;
}

inline std::size_t model_group::get_index_count() const
{
	return index_count;
}

/**
 *
 */
class model
{
public:
	model();
	~model();
	
	void set_bounds(const aabb<float>& bounds);

	model_group* add_group(const std::string& name = std::string());

	bool remove_group(const std::string& name);
	bool remove_group(model_group* group);
	
	const aabb<float>& get_bounds() const;

	const model_group* get_group(const std::string& name) const;
	model_group* get_group(const std::string& name);

	const std::vector<model_group*>* get_groups() const;

	const vertex_array* get_vertex_array() const;
	vertex_array* get_vertex_array();

	const vertex_buffer* get_vertex_buffer() const;
	vertex_buffer* get_vertex_buffer();

private:
	aabb<float> bounds;
	std::vector<model_group*> groups;
	std::map<std::string, model_group*> group_map;
	vertex_array vao;
	vertex_buffer vbo;
	skeleton* skeleton;
};

inline void model::set_bounds(const aabb<float>& bounds)
{
	this->bounds = bounds;
}

inline const aabb<float>& model::get_bounds() const
{
	return bounds;
}

inline const std::vector<model_group*>* model::get_groups() const
{
	return &groups;
}

inline const vertex_array* model::get_vertex_array() const
{
	return &vao;
}

inline vertex_array* model::get_vertex_array()
{
	return &vao;
}

inline const vertex_buffer* model::get_vertex_buffer() const
{
	return &vbo;
}

inline vertex_buffer* model::get_vertex_buffer()
{
	return &vbo;
}

#endif // ANTKEEPER_MODEL_HPP

