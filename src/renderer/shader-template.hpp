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

#ifndef ANTKEEPER_SHADER_TEMPLATE_HPP
#define ANTKEEPER_SHADER_TEMPLATE_HPP

#include "gl/shader-object.hpp"
#include "gl/shader-program.hpp"
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

/**
 * Shader templates can be used to generate multiple shader variants from a single source.
 *
 * Shader templates support the following preprocessor directives:
 *
 * * `#pragma vertex`: Replaced with `#define __VERTEX__` when generating vertex shader objects.
 * * `#pragma fragment`: Replaced with `#define __FRAGMENT__` when generating fragment shader objects.
 * * `#pragma geometry`: Replaced with `#define __GEOMETRY__` when generating geometry shader objects.
 * * `#pragma define <key>`: Will be replaced with `#define <key> <value>` if its definition is passed to the shader template.
 *
 * @see gl::shader_stage
 * @see gl::shader_object
 * @see gl::shader_program
 */
class shader_template
{
public:
	/// Container of definitions used to replace `#pragma define <key> <value>` directives.
	typedef std::unordered_map<std::string, std::string> dictionary_type;
	
	/**
	 * Creates a shader template and sets its source code.
	 *
	 * @param source_code String containing the shader template source code.
	 *
	 * @see shader_template::source(const std::string&)
	 */
	shader_template(const std::string& source_code);
	
	/**
	 * Creates a shader template.
	 */
	shader_template();
	
	/**
	 * Replaces the source code of the shader template.
	 *
	 * @param source_code String containing shader template source code.
	 */
	void source(const std::string& source_code);
	
	/**
	 * Configures shader object source code given a shader stage and template dictionary.
	 *
	 * @param stage Shader stage of the shader object to generate. Instances of `#pragma <stage>` in the template source will be replaced with `#define __<STAGE>__`.
	 * @param definitions Container of definitions used to replace `#pragma define <key> <value>` directives.
	 * @return Configured shader object source code.
	 */
	std::string configure(gl::shader_stage stage, const dictionary_type& definitions) const;
	
	/**
	 * Configures and compiles a shader object.
	 *
	 * @param stage Shader stage of the shader object to generate. Instances of `#pragma <stage>` in the template source will be replaced with `#define __<STAGE>__`.
	 * @param definitions Container of definitions used to replace `#pragma define <key> <value>` directives.
	 * @return Compiled shader object.
	 *
	 * @exception std::runtime_error Any exceptions thrown by gl::shader_object.
	 */
	gl::shader_object* compile(gl::shader_stage stage, const dictionary_type& definitions) const;
	
	/**
	 * Configures and compiles shader objects, then links them into a shader program. Shader object stages are determined according to the presence of `#pragma <stage>` directives.
	 *
	 * @param definitions Container of definitions used to replace `#pragma define <key> <value>` directives.
	 * @return Linked shader program.
	 *
	 * @exception std::runtime_error Any exceptions thrown by gl::shader_object or gl::shader_program.
	 *
	 * @see has_vertex_directive() const
	 * @see has_fragment_directive() const
	 * @see has_geometry_directive() const
	 */
	gl::shader_program* build(const dictionary_type& definitions) const;
	
	/// Returns `true` if the template source contains one or more `#pragma vertex` directive.
	bool has_vertex_directive() const;
	
	/// Returns `true` if the template source contains one or more `#pragma fragment` directive.
	bool has_fragment_directive() const;
	
	/// Returns `true` if the template source contains one or more `#pragma geometry` directive.
	bool has_geometry_directive() const;
	
	/**
	 * Returns `true` if the template source contains one or more instance of `#pragma define <key>`.
	 *
	 * @param key Definition key.
	 */
	bool has_define_directive(const std::string& key) const;
	
private:
	void replace_stage_directives(gl::shader_stage stage) const;
	void replace_define_directives(const dictionary_type& definitions) const;
	
	mutable std::vector<std::string> template_source;
	std::unordered_set<std::size_t> vertex_directives;
	std::unordered_set<std::size_t> fragment_directives;
	std::unordered_set<std::size_t> geometry_directives;
	std::multimap<std::string, std::size_t> define_directives;
};

#endif // ANTKEEPER_SHADER_TEMPLATE_HPP
